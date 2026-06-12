#include <stdio.h>
#include <stdlib.h>
#include "server.h"

#define DEFAULT_PORT 8080
#define WEB_ROOT "./public"

int main(int argc, char *argv[]) {
    int port = DEFAULT_PORT;
    
    if (argc > 1) {
        port = atoi(argv[1]);
    }

    printf("[INFO] Starting server on port %d...\n", port);
    printf("[INFO] Serving files from %s\n", WEB_ROOT);
    
    start_server(port, WEB_ROOT);

    return 0;
}
