#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <ESP32TcpClient.h>

#define BUTTON1_PIN 10
#define BUTTON2_PIN 11
#define BUTTON3_PIN 12
#define BUTTON4_PIN 13
#define BUTTON5_PIN 14

// Moves
#define MOVE_UP "w"
#define MOVE_DOWN "s"
#define MOVE_LEFT "a"
#define MOVE_RIGHT "d"
#define RESTART "r"

#define Y_MAX 4000
#define Y_MIN 1000

// Define LCD pins
#define SDA 17
#define SCL 18

// Joystick pins
#define yAxisPin 4

// WiFi credentials
const char* ssid = "//";
const char* password = "//";

// MQTT Broker settings
const char* mqtt_server = "//";      // Replace with your broker IP
const int mqtt_port = 1883;        // Default MQTT port
const char* mqtt_username = "";    // Optional: MQTT username if required
const char* mqtt_password = "";    // Optional: MQTT password if required
const char* clientID = "ESP32_TTT"; // Client ID for MQTT connection
const char* topic_sub = "MUD";      // Topic to subscribe to for descriptions
const char* topic_pub = "MUD/moves";       // Topic to publish movement commands

// TCP settings
const char* tcp_server = "//";
const int tcp_port = 8888;  // Adjust to match your server port

// Initialize TCP client
ESP32TcpClient tcpClient(tcp_server, tcp_port, ssid, password);

// Initialize WiFi and MQTT client
WiFiClient espClient;
PubSubClient client(espClient);

// LCD initialization
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Variables to store button states
int button1State = HIGH;
int button2State = HIGH;
int button3State = HIGH;
int button4State = HIGH;
int button5State = HIGH;

// Prevent debounce
int lastButton1State = HIGH;
int lastButton2State = HIGH;
int lastButton3State = HIGH;
int lastButton4State = HIGH;
int lastButton5State = HIGH;

// Joystick variables
int lastYVal = 0;
int currentLine = 0;

// Dynamic storage for lines
char **lines = NULL;
int numLines = 0;

// Flag to indicate if we need to resubscribe
bool needToResubscribe = false;

// Buffer for accumulating messages
#define MAX_BUFFER_SIZE 1024
char messageBuffer[MAX_BUFFER_SIZE];
unsigned int bufferLength = 0;

// End of message marker - when we see this, we process the accumulated buffer
#define END_MARKER "Where would you like to go? (^/v/</>/r):"

// Function to test I2C address
bool i2CAddrTest(uint8_t addr) {
  Wire.beginTransmission(addr);
  return (Wire.endTransmission() == 0);
}

// Function to handle incoming MQTT messages
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.println(topic);

  // Convert payload to a null-terminated string
  char message[length + 1];
  memcpy(message, payload, length);
  message[length] = '\0';

  Serial.print("Message: ");
  Serial.println(message);

  // Check if the topic is for descriptions
  if (strcmp(topic, topic_sub) == 0) {
    // If the buffer might overflow, reset it
    if (bufferLength + length >= MAX_BUFFER_SIZE - 1) {
      bufferLength = 0;
    }

    // Append the new message to our buffer
    if (bufferLength > 0 && bufferLength < MAX_BUFFER_SIZE - 1) {
      // Add a space between accumulated messages
      messageBuffer[bufferLength++] = ' ';
    }

    // Copy the new message to the buffer
    memcpy(messageBuffer + bufferLength, message, length);
    bufferLength += length;
    messageBuffer[bufferLength] = '\0';

    // Check if we have received the end marker
    if (strstr(messageBuffer, END_MARKER) != NULL) {
      Serial.println("End marker detected, processing buffer");

      // Free the previous lines
      freeLines();

      // Process the complete message
      splitDescription(messageBuffer);

      // Display the first two lines
      currentLine = 0;
      displayTwoLines(currentLine);

      // Reset the buffer for the next set of messages
      bufferLength = 0;
    }
  }
}

// Function to publish movement commands
void publishMove(const char* move) {
  if (client.connected()) {
    client.publish(topic_pub, move);
    Serial.print("Published move: ");
    Serial.println(move);

    // Add a short delay to allow the MQTT server to process the command
    delay(100);

    // Process any pending messages after sending a command
    for(int i = 0; i < 20; i++) {
      client.loop();
      delay(50);
    }
  } else {
    Serial.println("MQTT not connected!");
    needToResubscribe = true;
  }
}

// Function to publish movement commands via TCP
void publishMoveTCP(const char* move) {
  if (tcpClient.isServerConnected()) {
    tcpClient.sendMessageLine(move);
    Serial.print("Published move TCP: ");
    Serial.println(move);

    // Add a short delay to allow the server to process the command
    delay(100);
  } else {
    Serial.println("TCP server not connected!");
    // Try to reconnect
    tcpClient.reconnect();
  }
}

