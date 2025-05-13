#ifndef MQTT_H
#define MQTT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <pthread.h>

// MQTT Configuration
extern char* MQTT_HOST; // MQTT broker address
#define MQTT_TOPIC "MUD"

// Global variables
extern char mosquittoPath[];
extern char mosquittoSub[];
extern int mqtt_pipe_fd[2];
extern pid_t mqtt_sub_pid;
extern int listener_running;
extern char currentMove;

// Function declarations
void publishMessage(const char *message);
void startListener();
void stopListener();
void* mqttListenerThread(void *arg);
void mqttUpdate(const char *topic, const char *message);
char getMQTTInput();

#endif // MQTT_H