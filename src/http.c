#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include "http.h"

#define BUFFER_SIZE 4096

// Fungsi Bantuan: Mengirim template error HTTP standar
void send_http_error(int client_socket, int status_code, const char *status_text) {
    char response[1024];
    char body[512];
    
    snprintf(body, sizeof(body), 
             "<html><head><title>%d %s</title></head>"
             "<body><center><h1>%d %s</h1></center><hr><center>C Mini WebServer</center></body></html>", 
             status_code, status_text, status_code, status_text);

    snprintf(response, sizeof(response),
             "HTTP/1.1 %d %s\r\n"
             "Content-Type: text/html\r\n"
             "Content-Length: %lu\r\n"
             "Connection: close\r\n\r\n"
             "%s",
             status_code, status_text, strlen(body), body);
             
    send(client_socket, response, strlen(response), 0);
}

const char *get_mime_type(const char *path) {
    const char *ext = strrchr(path, '.');
    if (!ext) return "application/octet-stream";
    if (strcmp(ext, ".html") == 0) return "text/html";
    if (strcmp(ext, ".css") == 0) return "text/css";
    if (strcmp(ext, ".js") == 0) return "application/javascript";
    if (strcmp(ext, ".png") == 0) return "image/png";
    if (strcmp(ext, ".jpg") == 0 || strcmp(ext, ".jpeg") == 0) return "image/jpeg";
    if (strcmp(ext, ".gif") == 0) return "image/gif";
    return "text/plain";
}

void handle_http_request(int client_socket, const char *web_root) {
    char buffer[BUFFER_SIZE];
    ssize_t bytes_read = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
    
    if (bytes_read <= 0) return;
    buffer[bytes_read] = '\0';

    char method[16], path[256], protocol[16];
    
    // Validasi 1: Format Request Harus Lengkap (Menghindari 400 Bad Request)
    if (sscanf(buffer, "%15s %255s %15s", method, path, protocol) != 3) {
        send_http_error(client_socket, 400, "Bad Request");
        return;
    }

    // Validasi 2: Hanya izinkan method GET (405 Method Not Allowed)
    if (strcmp(method, "GET") != 0) {
        send_http_error(client_socket, 405, "Method Not Allowed");
        return;
    }

    // Validasi 3: Keamanan Path / Directory Traversal (403 Forbidden)
    // Mencegah klien mengakses ../../etc/passwd
    if (strstr(path, "..") != NULL) {
        send_http_error(client_socket, 403, "Forbidden");
        return;
    }

    // Redirect path "/" ke "/index.html"
    if (strcmp(path, "/") == 0) {
        strcpy(path, "/index.html");
    }

    char file_path[512];
    snprintf(file_path, sizeof(file_path), "%s%s", web_root, path);

    // Validasi 4: Buka file (404 Not Found atau 403 Forbidden jika tidak ada izin)
    FILE *file = fopen(file_path, "rb");
    if (!file) {
        // Cek apakah file sebenarnya ada tapi tidak bisa dibaca, atau memang tidak ada
        if (access(file_path, F_OK) == 0) {
            send_http_error(client_socket, 403, "Forbidden");
        } else {
            send_http_error(client_socket, 404, "Not Found");
        }
        return;
    }

    // Eksekusi: Kirim Header 200 OK dan isi file jika semua validasi lolos
    struct stat file_stat;
    stat(file_path, &file_stat);
    long file_size = file_stat.st_size;

    char header[512];
    snprintf(header, sizeof(header),
             "HTTP/1.1 200 OK\r\n"
             "Content-Type: %s\r\n"
             "Content-Length: %ld\r\n"
             "Connection: close\r\n\r\n",
             get_mime_type(file_path), file_size);
             
    send(client_socket, header, strlen(header), 0);

    size_t n;
    while ((n = fread(buffer, 1, sizeof(buffer), file)) > 0) {
        if (send(client_socket, buffer, n, 0) < 0) {
            perror("[WARNING] Failed to send file data to client");
            break; // Hentikan jika klien tiba-tiba menutup koneksi
        }
    }

    fclose(file);
}