void setup() {
  Wire.begin(SDA, SCL);
  if (!i2CAddrTest(0x27)) {
    lcd = LiquidCrystal_I2C(0x3F, 16, 2);
  }
  lcd.init();
  lcd.backlight();

  // Initialize Serial communication
  Serial.begin(115200);

  // Set button pins as inputs with internal pull-up resistors
  pinMode(BUTTON1_PIN, INPUT_PULLUP);
  pinMode(BUTTON2_PIN, INPUT_PULLUP);
  pinMode(BUTTON3_PIN, INPUT_PULLUP);
  pinMode(BUTTON4_PIN, INPUT_PULLUP);
  pinMode(BUTTON5_PIN, INPUT_PULLUP);

  Serial.println("Four Button Test Program Started");

  // For analog stick
  pinMode(yAxisPin, INPUT);
  Serial.println("Joystick scrolling test started");

  // Initialize WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  // Initialize TCP client for sending commands
  if (tcpClient.connectToServer()) {
    Serial.println("Connected to TCP server");
  } else {
    Serial.println("Failed to connect to TCP server. Will retry in loop.");
  }

  // Initialize MQTT client
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);

  // Connect to MQTT broker and subscribe
  connectMQTT();

  // Display initial message
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("MUD Client Ready");
  lcd.setCursor(0, 1);
  lcd.print("Waiting for data");
}

// Connect to MQTT broker function
void connectMQTT() {
  // Connect to MQTT broker
  while (!client.connected()) {
    Serial.print("Connecting to MQTT broker...");
    if (client.connect(clientID, mqtt_username, mqtt_password)) {
      Serial.println("connected");

      // Subscribe to the description topic
      client.subscribe(topic_sub);
      Serial.println("Subscribed to topic: MUD");
      needToResubscribe = false;
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void loop() {
  // Ensure the MQTT client stays connected
  if (!client.connected() || needToResubscribe) {
    connectMQTT();
  }

  if (!tcpClient.isServerConnected()) {
    tcpClient.reconnect();
  }

  // Handle incoming MQTT messages
  client.loop();

  // Read the state of each button
  button1State = digitalRead(BUTTON1_PIN);
  button2State = digitalRead(BUTTON2_PIN);
  button3State = digitalRead(BUTTON3_PIN);
  button4State = digitalRead(BUTTON4_PIN);
  button5State = digitalRead(BUTTON5_PIN);

  // Check Button 1 (Move North)
  if (button1State != lastButton1State) {
    if (button1State == LOW) {
      publishMoveTCP(MOVE_UP); // Move North
    }
    delay(50);
    lastButton1State = button1State;
  }

  // Check Button 2 (Move South)
  if (button2State != lastButton2State) {
    if (button2State == LOW) {
      publishMoveTCP(MOVE_DOWN); // Move South
    }
    delay(50);
    lastButton2State = button2State;
  }

  // Check Button 3 (Move East)
  if (button3State != lastButton3State) {
    if (button3State == LOW) {
      publishMoveTCP(MOVE_LEFT); // Move East
    }
    delay(50);
    lastButton3State = button3State;
  }

  // Check Button 4 (Move West)
  if (button4State != lastButton4State) {
    if (button4State == LOW) {
      publishMoveTCP(MOVE_RIGHT); // Move West
    }
    delay(50);
    lastButton4State = button4State;
  }

  // Check Button 5 (Quit)
  if (button5State != lastButton5State) {
    if (button5State == LOW) {
      publishMoveTCP(RESTART); // Quit
    }
    delay(50);
    lastButton5State = button5State;
  }

  // Read the Y-axis value of the joystick
  int yVal = analogRead(yAxisPin);

  // Check if the joystick is moved up or down
  if (yVal > Y_MAX && lastYVal <= Y_MAX) { // Joystick moved up
    if (currentLine > 0) {
      currentLine--;
      Serial.println("UP");
      displayTwoLines(currentLine);
    }
  } else if (yVal < Y_MIN && lastYVal >= Y_MIN) { // Joystick moved down
    if (currentLine < numLines - 2) {
      currentLine++;
      Serial.println("DOWN");
      displayTwoLines(currentLine);
    }
  }

  lastYVal = yVal;
  delay(100);
}

// Function to display two lines on the LCD
void displayTwoLines(int line) {
  static int lastLine = -1; // Keep track of the last displayed line
  if (line != lastLine) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(lines[line]);
    if (line + 1 < numLines) {
      lcd.setCursor(0, 1);
      lcd.print(lines[line + 1]);
    }
    lastLine = line;
  }
}

// Function to split a description into lines
void splitDescription(const char *description) {
  int len = strlen(description);
  numLines = 0; // Reset the number of lines

  // Allocate temporary storage for lines
  lines = NULL;

  int start = 0; // Start index of the current line
  while (start < len) {
    int end = start + 16; // Tentative end index for the current line

    // If the end index is beyond the description length, adjust it
    if (end >= len) {
      end = len;
    } else {
      // Move back to the last space to avoid splitting words
      while (end > start && description[end] != ' ') {
        end--;
      }

      // If no space was found, allow splitting at the 16th character
      if (end == start) {
        end = start + 16;
      }
    }

    // Allocate memory for the current line
    lines = (char **)realloc(lines, (numLines + 1) * sizeof(char *));
    lines[numLines] = (char *)malloc((end - start + 1) * sizeof(char));

    // Copy the line and null-terminate it
    strncpy(lines[numLines], description + start, end - start);
    lines[numLines][end - start] = '\0';

    // Move to the next line, skipping the space
    start = end;
    while (start < len && description[start] == ' ') {
      start++;
    }

    numLines++;
  }
}

// Function to free allocated memory for lines
void freeLines() {
  for (int i = 0; i < numLines; i++) {
    free(lines[i]);
  }
  free(lines);
  lines = NULL;
  numLines = 0;
}