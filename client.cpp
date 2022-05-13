#include <stdio.h> /* printf, sprintf */
#include <iostream>
#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>
#include "helpers.h"
#include "requests.h"

#define STRLEN 256
#define ADDR "34.241.4.235"
#define PORT 8080

int main(int argc, char *argv[])
{
    char command[STRLEN], username[STRLEN], password[STRLEN];
    char *cookie = NULL, *token = NULL;

    std::cout << "Hello there, just connected to the server. What would you like to do next?\n";

    while (true)
    {
        std::cout << "Input command: ";
        std::cin.getline(command, STRLEN);

        int sockfd = open_connection(ADDR, PORT, AF_INET, SOCK_STREAM, 0);

        if (strcmp(command, "register") == 0)
        {
            std::cout << "\n--------------REGISTER---------------\n";
            std::cout << "Username = ";
            std::cin.getline(username, STRLEN);

            std::cout << "Password = ";
            std::cin.getline(password, STRLEN);
            std::cout << "\n";

            if (strstr(username, " ") != NULL || strstr(password, " ") != NULL)
            {
                std::cout << "Username or password contains spaces!\n";
                continue;
            }

            registerToServer(sockfd, username, password);
        }
        else if (strcmp(command, "login") == 0)
        {
            std::cout << "\n--------------LOGIN---------------\n";
            std::cout << "Username = ";
            std::cin.getline(username, STRLEN);

            std::cout << "Password = ";
            std::cin.getline(password, STRLEN);
            std::cout << "\n";

            cookie = loginToServer(sockfd, username, password);
        }
        else if (strcmp(command, "enter_library") == 0)
        {
            if (cookie == NULL)
            {
                std::cout << "Please login first\n";
                continue;
            }

            std::cout << "\n--------------ENTER LIBRARY---------------\n";
            token = enterLibrary(sockfd, cookie);
        }
        else if (strcmp(command, "get_books") == 0)
        {
            if (token == NULL)
            {
                std::cout << "Please enter the library first!\n";
                continue;
            }

            std::cout << "\n--------------GET BOOKS---------------\n";
            getBooks(sockfd, token);
        }
        else if (strcmp(command, "get_book") == 0)
        {
            if (token == NULL)
            {
                std::cout << "Please enter the library first!\n";
                continue;
            }

            std::cout << "\n--------------GET BOOK---------------\n";

            char id[STRLEN];
            std::cout << "id = ";
            std::cin.getline(id, STRLEN);

            if (atoi(id) == 0)
            {
                std::cout << "ID is not an integer!\n";
                continue;
            }

            getBook(sockfd, token, id);
        }
        else if (strcmp(command, "add_book") == 0)
        {
            if (token == NULL)
            {
                std::cout << "Please enter the library first!\n";
                continue;
            }

            std::cout << "\n--------------ADD BOOK---------------\n";

            char title[STRLEN], author[STRLEN], genre[STRLEN], publisher[STRLEN], page_count[STRLEN];

            std::cout << "title = ";
            std::cin.getline(title, STRLEN);
            std::cout << "author = ";
            std::cin.getline(author, STRLEN);
            std::cout << "genre = ";
            std::cin.getline(genre, STRLEN);
            std::cout << "publisher = ";
            std::cin.getline(publisher, STRLEN);
            std::cout << "page_count = ";
            std::cin.getline(page_count, STRLEN);

            if (atoi(page_count) == 0)
            {
                std::cout << "Page_count is not an integer!\n";
                continue;
            }

            addBook(sockfd, token, title, author, genre, publisher, page_count);
        }
        else if (strcmp(command, "delete_book") == 0)
        {

            if (token == NULL)
            {
                std::cout << "ERROR - Please enter the library first!\n";
                continue;
            }

            std::cout << "\n--------------DELETE BOOK---------------\n";

            char id[STRLEN];
            std::cout << "id = ";
            std::cin.getline(id, STRLEN);

            if (atoi(id) == 0)
            {
                std::cout << "ERROR - ID is not an integer!\n";
                continue;
            }

            deleteBook(sockfd, token, id);
        }
        else if (strcmp(command, "logout") == 0)
        {
            if (cookie == NULL)
            {
                std::cout << "ERROR - Not authenticated!\n";
                continue;
            }

            logout(sockfd, cookie);
            token = NULL;
            cookie = NULL;
        }
        else if (strcmp(command, "exit") == 0)
        {
            close_connection(sockfd);
            break;
        }
        close_connection(sockfd);
    }

    // Ex 1.2: POST dummy and print response from main server
    // char **body = (char **)calloc(3, sizeof(char *));
    // body[0] = (char *)calloc(1, strlen("username=student"));
    // body[1] = (char *)calloc(1, strlen("password=student"));
    // strcat(body[0], "username=student");
    // strcat(body[1], "password=student");
    // message = compute_post_request("34.241.4.235", "/api/v1/auth/login", "application/x-www-form-urlencoded",
    //                                body, 2, NULL, 0);
    // puts(message);
    // send_to_server(sockfd, message);
    // response = receive_from_server(sockfd);
    // puts(response);
    // Ex 2: Login into main server
    // Ex 3: GET weather key from main server
    // Ex 4: GET weather data from OpenWeather API
    // char *sid = (char *)malloc(1000);
    // strcpy(sid, "connect.sid=s%3Ap1I4RKMMkkfar7DnctiTYXFLmngpT1Q8.A7GlbN%2BlqMv%2FMffVALR1eZjhfncYvFTJW3%2BvpOngliA\0");

    // body[0] = (char *)realloc(body[0], strlen(sid));
    // body[0] = sid;

    // message = compute_get_request("34.241.4.235", "/api/v1/weather/key", NULL,
    //                               body, 1);

    // puts(message);
    // send_to_server(sockfd, message);
    // response = receive_from_server(sockfd);
    // puts(response);

    // Ex 5: POST weather data for verification to main server
    // close_connection(sockfd);

    // sockfd = open_connection("34.241.4.235", 80, AF_INET, SOCK_STREAM, 0);

    // char *query = (char *)malloc(1000);

    // strcpy(query, "lat=44.7398&lon=22.2767&appid=b912dd495585fbf756dc6d8f415a7649");

    // message = compute_get_request("34.241.4.235", "/data/2.5/weather", query,
    //                               NULL, 0);

    // puts(message);
    // send_to_server(sockfd, message);
    // response = receive_from_server(sockfd);
    // puts(response);
    // Ex 6: Logout from main server

    // BONUS: make the main server return "Already logged in!"

    // free the allocated data at the end!

    return 0;
}
