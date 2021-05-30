#include <stdio.h>      
#include <stdlib.h>     
#include <unistd.h>     
#include <string.h>    
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <netdb.h>      
#include <arpa/inet.h>
#include "helpers.h"
#include "requests.h"

int main(int argc, char *argv[]) {
	char *ip = "34.118.48.238";
	char buffer[BUFLEN];
	char *cookies = NULL;
	char *jwt = NULL;
	memset(buffer, 0, BUFLEN);
	while(1) {
		fgets(buffer, BUFLEN, stdin);
		if (!strncmp(buffer, "register", 8)) {
			register_(ip, &cookies);
		} else if (!strncmp(buffer, "login", 5)) {
			login_(ip, &cookies);
		} else if (!strncmp(buffer, "enter_library", 13)) {
			enter_library(ip, &cookies, &jwt);
		} else if (!strncmp(buffer, "get_books", 9)) {
			get_books(ip, &cookies, &jwt);
		} else if (!strncmp(buffer, "get_book", 8)) {
			get_book(ip, &cookies, &jwt);
		} else if (!strncmp(buffer, "add_book", 8)) {
			add_book(ip, &cookies, &jwt);
		} else if (!strncmp(buffer, "delete_book", 11)) {
			delete_book(ip, &cookies, &jwt);
		} else if (!strncmp(buffer, "logout", 6)) {
			logout(ip, &cookies);
		} else if (!strncmp(buffer, "exit", 4)) {
			break;
		}
		memset(buffer, 0, BUFLEN);
	}
	return 0;
}