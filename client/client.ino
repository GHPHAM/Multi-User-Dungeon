#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <WiFi.h>
#include <PubSubClient.h>

// Define button pins
#define BUTTON1_PIN 2
#define BUTTON2_PIN 3
#define BUTTON3_PIN 4
#define BUTTON4_PIN 5

// Define LCD pins
#define SDA 14
#define SCL 13

// WiFi credentials
const char* ssid = "";       // Replace with your WiFi name
const char* password = "";       // Replace with your WiFi password

// MQTT Broker settings
const char* mqtt_server = "";      // Replace with your broker IP
const int mqtt_port = 1883;                    // Default MQTT port
const char* mqtt_username = "";                // Optional: MQTT username if required
const char* mqtt_password = "";                // Optional: MQTT password if required
const char* clientID = "ESP32_TTT";            // Client ID for MQTT connection
const char* topic_sub = "TTT";                 // Topic to subscribe to for game control

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

// Prevent debounce
int lastButton1State = HIGH;
int lastButton2State = HIGH;
int lastButton3State = HIGH;
int lastButton4State = HIGH;

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

  // Set button pins as inputs with internal pull-up resistors
  pinMode(BUTTON1_PIN, INPUT_PULLUP);
  pinMode(BUTTON2_PIN, INPUT_PULLUP);
  pinMode(BUTTON3_PIN, INPUT_PULLUP);
  pinMode(BUTTON4_PIN, INPUT_PULLUP);

  Serial.println("Four Button Test Program Started");
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

  // Check Button 1
  if (button1State != lastButton1State) {
    if (button1State == LOW) {
      Serial.println("Button 1");
      lcdDisplayTest("Button 1");
    }
    // Small delay to debounce
    delay(50);
    lastButton1State = button1State;
  }

  // Check Button 2
  if (button2State != lastButton2State) {
    if (button2State == LOW) {
      Serial.println("Button 2");
      lcdDisplayTest("Button 2");
    }
    delay(50);
    lastButton2State = button2State;
  }

  // Check Button 3
  if (button3State != lastButton3State) {
    if (button3State == LOW) {
      Serial.println("Button 3");
      lcdDisplayTest("Button 3");
    }
    delay(50);
    lastButton3State = button3State;
  }

  // Check Button 4
  if (button4State != lastButton4State) {
    if (button4State == LOW) {
      Serial.println("Button 4");
      lcdDisplayTest("Button 4");
    }
    delay(50);
    lastButton4State = button4State;
  }
}