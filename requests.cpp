#include <stdlib.h> /* exit, atoi, malloc, free */
#include <stdio.h>
#include <iostream>
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>
#include "helpers.h"
#include "requests.h"
#include "nlohmann/json.hpp"

#define ADDR "34.241.4.235"
#define PORT 8080
#define STRLEN 256

char *parser(char *response, char *retPointer, char const *delimiter)
{
    // ERROR incoming from server
    if (strstr(response, "HTTP/1.1 2") == NULL)
    {
        nlohmann::json resp = nlohmann::json::parse(strstr(response, "\r\n\r\n") + 4);
        std::cout << "ERROR: " << resp["error"] << "\n";
        return NULL;
    }
    // SUCCESS response
    else
    {
        std::cout << "Success!\n";
        return retPointer != NULL && delimiter != NULL ? strtok(retPointer, delimiter) : NULL;
    }
}

char *compute_get_request(char const *url, char *cookies, char *token)
{
    char *message = (char *)calloc(BUFLEN, sizeof(char));
    char *line = (char *)calloc(LINELEN, sizeof(char));

    // HEADER
    sprintf(line, "GET %s HTTP/1.1", url);
    compute_message(message, line);

    // HOST
    sprintf(line, "Host: %s", ADDR);
    compute_message(message, line);

    // COOKIES (optional)
    if (cookies != NULL)
    {
        sprintf(line, "Cookie: %s", cookies);
        compute_message(message, line);
    }

    // JWT TOKEN (optional)
    if (token != NULL)
    {
        sprintf(line, "Authorization: Bearer %s", token);
        compute_message(message, line);
    }

    // final "/r/n/r/n"
    compute_message(message, "");
    free(line);
    return message;
}

char *compute_delete_request(char const *url, char *JWT)
{
    char *message = (char *)calloc(BUFLEN, sizeof(char));
    char *line = (char *)calloc(LINELEN, sizeof(char));

    // HEADER
    sprintf(line, "DELETE %s HTTP/1.1", url);
    compute_message(message, line);

    // HOST
    sprintf(line, "Host: %s", ADDR);
    compute_message(message, line);

    // JWT TOKEN (optional)
    if (JWT != NULL)
    {
        sprintf(line, "Authorization: Bearer %s", JWT);
        compute_message(message, line);
    }

    // final "/r/n/r/n"
    compute_message(message, "");
    free(line);
    return message;
}

char *compute_post_request(char const *url, char *body_data, char *JWT)
{
    char *message = (char *)calloc(BUFLEN, sizeof(char));
    char *line = (char *)calloc(LINELEN, sizeof(char));

    // HEADER
    sprintf(line, "POST %s HTTP/1.1", url);
    compute_message(message, line);

    // HOST
    sprintf(line, "Host: %s", ADDR);
    compute_message(message, line);

    sprintf(line, "Content-Length: %ld", strlen(body_data));
    compute_message(message, line);

    sprintf(line, "Content-Type: application/json");
    compute_message(message, line);

    // JWT TOKEN (optional)
    if (JWT != NULL)
    {
        sprintf(line, "Authorization: Bearer %s", JWT);
        compute_message(message, line);
    }

    compute_message(message, "");
    compute_message(message, body_data);

    free(line);
    return message;
}

void registerToServer(int sockfd, char *username, char *password)
{
    nlohmann::json jason;
    char body[BUFLEN];

    // parse JSON request
    jason["username"] = username;
    jason["password"] = password;

    strcpy(body, jason.dump(4).c_str());

    // send request and await server's response
    char *message = compute_post_request("/api/v1/tema/auth/register", body, NULL);
    send_to_server(sockfd, message);
    char *response = receive_from_server(sockfd);

    // parse server's response
    parser(response, NULL, NULL);
}

char *loginToServer(int sockfd, char *username, char *password)
{
    nlohmann::json jason;
    char body[BUFLEN];

    // parse JSON request
    jason["username"] = username;
    jason["password"] = password;

    strcpy(body, jason.dump(4).c_str());

    // send request and await server's response
    char *message = compute_post_request("/api/v1/tema/auth/login", body, NULL);
    send_to_server(sockfd, message);
    char *response = receive_from_server(sockfd);

    // parse server's response
    return parser(response, strstr(response, "connect.sid"), ";");
}

char *enterLibrary(int sockfd, char *cookie)
{
    // send request and await server's response
    char *message = compute_get_request("/api/v1/tema/library/access", cookie, NULL);
    send_to_server(sockfd, message);
    char *response = receive_from_server(sockfd);

    // parse server's response
    return parser(response, strstr(response, "token") + 8, "\"");
}

void getBooks(int sockfd, char *token)
{
    // send request and await server's response
    char *message = compute_get_request("/api/v1/tema/library/books", NULL, token);
    send_to_server(sockfd, message);
    char *response = receive_from_server(sockfd);

    // parse server's response
    nlohmann::json resp = nlohmann::json::parse(strstr(response, "\r\n\r\n"));

    for (auto i : resp)
    {
        std::cout << i["id"] << ". " << i["title"] << "\n";
    }
}

void getBook(int sockfd, char *token, char *id)
{
    // compute the right url
    char url[STRLEN];
    sprintf(url, "/api/v1/tema/library/books/%s", id);

    // send request and await server's response
    char *message = compute_get_request(url, NULL, token);
    send_to_server(sockfd, message);
    char *response = receive_from_server(sockfd);

    // parse server's response
    parser(response, NULL, NULL);

    if (strcmp(strstr(response, "\r\n\r\n") + 4, "{\"error\":\"No book was found!\"}") != 0)
    {
        nlohmann::json resp = nlohmann::json::parse(strstr(response, "\r\n\r\n"));

        for (auto i : resp)
            std::cout << "\ntitle: " << i["title"] << "\nauthor: " << i["author"] << "\npublisher: " << i["publisher"] << "\ngenre: " << i["genre"] << "\npage_count: " << i["page_count"] << "\n\n";
    }
}

void addBook(int sockfd, char *token, char *title, char *author, char *genre, char *publisher, char *page_count)
{
    nlohmann::json jason;
    char body[BUFLEN];

    // parse JSON request
    jason["title"] = title;
    jason["author"] = author;
    jason["genre"] = genre;
    jason["publisher"] = publisher;
    jason["page_count"] = page_count;

    strcpy(body, jason.dump(4).c_str());

    // send request and await server's response
    char *message = compute_post_request("/api/v1/tema/library/books", body, token);
    send_to_server(sockfd, message);
    char *response = receive_from_server(sockfd);

    // parse server's response
    parser(response, NULL, NULL);
}

void deleteBook(int sockfd, char *token, char *id)
{
    // compute the right url
    char url[STRLEN];
    sprintf(url, "/api/v1/tema/library/books/%s", id);

    // send request and await server's response
    char *message = compute_delete_request(url, token);
    send_to_server(sockfd, message);
    char *response = receive_from_server(sockfd);

    // parse server's response
    parser(response, NULL, NULL);
}

void logout(int sockfd, char *cookie)
{
    // send request and await server's response
    char *message = compute_get_request("/api/v1/tema/auth/logout", cookie, NULL);
    send_to_server(sockfd, message);
    char *response = receive_from_server(sockfd);

    // parse server's response
    parser(response, NULL, NULL);
}