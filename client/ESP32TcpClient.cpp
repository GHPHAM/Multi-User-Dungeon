#include "ESP32TcpClient.h"

ESP32TcpClient::ESP32TcpClient(const char* serverAddress, int port, const char* wifiSsid, const char* wifiPassword) {
    serverIP = serverAddress;
    serverPort = port;
    ssid = wifiSsid;
    password = wifiPassword;
    isConnected = false;
}

bool ESP32TcpClient::begin() {
    Serial.println("Initializing ESP32 TCP Client...");
    Serial.print("Connecting to WiFi: ");
    Serial.println(ssid);

    WiFi.begin(ssid, password);

    // Wait for connection with timeout
    int timeout = 20; // 10 seconds timeout
    while (WiFi.status() != WL_CONNECTED && timeout > 0) {
        delay(500);
        Serial.print(".");
        timeout--;
    }

    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("\nWiFi connection failed!");
        return false;
    }

    Serial.println("\nWiFi connected successfully");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());

    return true;
}

bool ESP32TcpClient::connectToServer() {
    Serial.print("Connecting to server at ");
    Serial.print(serverIP);
    Serial.print(":");
    Serial.println(serverPort);

    if (client.connect(serverIP, serverPort)) {
        isConnected = true;
        Serial.println("Connected to server");
        return true;
    } else {
        isConnected = false;
        Serial.println("Server connection failed");
        return false;
    }
}

bool ESP32TcpClient::isServerConnected() {
    isConnected = client.connected();
    return isConnected;
}

bool ESP32TcpClient::reconnect() {
    // First disconnect if already connected
    if (client.connected()) {
        client.stop();
    }

    isConnected = false;
    Serial.println("Attempting to reconnect to server...");

    if (client.connect(serverIP, serverPort)) {
        isConnected = true;
        Serial.println("Reconnected to server");
        return true;
    } else {
        Serial.println("Reconnection failed");
        return false;
    }
}

bool ESP32TcpClient::sendMessage(const char* message) {
    if (!isServerConnected()) {
        Serial.println("Cannot send message: Not connected to server");
        return false;
    }

    size_t bytesWritten = client.write(message);
    return bytesWritten > 0;
}

bool ESP32TcpClient::sendMessageLine(const char* message) {
    if (!isServerConnected()) {
        Serial.println("Cannot send message: Not connected to server");
        return false;
    }

    return client.println(message) > 0;
}

bool ESP32TcpClient::messageAvailable() {
    return client.available() > 0;
}

String ESP32TcpClient::readMessageLine() {
    if (!isServerConnected()) {
        return "";
    }

    return client.readStringUntil('\n');
}

int ESP32TcpClient::readRawData(char* buffer, int maxLength) {
    if (!isServerConnected() || !messageAvailable()) {
        return 0;
    }

    return client.readBytes(buffer, maxLength);
}

void ESP32TcpClient::disconnect() {
    if (isConnected) {
        client.stop();
        isConnected = false;
        Serial.println("Disconnected from server");
    }
}

bool ESP32TcpClient::isWiFiConnected() {
    return WiFi.status() == WL_CONNECTED;
}

String ESP32TcpClient::getLocalIP() {
    return WiFi.localIP().toString();
}