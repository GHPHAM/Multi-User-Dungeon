#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <WiFi.h>
#include <PubSubClient.h>

// Define button pins
#define BUTTON1_PIN 13
#define BUTTON2_PIN 12
#define BUTTON3_PIN 11
#define BUTTON4_PIN 10
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
  pinMode(BUTTON5_PIN, INPUT_PULLUP);

  Serial.println("Four Button Test Program Started");

  //For analog stick
  pinMode(yAxisPin, INPUT);
    Serial.println("Joystick scrolling test started");

    // Example description
    char* description = "Foxes are small to medium-sized omnivorous mammals belonging to several genera of the family Canidae. They have a flattened skull, upright triangular ears, a pointed slightly upturned snout, and a long bushy tail.";

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
    // Check Button 5
  if (button5State != lastButton5State) {
    if (button5State == LOW) {
      Serial.println("Button 5");
      lcdDisplayTest("Button 5");
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
