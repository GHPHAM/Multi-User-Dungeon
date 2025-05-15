/**
 * @file tcp_server.h
 * @brief Header file for a simple TCP server library
 *
 * This library provides an abstraction for creating and managing TCP servers
 * with simple connection handling capabilities.
 */

#ifndef TCP_SERVER_H
#define TCP_SERVER_H

#include <sys/socket.h>
#include <netinet/in.h>

/**
 * @brief TCP Server configuration structure
 */
typedef struct {
    int port;              /**< Port number to listen on */
    int max_connections;   /**< Maximum number of pending connections */
    int buffer_size;       /**< Size of message buffer */
} tcp_server_config_t;

/**
 * @brief TCP Server handle structure
 */
typedef struct {
    int server_fd;                 /**< Server socket file descriptor */
    int client_fd;                 /**< Connected client socket file descriptor */
    struct sockaddr_in server_addr; /**< Server address information */
    struct sockaddr_in client_addr; /**< Client address information */
    char* buffer;                  /**< Message buffer */
    int buffer_size;               /**< Size of message buffer */
    int is_connected;              /**< Flag indicating if a client is connected */
} tcp_server_t;

/**
 * @brief Initialize a TCP server with default configuration
 *
 * Default configuration: port 8888, max 5 connections, 1024 buffer size
 *
 * @return Initialized TCP server handle or NULL on failure
 */
tcp_server_t* tcp_server_init(void);

/**
 * @brief Initialize a TCP server with custom configuration
 *
 * @param config Server configuration parameters
 * @return Initialized TCP server handle or NULL on failure
 */
tcp_server_t* tcp_server_init_with_config(const tcp_server_config_t* config);

/**
 * @brief Start the server and begin listening for connections
 *
 * @param server Server handle
 * @return 0 on success, -1 on failure
 */
int tcp_server_start(tcp_server_t* server);

/**
 * @brief Accept a client connection
 *
 * This function blocks until a client connects.
 *
 * @param server Server handle
 * @return 0 on success, -1 on failure
 */
int tcp_server_accept(tcp_server_t* server);

/**
 * @brief Send a message to the connected client
 *
 * @param server Server handle
 * @param message Message to send
 * @param length Length of the message
 * @return Number of bytes sent or -1 on failure
 */
int tcp_server_send(tcp_server_t* server, const char* message, size_t length);

/**
 * @brief Receive a message from the connected client
 *
 * @param server Server handle
 * @return Number of bytes received or -1 on error
 */
int tcp_server_receive(tcp_server_t* server);

/**
 * @brief Get the received message buffer
 *
 * @param server Server handle
 * @return Pointer to the message buffer
 */
const char* tcp_server_get_message(const tcp_server_t* server);

/**
 * @brief Disconnect the current client
 *
 * @param server Server handle
 */
void tcp_server_disconnect_client(tcp_server_t* server);

/**
 * @brief Stop the server and free resources
 *
 * @param server Server handle
 */
void tcp_server_cleanup(tcp_server_t* server);

#endif /* TCP_SERVER_H */