#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <stdio.h>
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>
#include "helpers.h"
#include "requests.h"

char *compute_get_request(char *host, char *url, char *token, char *cookie){
    char* message = calloc(BUFLEN, sizeof(char));
    char* line = calloc(LINELEN, sizeof(char));

    // Scriu numele metodei, calea, parametri din url (daca exista) si tipul protocolului
    sprintf(line, "GET %s HTTP/1.1", url);

    compute_message(message, line);

    // Adaug host-ul
    sprintf(line, "HOST: %s", host);
    compute_message(message, line);

    if (cookie != NULL) {
        sprintf(line, "Cookie: %s", cookie);
        compute_message(message, line);
    }

    // Adaug token-ul sau cookies, respectand forma protocolului
    if (token != NULL) {
        sprintf(line, "Authorization: Bearer %s", token);
        compute_message(message, line);
    }

    // Adaug linia de final
    compute_message(message, "");

    free(line);
    return message;
}

char *compute_post_request(char *host, char *url, char *content_type, char *content, char *token, char *cookie ){

    char* message = calloc(BUFLEN, sizeof(char));
    char* line = calloc(LINELEN, sizeof(char));

    // Step 1: write the method name, URL and protocol type
    sprintf(line, "POST %s HTTP/1.1", url);
    compute_message(message, line);

    // Step 2: add the host
    if (host != NULL) {
        sprintf(line, "Host: %s", host);
    }
    compute_message(message, line);

    /* Step 3: add necessary headers (Content-Type and Content-Length are mandatory)
            in order to write Content-Length you must first compute the message size
    */
    sprintf(line, "Content-Type: %s", content_type);
    compute_message(message, line);

    sprintf(line, "Content-Length: %zu", strlen(content));
    compute_message(message, line);

    // Step 4 (optional): add cookies
    if (cookie != NULL) {
        memset(line, 0, LINELEN);
        sprintf(line, "Cookie: %s", cookie);
        compute_message(message, line);
    }

    if (token != NULL) {
        sprintf(line, "Authorization: Bearer %s", token);
        compute_message(message, line);
    }

    // Step 5: add new line at end of header
    compute_message(message, "");

    // Step 6: add the actual payload content
    compute_message(message, content);

    free(line);
    return message;
}

char *compute_delete_request(char *host, char *url, char *token, char *cookie){
    char* message = calloc(BUFLEN, sizeof(char));
    char* line = calloc(LINELEN, sizeof(char));

    // Scriu numele metodei, calea, parametri din url (daca exista) si tipul protocolului
    sprintf(line, "DELETE %s HTTP/1.1", url);

    compute_message(message, line);

    // Adaug host-ul
    sprintf(line, "HOST: %s", host);
    compute_message(message, line);

    if (cookie != NULL) {
        sprintf(line, "Cookie: %s", cookie);
        compute_message(message, line);
    }

    // Adaug token-ul sau cookies, respectand forma protocolului
    if (token != NULL) {
        sprintf(line, "Authorization: Bearer %s", token);
        compute_message(message, line);
    }

    // Adaug linia de final
    compute_message(message, "");

    free(line);
    return message;
}