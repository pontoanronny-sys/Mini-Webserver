#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include "server.h"
#include "http.h"

typedef struct {
    int client_socket;
    char web_root[256];
} thread_arg_t;

void *client_thread_handler(void *arg) {
    thread_arg_t *t_arg = (thread_arg_t *)arg;
    
    // Panggil Tugas 3 (Protokol)
    handle_http_request(t_arg->client_socket, t_arg->web_root);
    
    // Tutup koneksi dan bersihkan memori
    close(t_arg->client_socket);
    free(t_arg);
    
    pthread_exit(NULL);
}

void start_server(int port, const char *web_root) {
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);

    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        perror("[ERROR] Socket creation failed");
        exit(EXIT_FAILURE);
    }

    int opt = 1;
    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("[ERROR] Setsockopt failed");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("[ERROR] Bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_socket, 10) < 0) {
        perror("[ERROR] Listen failed");
        exit(EXIT_FAILURE);
    }

    printf("[INFO] Server is listening on http://localhost:%d\n", port);

    while (1) {
        client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_len);
        if (client_socket < 0) {
            perror("[WARNING] Accept failed");
            continue;
        }

        // Pengecekan error alokasi memori (Mencegah 500 Internal Error dari sisi server crash)
        thread_arg_t *arg = malloc(sizeof(thread_arg_t));
        if (arg == NULL) {
            perror("[ERROR] Failed to allocate memory for thread arguments");
            send_http_error(client_socket, 500, "Internal Server Error");
            close(client_socket);
            continue;
        }

        arg->client_socket = client_socket;
        strncpy(arg->web_root, web_root, sizeof(arg->web_root) - 1);

        pthread_t thread_id;
        if (pthread_create(&thread_id, NULL, client_thread_handler, (void *)arg) != 0) {
            perror("[ERROR] Could not create thread");
            send_http_error(client_socket, 500, "Internal Server Error");
            free(arg);
            close(client_socket);
        } else {
            pthread_detach(thread_id);
        }
    }
    
    close(server_socket);
}
