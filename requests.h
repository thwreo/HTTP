#ifndef _REQUESTS_
#define _REQUESTS_

// computes and returns a GET request string (query_params
// and cookies can be set to NULL if not needed)
char *compute_get_request(char *host, char *url, char *query_params, char *cookies, char *jwt);

// computes and returns a POST request string (cookies can be NULL if not needed)
char *compute_post_request(char *host, char *url, char* content_type, char *body_data, char* cookies, char *jwt);
char *compute_delete_request(char *host, char *url, char *cookies, char *jwt);
void register_(char *ip, char **cookies);
void login_(char *ip, char **cookies);
void enter_library(char *ip, char **cookies, char **jwt);
void get_books(char *ip, char **cookies, char **jwt);
void logout(char *ip, char **cookies);
void get_book(char *ip, char **cookies, char **jwt);
void add_book(char *ip, char **cookies, char **jwt);
void delete_book(char *ip, char **cookies, char **jwt);


#endif
