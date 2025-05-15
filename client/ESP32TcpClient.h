#ifndef ESP32_TCP_CLIENT_H
#define ESP32_TCP_CLIENT_H

#include <WiFi.h>

class ESP32TcpClient {
private:
    WiFiClient client;
    const char* serverIP;
    int serverPort;
    bool isConnected;

    // WiFi credentials
    const char* ssid;
    const char* password;

    // Buffer for receiving messages
    static const int MAX_MESSAGE_LENGTH = 1024;
    char messageBuffer[MAX_MESSAGE_LENGTH];

public:
    /**
     * Constructor
     *
     * @param serverAddress IP address of the server to connect to
     * @param port Port number of the server
     * @param wifiSsid WiFi network name
     * @param wifiPassword WiFi password
     */
    ESP32TcpClient(const char* serverAddress, int port, const char* wifiSsid, const char* wifiPassword);

    /**
     * Initialize the client and connect to WiFi
     *
     * @return true if successfully connected to WiFi, false otherwise
     */
    bool begin();

    /**
     * Connect to the TCP server
     *
     * @return true if successfully connected, false otherwise
     */
    bool connectToServer();

    /**
     * Check if client is connected to the server
     *
     * @return true if connected, false otherwise
     */
    bool isServerConnected();

    /**
     * Reconnect to the server if connection is lost
     *
     * @return true if reconnection successful, false otherwise
     */
    bool reconnect();

    /**
     * Send a message to the server
     *
     * @param message The message to send
     * @return true if message was sent successfully, false otherwise
     */
    bool sendMessage(const char* message);

    /**
     * Send a message to the server with line ending
     *
     * @param message The message to send
     * @return true if message was sent successfully, false otherwise
     */
    bool sendMessageLine(const char* message);

    /**
     * Check if data is available from the server
     *
     * @return true if data is available, false otherwise
     */
    bool messageAvailable();

    /**
     * Read a message from the server until newline
     *
     * @return String containing the received message
     */
    String readMessageLine();

    /**
     * Read raw data from the server
     *
     * @param buffer Buffer to store the data
     * @param maxLength Maximum length of data to read
     * @return Number of bytes read
     */
    int readRawData(char* buffer, int maxLength);

    /**
     * Disconnect from the server
     */
    void disconnect();

    /**
     * Get the WiFi connection status
     *
     * @return true if connected to WiFi, false otherwise
     */
    bool isWiFiConnected();

    /**
     * Get the local IP address
     *
     * @return String containing the local IP address
     */
    String getLocalIP();
};

#endif // ESP32_TCP_CLIENT_H