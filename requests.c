#include <stdlib.h>   
#include <stdio.h>
#include <unistd.h>     
#include <string.h>    
#include <sys/socket.h> 
#include <netinet/in.h>
#include <netdb.h>      
#include <arpa/inet.h>
#include "helpers.h"
#include "requests.h"
#include "parson.h"

char *compute_get_request(char *host, char *url, char *query_params, char *cookies, char *jwt)
{
    char *message = calloc(BUFLEN, sizeof(char));
    char *line = calloc(LINELEN, sizeof(char));
    // Step 1: write the method name, URL, request params (if any) and protocol type
    if (query_params != NULL) {
        sprintf(line, "GET %s?%s HTTP/1.1", url, query_params);
        compute_message(message, line);
    } else {
        sprintf(line, "GET %s HTTP/1.1", url);
        compute_message(message, line);
    }
    // Step 2: add the host
    sprintf(line, "Host: %s", host);
    compute_message(message, line);
    // Step 3 (optional): add headers and/or cookies, according to the protocol format
    if (jwt != NULL) {
        sprintf(line, "Authorization: Bearer %s", jwt);
        compute_message(message, line);
    }
    if (cookies != NULL) {
       sprintf(line, "Cookie: %s", cookies);
       compute_message(message, line);
    }
    // Step 4: add final new line
    compute_message(message, "");
    free(line);
    return message;
}

char *compute_post_request(char *host, char *url, char *content_type, char *body_data, char *cookies, char *jwt)
{
    char *message = calloc(BUFLEN, sizeof(char));
    char *line = calloc(LINELEN, sizeof(char));
    char *body_data_buffer = calloc(LINELEN, sizeof(char));

    // Step 1: write the method name, URL and protocol type
    sprintf(line, "POST %s HTTP/1.1", url);
    compute_message(message, line);
    
    // Step 2: add the host
    sprintf(line, "Host: %s", host);
    compute_message(message, line);

    /* Step 3: add necessary headers (Content-Type and Content-Length are mandatory)
            in order to write Content-Length you must first compute the message size
    */
    sprintf(line, "Content-Type: %s", content_type);
    compute_message(message, line);

    sprintf(line, "Content-Length: %ld", strlen(body_data));
    compute_message(message, line);
    
    // Step 4 (optional): add cookies/jwt
    if (jwt != NULL) {
        sprintf(line, "Authorization: Bearer %s", jwt);
        compute_message(message, line);
    }
    if (cookies != NULL) {
       sprintf(line, "Cookie: %s", cookies);
       compute_message(message, line);
    }
    // Step 5: add new line at end of header
    compute_message(message, "");
    // Step 6: add the actual payload data
    memset(line, 0, LINELEN);
    strcat(body_data_buffer, body_data);
    compute_message(message, body_data_buffer);

    free(line);
    return message;
}
char *compute_delete_request(char *host, char *url, char *cookies, char *jwt) {
    char *message = calloc(BUFLEN, sizeof(char));
    char *line = calloc(LINELEN, sizeof(char));

    // Step 1: write the method name, URL and protocol type
    sprintf(line, "DELETE %s HTTP/1.1", url);
    compute_message(message, line);
    
    // Step 2: add the host
    sprintf(line, "Host: %s", host);
    compute_message(message, line);

    if (jwt != NULL) {
        sprintf(line, "Authorization: Bearer %s", jwt);
        compute_message(message, line);
    }
    if (cookies != NULL) {
        sprintf(line, "Cookie: %s", cookies);
        compute_message(message, line);
    }
    compute_message(message, "");
    free(line);
    return message;
}

void register_(char *ip, char **cookies) {
    int sockfd = open_connection(ip, 8080, AF_INET, SOCK_STREAM, 0);

    char username[50];
    char password[50];
    char *message;
    char *response;

    if (*cookies) {
        printf("To create a new account, please log out\n");
        return;
    }
    printf("username=");
    memset(username, 0, 50);
    fgets(username, 50, stdin);
    strcpy(username + strlen(username) - 1, username + strlen(username));
    
    printf("password=");
    memset(password, 0, 50);
    fgets(password, 50, stdin);
    strcpy(password + strlen(password) - 1, password + strlen(password));

    JSON_Value *root_value = json_value_init_object();
    JSON_Object *root_object = json_value_get_object(root_value);
    char *serialized_string = NULL;
    json_object_set_string(root_object, "username", username);
    json_object_set_string(root_object, "password", password);
    serialized_string = json_serialize_to_string_pretty(root_value);

    message = compute_post_request(ip, "/api/v1/tema/auth/register", "application/json", serialized_string, *cookies, NULL);
    // printf("%s\n", message);
    send_to_server(sockfd, message);
    response = receive_from_server(sockfd);
    // printf("%s\n", response);
    if (strstr(response, "The username")) {
        printf("Username %s is taken!\n", username);
    } else if (!strstr(response, "error")) {
        printf("Successfully registered, welcome %s !\n", username);
    }

    free(message);
    free(response);
    json_free_serialized_string(serialized_string);
    json_value_free(root_value);
    close_connection(sockfd);
}

