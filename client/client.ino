#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <WiFi.h>
#include <PubSubClient.h>

#define BUTTON1_PIN 10
#define BUTTON2_PIN 11
#define BUTTON3_PIN 12
#define BUTTON4_PIN 13
#define BUTTON5_PIN 14

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
    // Free the previous lines
    freeLines();

    // Split the new description into lines
    splitDescription(message);

    // Display the first two lines
    currentLine = 0;
    displayTwoLines(currentLine);
  }
}

// Function to publish movement commands
void publishMove(const char* move) {
  if (client.connected()) {
    client.publish(topic_pub, move);
    Serial.print("Published move: ");
    Serial.println(move);
  } else {
    Serial.println("MQTT not connected!");
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

  // Initialize MQTT client
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);

  // Connect to MQTT broker
  while (!client.connected()) {
    Serial.print("Connecting to MQTT broker...");
    if (client.connect(clientID, mqtt_username, mqtt_password)) {
      Serial.println("connected");

      // Subscribe to the description topic
      client.subscribe(topic_sub);
      Serial.println("Subscribed to topic: MUD/description");
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
  if (!client.connected()) {
    while (!client.connected()) {
      Serial.print("Reconnecting to MQTT broker...");
      if (client.connect(clientID, mqtt_username, mqtt_password)) {
        Serial.println("reconnected");

        // Re-subscribe to the description topic
        client.subscribe(topic_sub);
        Serial.println("Re-subscribed to topic: MUD/description");
      } else {
        Serial.print("failed, rc=");
        Serial.print(client.state());
        Serial.println(" try again in 5 seconds");
        delay(5000);
      }
    }
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
      publishMove("w"); // Move North
    }
    delay(50);
    lastButton1State = button1State;
  }

  // Check Button 2 (Move South)
  if (button2State != lastButton2State) {
    if (button2State == LOW) {
      publishMove("s"); // Move South
    }
    delay(50);
    lastButton2State = button2State;
  }

  // Check Button 3 (Move East)
  if (button3State != lastButton3State) {
    if (button3State == LOW) {
      publishMove("d"); // Move East
    }
    delay(50);
    lastButton3State = button3State;
  }

  // Check Button 4 (Move West)
  if (button4State != lastButton4State) {
    if (button4State == LOW) {
      publishMove("a"); // Move West
    }
    delay(50);
    lastButton4State = button4State;
  }

  // Check Button 5 (Quit)
  if (button5State != lastButton5State) {
    if (button5State == LOW) {
      publishMove("q"); // Quit
    }
    delay(50);
    lastButton5State = button5State;
  }

  // Read the Y-axis value of the joystick
  int yVal = analogRead(yAxisPin);

  // Check if the joystick is moved up or down
  if (yVal > 4000 && lastYVal <= 4000) { // Joystick moved up
    if (currentLine > 0) {
      currentLine--;
      displayTwoLines(currentLine);
    }
  } else if (yVal < 100 && lastYVal >= 100) { // Joystick moved down
    if (currentLine < numLines - 2) {
      currentLine++;
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
