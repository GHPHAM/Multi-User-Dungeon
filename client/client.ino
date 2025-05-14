#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <WiFi.h>
#include <PubSubClient.h>

// Define button pins
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
const char* ssid = "";       // Replace with your WiFi name
const char* password = "";       // Replace with your WiFi password

// MQTT Broker settings
const char* mqtt_server = "";      // Replace with your broker IP
const int mqtt_port = 1883;                    // Default MQTT port
const char* mqtt_username = "";                // Optional: MQTT username if required
const char* mqtt_password = "";                // Optional: MQTT password if required
const char* clientID = "ESP32_TTT";            // Client ID for MQTT connection
const char* topic_sub = "MUD";                   // Topic to subscribe to for game control
const char* topic_push = "MUD/moves";                   // Topic to push game control to

// Initialize WiFi and MQTT client - GLOBAL DECLARATIONS
WiFiClient espClient;
PubSubClient client(espClient);

// LCD initialization
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Variables to store button states
int button1State = HIGH;  // Default state (not pressed)
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
char* description = NULL;

void publishButtonPress(const char* buttonName) {
  String message = String("Button Pressed: ") + buttonName;
  Serial.println("Publishing: " + message);
  client.publish("esp32/button", message.c_str());
}

void mqttCallback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("]: ");

  // Free previous memory if any
  if (description != NULL) {
    free(description);
  }

  // Allocate new memory for the description (+1 for null terminator)
  description = (char*)malloc(length + 1);

  if (description != NULL) {
    memcpy(description, payload, length);
    description[length] = '\0';  // Null-terminate the string

    Serial.println(description); // Optional: print to Serial

    // Process the description text
    splitDescription(description);
    currentLine = 0;
    displayTwoLines(currentLine);
  } else {
    Serial.println("Failed to allocate memory for description");
  }
}


void connectToWiFi() { //added wifi test
  Serial.print("Connecting to WiFi");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" Connected to WiFi");
}

void reconnectMQTT() { //added MQTT conection test
  while (!client.connected()) {
    Serial.print("Connecting to MQTT...");
    if (client.connect(clientID, mqtt_username, mqtt_password)) {
      Serial.println("connected");
      client.subscribe(topic_sub);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

bool i2CAddrTest(uint8_t addr) {
  Wire.beginTransmission(addr);
  if (Wire.endTransmission() == 0) {
    return true;
  }
  return false;
}

void setup() {
  Wire.begin(SDA, SCL);           // attach the IIC pin
  if (!i2CAddrTest(0x27)) {
    lcd = LiquidCrystal_I2C(0x3F, 16, 2);
  }
  lcd.init();                     // LCD driver initialization
  lcd.backlight();                // Open the backlight

  // Initialize Serial communication
  Serial.begin(115200);

  // Initialize wifi and MQTT communication
  connectToWiFi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(mqttCallback);
  reconnectMQTT();
  client.subscribe(topic_sub);



  // Set button pins as inputs with internal pull-up resistors
  pinMode(BUTTON1_PIN, INPUT_PULLUP);
  pinMode(BUTTON2_PIN, INPUT_PULLUP);
  pinMode(BUTTON3_PIN, INPUT_PULLUP);
  pinMode(BUTTON4_PIN, INPUT_PULLUP);
  pinMode(BUTTON5_PIN, INPUT_PULLUP);

  Serial.println("Four Button Test Program Started");

  //For analog stick
  pinMode(yAxisPin, INPUT);
    Serial.println("Joystick scrolling test started");

    // Example description
    description = "Foxes are small to medium-sized omnivorous mammals belonging to several genera of the family Canidae. They have a flattened skull, upright triangular ears, a pointed slightly upturned snout, and a long bushy tail.";

    // Split the description into lines
    splitDescription(description);

    // Display the first line
    displayTwoLines(currentLine);
}

void lcdDisplayTest(char* string)
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(string);
}

void loop() {
  // Read the state of each button
  button1State = digitalRead(BUTTON1_PIN);
  button2State = digitalRead(BUTTON2_PIN);
  button3State = digitalRead(BUTTON3_PIN);
  button4State = digitalRead(BUTTON4_PIN);
  button5State = digitalRead(BUTTON5_PIN);

  // Check Button 1
  if (button1State != lastButton1State) {
    if (button1State == LOW) {
      String message = String("W");
      client.publish(topic_push, message.c_str());
    }
    // Small delay to debounce
    delay(50);
    lastButton1State = button1State;
  }

  // Check Button 2
  if (button2State != lastButton2State) {
    if (button2State == LOW) {
      String message = String("S");
      client.publish(topic_push, message.c_str());
    }
    delay(50);
    lastButton2State = button2State;
  }

  // Check Button 3
  if (button3State != lastButton3State) {
    if (button3State == LOW) {
      String message = String("A");
      client.publish(topic_push, message.c_str());
    }
    delay(50);
    lastButton3State = button3State;
  }

  // Check Button 4
  if (button4State != lastButton4State) {
    if (button4State == LOW) {
      String message = String("D");
      client.publish(topic_push, message.c_str());
    }
    delay(50);
    lastButton4State = button4State;
  }
    // Check Button 5
  if (button5State != lastButton5State) {
    if (button5State == LOW) {
      String message = String("Q");
      client.publish(topic_push, message.c_str());
    }
    delay(50);
    lastButton5State = button5State;
  }


  // Read the Y-axis value of the joystick
  int yVal = analogRead(yAxisPin);

  // Check if the joystick is moved up or down
  if (yVal > 4000 && lastYVal <= 4000) // Joystick moved up
  {
    if (currentLine > 0)
    {
      currentLine--;
      Serial.println("Joystick moved up");
      displayTwoLines(currentLine);
    }
  }
  else if (yVal < 100 && lastYVal >= 100) // Joystick moved down
  {
    if (currentLine < numLines - 2)
    {
      currentLine++;
      Serial.println("Joystick moved down");
      displayTwoLines(currentLine);
    }
  }

  lastYVal = yVal; // Update the last Y-axis value
  delay(100);
}

//For LCD description
void displayTwoLines(int line)
{
    static int lastLine = -1; // Keep track of the last displayed line
    if (line != lastLine)
    {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print(lines[line]);
        if (line + 1 < numLines)
        {
            lcd.setCursor(0, 1);
            lcd.print(lines[line + 1]);
        }

        lastLine = line;
    }
}

// Function to split a description into lines
void splitDescription(const char *description)
{
    int len = strlen(description);
    numLines = 0; // Reset the number of lines

    // Allocate temporary storage for lines
    lines = NULL;

    int start = 0; // Start index of the current line
    while (start < len)
    {
      int end = start + 16; // Tentative end index for the current line

      // If the end index is beyond the description length, adjust it
      if (end >= len)
      {
          end = len;
      }
      else
      {
        // Move back to the last space to avoid splitting words
        while (end > start && description[end] != ' ')
        {
          end--;
        }

        // If no space was found, allow splitting at the 16th character
        if (end == start)
        {
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
    while (start < len && description[start] == ' ')
    {
        start++;
    }

    numLines++;
  }
}

// Function to free allocated memory for lines
void freeLines()
{
    for (int i = 0; i < numLines; i++)
    {
        free(lines[i]);
    }
    free(lines);
    lines = NULL;
    numLines = 0;
}
