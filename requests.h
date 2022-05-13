#ifndef _REQUESTS_
#define _REQUESTS_

char *parser(char *response, char *retPointer, char const *delimiter);

// computes and returns a GET request string (query_params
// and cookies can be set to NULL if not needed)
char *compute_get_request(char const *url, char *cookies, char *token);

char *compute_delete_request(char const *url, char *token);

// computes and returns a POST request string (cookies can be NULL if not needed)
char *compute_post_request(char const *url, char *body_data, char *SWT);

char *loginToServer(int sockfd, char *username, char *password);

char *enterLibrary(int sockfd, char *cookie);

void getBooks(int sockfd, char *token);

void getBook(int sockfd, char *token, char *id);

void addBook(int sockfd, char *token, char *title, char *author, char *genre, char *publisher, char *page_count);

void deleteBook(int sockfd, char *token, char *id);

void logout(int sockfd, char *cookie);

#endif
