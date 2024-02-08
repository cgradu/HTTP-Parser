#ifndef _REQUESTS_
#define _REQUESTS_

// computes and returns a GET request string (token
// and cookies can be set to NULL if not needed)
char *compute_get_request(char *host, char *url, char *token, char *cookie);

// computes and returns a POST request string (token and cookies can be NULL if not needed)
char *compute_post_request(char *host, char *url, char *content_type, char *content, char *token, char *cookie);

// computes and returns a DELETE request string (token and cookies can be NULL if not needed)
char *compute_delete_request(char *host, char *url, char *token, char *cookie);

#endif
