/**
 * @file tcp_server.c
 * @brief Implementation of the TCP server library
 */

#include "tcp_server.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>

/* Default configuration values */
#define DEFAULT_PORT 8888
#define DEFAULT_MAX_CONNECTIONS 5
#define DEFAULT_BUFFER_SIZE 1024

tcp_server_t* tcp_server_init(void) {
    tcp_server_config_t config = {
        .port = DEFAULT_PORT,
        .max_connections = DEFAULT_MAX_CONNECTIONS,
        .buffer_size = DEFAULT_BUFFER_SIZE
    };
    return tcp_server_init_with_config(&config);
}

tcp_server_t* tcp_server_init_with_config(const tcp_server_config_t* config) {
    if (config == NULL) {
        fprintf(stderr, "Error: NULL config provided\n");
        return NULL;
    }

    tcp_server_t* server = (tcp_server_t*)malloc(sizeof(tcp_server_t));
    if (server == NULL) {
        perror("Failed to allocate server");
        return NULL;
    }

    /* Initialize the server structure */
    memset(server, 0, sizeof(tcp_server_t));
    server->buffer_size = config->buffer_size;
    server->buffer = (char*)malloc(server->buffer_size);
    if (server->buffer == NULL) {
        perror("Failed to allocate buffer");
        free(server);
        return NULL;
    }

    /* Create TCP socket */
    server->server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server->server_fd == -1) {
        perror("Socket creation failed");
        free(server->buffer);
        free(server);
        return NULL;
    }

    /* Set socket options to reuse address */
    int opt = 1;
    if (setsockopt(server->server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("setsockopt failed");
        close(server->server_fd);
        free(server->buffer);
        free(server);
        return NULL;
    }

    /* Configure server address */
    memset(&server->server_addr, 0, sizeof(server->server_addr));
    server->server_addr.sin_family = AF_INET;
    server->server_addr.sin_addr.s_addr = INADDR_ANY;
    server->server_addr.sin_port = htons(config->port);

    /* Bind socket to address and port */
    if (bind(server->server_fd, (struct sockaddr*)&server->server_addr, sizeof(server->server_addr)) != 0) {
        perror("Binding failed");
        close(server->server_fd);
        free(server->buffer);
        free(server);
        return NULL;
    }

    /* Set max connections */
    if (listen(server->server_fd, config->max_connections) != 0) {
        perror("Listening failed");
        close(server->server_fd);
        free(server->buffer);
        free(server);
        return NULL;
    }

    server->is_connected = 0;
    server->client_fd = -1;

    return server;
}

int tcp_server_start(tcp_server_t* server) {
    if (server == NULL || server->server_fd < 0) {
        fprintf(stderr, "Invalid server handle\n");
        return -1;
    }

    /* Server is already started by the init function */
    return 0;
}

int tcp_server_accept(tcp_server_t* server) {
    if (server == NULL || server->server_fd < 0) {
        fprintf(stderr, "Invalid server handle\n");
        return -1;
    }

    /* Close previous client connection if exists */
    if (server->is_connected && server->client_fd >= 0) {
        tcp_server_disconnect_client(server);
    }

    /* Accept new connection */
    socklen_t client_len = sizeof(server->client_addr);
    server->client_fd = accept(server->server_fd, (struct sockaddr*)&server->client_addr, &client_len);

    if (server->client_fd < 0) {
        perror("Accept failed");
        return -1;
    }

    server->is_connected = 1;
    return 0;
}

int tcp_server_send(tcp_server_t* server, const char* message, size_t length) {
    if (server == NULL || !server->is_connected || server->client_fd < 0) {
        fprintf(stderr, "No client connection\n");
        return -1;
    }

    if (message == NULL) {
        fprintf(stderr, "NULL message provided\n");
        return -1;
    }

    ssize_t bytes_sent = send(server->client_fd, message, length, 0);
    if (bytes_sent < 0) {
        perror("Send failed");
        return -1;
    }

    return (int)bytes_sent;
}

int tcp_server_receive(tcp_server_t* server) {
    if (server == NULL || !server->is_connected || server->client_fd < 0) {
        fprintf(stderr, "No client connection\n");
        return -1;
    }

    /* Clear the buffer before receiving */
    memset(server->buffer, 0, server->buffer_size);

    ssize_t bytes_received = recv(server->client_fd, server->buffer, server->buffer_size - 1, 0);
    if (bytes_received < 0) {
        perror("Receive failed");
        return -1;
    } else if (bytes_received == 0) {
        /* Client disconnected */
        server->is_connected = 0;
        close(server->client_fd);
        server->client_fd = -1;
        return 0;
    }

    /* Ensure null termination */
    server->buffer[bytes_received] = '\0';
    return (int)bytes_received;
}

const char* tcp_server_get_message(const tcp_server_t* server) {
    if (server == NULL) {
        fprintf(stderr, "Invalid server handle\n");
        return NULL;
    }
    return server->buffer;
}

void tcp_server_disconnect_client(tcp_server_t* server) {
    if (server == NULL || !server->is_connected || server->client_fd < 0) {
        return;
    }

    close(server->client_fd);
    server->client_fd = -1;
    server->is_connected = 0;
}

void tcp_server_cleanup(tcp_server_t* server) {
    if (server == NULL) {
        return;
    }

    /* Disconnect client if connected */
    if (server->is_connected && server->client_fd >= 0) {
        close(server->client_fd);
    }

    /* Close server socket */
    if (server->server_fd >= 0) {
        close(server->server_fd);
    }

    /* Free allocated memory */
    free(server->buffer);
    free(server);
}