void login_(char *ip, char **cookies) {
    int sockfd = open_connection(ip, 8080, AF_INET, SOCK_STREAM, 0);

    char username[50];
    char password[50];
    char *message;
    char *response;
    if (*cookies) {
        printf("Already logged in\n");
        return;
    }
    printf("username=");
    memset(username, 0, 50);
    fgets(username, 50, stdin);
    strcpy(username + strlen(username) - 1, username + strlen(username));

    printf("password=");
    memset(password, 0, 50);
    fgets(password, 50, stdin);
    strcpy(password + strlen(password) - 1, password + strlen(password));

    JSON_Value *root_value = json_value_init_object();
    JSON_Object *root_object = json_value_get_object(root_value);
    char *serialized_string = NULL;
    json_object_set_string(root_object, "username", username);
    json_object_set_string(root_object, "password", password);
    serialized_string = json_serialize_to_string_pretty(root_value);

    message = compute_post_request(ip, "/api/v1/tema/auth/login", "application/json", serialized_string, *cookies, NULL);
    send_to_server(sockfd, message);
    response = receive_from_server(sockfd);
    // printf("%s\n", response);
    if (strstr(response, "200 OK")) {
        char *start = strstr(response, "Set-Cookie: ");
        char *tok = strtok(start + 12, "\n");
        strcat(tok, "\n"); // daca nu pun '\n', crapa
        *cookies = malloc(strlen(tok) * sizeof(char));
        memcpy(*cookies, tok, strlen(tok));
        // printf("%s", *cookies);
    } else if (strstr(response, "Credentials are not good!")) {
        printf("Credentials are not good!\n");
    } else if (strstr(response, "No account with this username!")) {
        printf("No account with this username!\n");
    }

    free(message);
    free(response);
    json_free_serialized_string(serialized_string);
    json_value_free(root_value);
    close_connection(sockfd);

}
void enter_library(char *ip, char **cookies, char **jwt) {
    int sockfd = open_connection(ip, 8080, AF_INET, SOCK_STREAM, 0);
    
    char *message;
    char *response;
    if (*cookies == NULL) {
        printf("You are not logged in!\nPlease log in\n");
        return;
    }
    message = compute_get_request(ip, "/api/v1/tema/library/access", NULL, *cookies, NULL);
    // printf("%s\n", message);
    send_to_server(sockfd, message);
    response = receive_from_server(sockfd);
    // printf("%s\n", response);
    char *token = strstr(response, "token");
    if (token) {
        char *tok = strtok(token + 8, "}");
        strcpy(tok + strlen(tok) - 1, tok + strlen(tok));
        *jwt = malloc(strlen(tok) * sizeof(char));
        // strncpy(*jwt, tok, strlen(tok));
        memcpy(*jwt, tok, strlen(tok));
        printf("Access granted\n");
    }
    free(message);
    free(response);
    close_connection(sockfd);

}
void get_books(char *ip, char **cookies, char **jwt) {
    int sockfd = open_connection(ip, 8080, AF_INET, SOCK_STREAM, 0);
    
    char *message;
    char *response;
    if (*jwt == NULL) {
        printf("You don't have access\n");
        return;
    }
    message = compute_get_request(ip, "/api/v1/tema/library/books", NULL, *cookies, *jwt);
    send_to_server(sockfd, message);
    response = receive_from_server(sockfd);
    // printf("%s\n", response);
    if (strstr(response, "200 OK")) {
        char *aux = strstr(response, "[");
        char *tok = strtok(aux, "\n");
        printf("%s\n", tok);
    } else {
        printf("Cannot access books\n");
    }
    free(message);
    free(response);
    close_connection(sockfd);

}
void logout(char *ip, char **cookies) {
    int sockfd = open_connection(ip, 8080, AF_INET, SOCK_STREAM, 0);
    
    if (*cookies == NULL) {
        printf("You are not logged in.\nIf you want to log out, please first log in\n");
        close_connection(sockfd);
        return;
    }
    char *message;
    char *response;
    message = compute_get_request(ip, "/api/v1/tema/auth/logout", NULL, *cookies, NULL);
    send_to_server(sockfd, message);
    response = receive_from_server(sockfd);
    if (strstr(response, "200 OK")) {
        printf("Successfully logged out\n");
    }
    free(*cookies);
    *cookies = NULL;
    free(message);
    free(response);
    close_connection(sockfd);

}
void get_book(char *ip, char **cookies, char **jwt) {
    int sockfd = open_connection(ip, 8080, AF_INET, SOCK_STREAM, 0);
    
    char *message;
    char *response;
    int id;
    if (*jwt == NULL) {
        printf("You don't have access\n");
        return;
    }
    printf("id=");
    scanf("%d", &id);
    char *aux = "/api/v1/tema/library/books/:";
    char *url = calloc((strlen(aux) + 10), sizeof(char));
    sprintf(url, "/api/v1/tema/library/books/%d", id);
    message = compute_get_request(ip, url, NULL, *cookies, *jwt);
    send_to_server(sockfd, message);
    response = receive_from_server(sockfd);
    if (strstr(response, "No book was found!")) {
        printf("No book was found or ID is invalid.\n");
    } else if (strstr(response, "200 OK")) {
        char *aux = strstr(response, "[");
        char *tok = strtok(aux, "\n");
        printf("%s\n", tok );
    }
    free(message);
    free(response);
    free(url);
    close_connection(sockfd);

}
void add_book(char *ip, char **cookies, char **jwt) {
    int sockfd = open_connection(ip, 8080, AF_INET, SOCK_STREAM, 0);
    
    char title[30];
    char author[30];
    char genre[30];
    char publisher[30];
    char page_count[30];
    int pg;
    if (*jwt == NULL) {
        printf("You don't have access\n");
        return;
    }
    printf("title=");
    memset(title, 0, 30);
    fgets(title, 30, stdin);
    strcpy(title + strlen(title) - 1, title + strlen(title));

    memset(author, 0, 30);
    printf("author=");
    fgets(author, 30, stdin);
    strcpy(author + strlen(author) - 1, author + strlen(author));

    memset(genre, 0, 30);
    printf("genre=");
    fgets(genre, 30, stdin);
    strcpy(genre + strlen(genre) - 1, genre + strlen(genre));

    memset(publisher, 0, 30);
    printf("publisher=");
    fgets(publisher, 30, stdin);
    strcpy(publisher + strlen(publisher) - 1, publisher + strlen(publisher));

    memset(page_count, 0, 30);
    printf("page_count=");
    scanf("%d", &pg);
    if (pg < 0) {
        printf("Invalid number of pages\n");
        close_connection(sockfd);
        return;
    }
    char *message;
    char *response;
    sprintf(page_count, "%d", pg);

    JSON_Value *root_value = json_value_init_object();
    JSON_Object *root_object = json_value_get_object(root_value);
    char *serialized_string = NULL;
    json_object_set_string(root_object, "title", title);
    json_object_set_string(root_object, "author", author);
    json_object_set_string(root_object, "genre", genre);
    json_object_set_string(root_object, "page_count", page_count);
    json_object_set_string(root_object, "publisher", publisher);

    serialized_string = json_serialize_to_string_pretty(root_value);
    // printf("%s\n", serialized_string);

    message = compute_post_request(ip, "/api/v1/tema/library/books", "application/json", serialized_string, NULL, *jwt);
    // printf("%s\n", message);
    send_to_server(sockfd, message);
    response = receive_from_server(sockfd);
    // printf("%s\n", response);
    if (strstr(response, "200 OK")) {
        printf("Book has been successfully added\n");
    }

    free(message);
    free(response);
    json_free_serialized_string(serialized_string);
    json_value_free(root_value);
    close_connection(sockfd);
}
void delete_book(char *ip, char **cookies, char **jwt) {
    int sockfd = open_connection(ip, 8080, AF_INET, SOCK_STREAM, 0);
    char *message;
    char *response;
    int id;
    if (*jwt == NULL) {
        printf("You don't have access\n");
        return;
    }
    printf("id=");
    scanf("%d", &id);
    char *aux = "/api/v1/tema/library/books/:";
    char *url = calloc((strlen(aux) + 10), sizeof(char));
    sprintf(url, "/api/v1/tema/library/books/%d", id);
    message = compute_delete_request(ip, url, *cookies, *jwt);
    send_to_server(sockfd, message);
    response = receive_from_server(sockfd);
    // printf("%s\n", response);
    if (strstr(response, "No book was deleted")) {
        printf("No book was deleted. Book with id : %d doesn't exist.\n", id);
        return;
    } else if (strstr(response, "200 OK")) {
        printf("Book with id : %d has been successfully deleted\n", id);
    }

    free(message);
    free(response);
    close_connection(sockfd);

}