#include <stdio.h>      /* printf, sprintf */
#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>
#include <ctype.h>
#include "helpers.h"
#include "requests.h"
#include "parson.h"

int main()
{   
    //initializez variabilele folosite
    char *message = NULL;
    char *response = NULL;
    char *cookie = NULL;
    char *token = NULL;
    int sockfd = -1;
    char buff[BUFLEN];
    
    //busy waiting pana cand primesc comanda de exit
    while(1){
        
        //primesc comanda
        printf("Introduceti comanda: ");
        fgets(buff, BUFLEN, stdin);
        buff[strlen(buff) - 1] = '\0';

        //daca comanda este exit ies din loop
        if(strncmp(buff, "exit", strlen("exit")) == 0){

            sockfd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);
            printf("Exiting...\n");
            close_connection(sockfd);
            break;

        }
        
        //daca comanda este register
        else if(strncmp(buff, "register", strlen("register")) == 0){
            
            //deschide conexiunea
            sockfd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);
            char username[BUFLEN], password[BUFLEN];
            
            //primesc username
            printf("Username: ");
            fgets(username, BUFLEN, stdin);
            username[strlen(username) - 1] = '\0';
            
            //primesc password
            printf("Password: ");
            fgets(password, BUFLEN, stdin);
            password[strlen(password) - 1] = '\0';

            //creez un obiect json cu username si password pe care il trimit serverului
            JSON_Value *login_value = json_value_init_object();
            JSON_Object *login_object = json_value_get_object(login_value);
            char *ss = NULL;
            json_object_set_string(login_object, "username", username); 
            json_object_set_string(login_object, "password", password);
            ss = json_serialize_to_string_pretty(login_value);
            
            //fac un post request catre server si trimit mesajul
            message = compute_post_request(HOST, "/api/v1/tema/auth/register", "application/json", ss, NULL, NULL);
            send_to_server(sockfd, message);

            //primesc raspunsul de la server
            response = receive_from_server(sockfd);

            //daca primesc eroare inseamna ca username-ul este deja luat
            if(strstr(response,"error")){
                error("Username already taken.\n");
            } 
            
            //inregistrare cu succes
            else{
                printf("You've been registered successfully.\n");
            }

            //eliberez variabilele alocate dinamic si inchid conexiunea
            json_free_serialized_string(ss);
            json_value_free(login_value);
            close_connection(sockfd);

        }
        
        //daca comanda este login
        else if(strncmp(buff, "login", strlen("login")) == 0){
            
            //deschid conexiunea catre server
            sockfd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);
            char username[BUFLEN], password[BUFLEN];
            
            //primesc username
            printf("Username: ");
            fgets(username, BUFLEN, stdin);
            username[strlen(username) - 1] = '\0';

            //priemsc password
            printf("Password: ");
            fgets(password, BUFLEN, stdin);
            password[strlen(password) - 1] = '\0';
            
            //creez un obiect json cu username si password pe care il trimit serverului
            JSON_Value *login_value = json_value_init_object();
            JSON_Object *login_object = json_value_get_object(login_value);
            char *ss = NULL;
            json_object_set_string(login_object, "username", username); 
            json_object_set_string(login_object, "password", password);
            ss = json_serialize_to_string_pretty(login_value);

            //fac un post request catre server si trimit mesajul
            message = compute_post_request(HOST, "/api/v1/tema/auth/login", "application/json", ss, NULL, NULL);
            send_to_server(sockfd, message);
            
            //primesc raspunsul de la server
            response = receive_from_server(sockfd);
            
            //daca primesc eroare inseamna ca nu exista cont asociat cu username-ul respectiv sau parola este gresita
            if(strstr(response,"error"))
                error("Incorrect username and/or password.\n");
            
            //daca se poate efectua login-ul primesc cookie-ul specific user-ului
            else{
                cookie = extract_cookie(response);
                printf("Welcome, %s.\n", username);
            }
            
            //eliberez variabilele alocate dinamic si inchid conexiunea
            json_free_serialized_string(ss);
            json_value_free(login_value); 
            close_connection(sockfd);

        }
        
        else if(strncmp(buff, "enter_library", strlen("enter_library")) == 0){

            //daca am primit cookie, adica daca m-am logat, permit accesul in librarie
            if(cookie){

                //deschid conexiunea catre serve
                sockfd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);

                //creez si trimit requestul de a primi token pt acces
                message = compute_get_request(HOST, "/api/v1/tema/library/access", NULL, cookie);
                send_to_server(sockfd, message);
                
                //primesc raspuns de la server
                response = receive_from_server(sockfd);
                
                //daca primesc eroare nu extrag tokenul pt ca nu-l primesc
                if(strstr(response, "error")){
                    error("Could not get access to library.\n");
                }
                
                //extrag tokenul din raspunsul serverului
                else{
                    token = extract_token(response);
                    if(token != NULL)
                    printf("You have entered the library.\n");
                }
                
                //inchid conexiunea 
                close_connection(sockfd);
            }

            else
                //nu sunt logat deci nu pot primi acces
                error("Not logged in.\n");

        }
        
        //daca comanda este get_books
        else if(strncmp(buff, "get_books", strlen("get_books")) == 0){

            //trb sa am acces la librarie si implicit sa fiu logat
            if(token){

                //deschid conexiunea catre serve
                sockfd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);

                //creez si trimit requestul de a primi lista de carti
                message = compute_get_request(HOST, "/api/v1/tema/library/books", token, cookie);
                if (message == NULL)
                    error("Failed to compute the GET request.\n");
                send_to_server(sockfd, message);

                //primesc raspunsul de la server
                response = receive_from_server(sockfd);
                if (response == NULL)
                    error("Failed to receive response from the server.\n");
                
                //daca primesc eroare inseamna ca nu am acces
                if(strstr(response, "error"))
                    error("Access error.\n");
                
                //printez cartile
                else
                    printf("%s\n", strstr(response, "[{"));
                
                //inchid conexiunea
                close_connection(sockfd);
            }

            //nu am acces la librarie
            else
                error("You don't have access to the library.\n");

        }
        
        //daca primesc comanda get_book
        else if(strncmp(buff, "get_book", strlen("get_book")) == 0){
            
            //trb sa am acces la librarie
            if(token){
                //deschid conexiunea catre server
                sockfd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);
                
                //buffer pt url-ul cartii
                char url[BUFLEN];
                printf("Book ID: ");
                int id, ret;
                
                //scanez id-ul cartii
                ret = scanf("%d", &id);
                
                //extrag "\n"-ul
                getchar();

                //eroare scanf sau id negativs
                if(ret != 1 || id < 0){
                    error("Not a valid book ID.\n");
                }

                //creez requestul la url-ul cartii si afisez cartea daca nu primesc eroare
                else{
                sprintf(url, "/api/v1/tema/library/books/%d", id);
                message = compute_get_request(HOST, url, token, cookie);
                send_to_server(sockfd, message);

                response = receive_from_server(sockfd);
                
                //daca primesc eroare inseamna ca nu exista carte cu id-ul respectiv
                if(strstr(response, "error"))
                    error("No book found with this ID.\n");
                
                else
                    printf("%s\n", strstr(response, "{"));
                }
                
                //inchid conexiunea
                close_connection(sockfd);
            }
            
            //nu am acces la librarie
            else
                error("You don't have access to the library.\n");

        }
        
        //daca primesc comanda add_book
        else if(strncmp(buff, "add_book", strlen("add_book")) == 0){

            //am creat un busy waiting pt a opri primirea detaliilor despre carte in cazul in care una din ele este gresita
            while(1){

            //am acces la librarie
            if(token){

                //deschid conexiunea
                sockfd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);
                char title[BUFLEN], author[BUFLEN], genre[BUFLEN], publisher[BUFLEN], page_count_str[BUFLEN];
                int page_count, ret, length, err = 0;
                
                //primesc titlu
                printf("title: ");
                fgets(title, BUFLEN, stdin);
                title[strlen(title) - 1] = '\0';
                length = strlen(title);
                for (int i = 0; i < length; i++) {
                    if (!isalpha(title[i])) {
                        error("Wrong title format.\n");
                        err = 1;
                        break;
                    }
                }

                if(err)
                break;
                
                //primesc autor
                printf("author: ");
                fgets(author, BUFLEN, stdin);
                author[strlen(author) - 1] = '\0';
                length = strlen(author);
                for (int i = 0; i < length; i++) {
                    if (!isalpha(author[i])) {
                        error("Wrong author format.\n");
                        err = 1;
                        break;
                    }
                }
                
                if(err)
                break;

                //primesc gen
                printf("genre: ");
                fgets(genre, BUFLEN, stdin);
                genre[strlen(genre) - 1] = '\0';
                length = strlen(genre);
                for (int i = 0; i < length; i++) {
                    if (!isalpha(genre[i])) {
                        error("Wrong genre format.\n");
                        err = 1;
                        break;
                    }
                }

                if(err)
                break;

                //primesc numarul paginilor
                printf("page_count: ");
                fgets(page_count_str, BUFLEN, stdin);
                page_count_str[strlen(page_count_str) - 1] = '\0';
                ret = sscanf(page_count_str, "%d", &page_count);
                if(ret != 1 || page_count < 0){
                    error("Not a valid page count.\n");
                    err = 1;
                }
                
                if(err)
                break;

                //primesc editor
                printf("publisher: ");
                fgets(publisher, BUFLEN, stdin);
                publisher[strlen(publisher) - 1] = '\0';
                length = strlen(publisher);
                for (int i = 0; i < length; i++) {
                    if (!isalpha(publisher[i])) {
                        error("Wrong publisher format.\n");
                        err = 1;
                        break;
                    }
                }

                //daca nu am avut nicio eroare creez mesajul json si il trimit serverului
                if(!err){
                JSON_Value *book_value = json_value_init_object();
                JSON_Object *book_object = json_value_get_object(book_value);
                char *ss = NULL;
                json_object_set_string(book_object, "title", title); 
                json_object_set_string(book_object, "author", author);
                json_object_set_string(book_object, "genre", genre);
                json_object_set_number(book_object, "page_count", page_count);
                json_object_set_string(book_object, "publisher", publisher);
                ss = json_serialize_to_string_pretty(book_value);
                message = compute_post_request(HOST, "/api/v1/tema/library/books", "application/json", ss, token, cookie);
                json_free_serialized_string(ss);
                json_value_free(book_value); 
                send_to_server(sockfd, message);

                //primesc raspuns de la server
                response = receive_from_server(sockfd);
                
                //cartea nu a fost trimisa in forma corecta
                if(strstr(response,"error"))
                    printf("Wrong book format.\n");
                
                //cartea a fost adaugata cu succes
                else
                    printf("Book added successfully.\n");
                }
                
                //cartea nu a putut fi adaugata
                else{
                    error("Could not add book.\n");
                    err = 0;
                }
                
                //inchid conexiunea si ies din loop
                close_connection(sockfd);
                break;
                }
            
            //nu am acces la librarie si ies din loop
            else{
                error("You don't have access to the library.\n");
                break;
            }
            }
        }
        
        //daca primresc comanda de delete_book
        else if(strncmp(buff, "delete_book", strlen("delete_book")) == 0){
            
            //am acces la librarie
            if(token){
                
                //deschid conexiunea
                sockfd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);
                char url[BUFLEN];

                //primesc id-ul cartii
                printf("Book ID: ");
                int id, ret;
                ret = scanf("%d", &id);
                getchar();
                if(ret != 1 || id < 0){
                    error("Not a valid book ID.\n");
                }
                
                //daca id-ul este bun trimit cerere pt delete serverului
                else{
                sprintf(url, "/api/v1/tema/library/books/%d", id);
                message = compute_delete_request(HOST, url, token, cookie);
                send_to_server(sockfd, message);
                
                //primesc raspuns de la server
                response = receive_from_server(sockfd);
                //nu a fost gasita cartea ceruta
                if(strstr(response, "error"))
                    error("No book found with this ID.\n");
                
                //cartea a fost stearsa
                else
                    printf("Book deleted successfully.\n");
                }

                //inchid conexiunea
                close_connection(sockfd);
            }

            //nu am acces la librarie
            else
                error("You don't have access to the library.\n");


        }
        
        //daca primesc comanda de logout
        else if(strncmp(buff, "logout", strlen("logout")) == 0){
            
            //daca sunt logat deja
            if(cookie){

                //deschid conexiunea, creez cererea de logout si trimit serverului
                sockfd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);
                message = compute_get_request(HOST, "/api/v1/tema/auth/logout", NULL, cookie);
                send_to_server(sockfd, message);

                //primesc raspunsul de la server
                response = receive_from_server(sockfd);

                //eroare
                if(strstr(response, "error"))
                    error("Logout error.\n");

                //logout cu succes
                else
                    printf("Goodbye!\n");

                //resetez cookie si token
                cookie = NULL;
                token = NULL;

                //inchid conexiunea
                close_connection(sockfd);    
            }

            //nu sunt logat
            else
                error("Not logged in.\n");
        }
    }

    return 0;

}