#ifndef HTTP_H
#define HTTP_H

void handle_http_request(int client_socket, const char *web_root);
const char *get_mime_type(const char *path);
void send_http_error(int client_socket, int status_code, const char *status_text);

#endif
