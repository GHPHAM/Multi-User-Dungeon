// Four Button Arduino Test Program

// Define button pins
const int BUTTON1_PIN = 2;
const int BUTTON2_PIN = 3;
const int BUTTON3_PIN = 4;
const int BUTTON4_PIN = 5;

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

void setup() {
  // Initialize Serial communication
  Serial.begin(115200);
  
  // Set button pins as inputs with internal pull-up resistors
  pinMode(BUTTON1_PIN, INPUT_PULLUP);
  pinMode(BUTTON2_PIN, INPUT_PULLUP);
  pinMode(BUTTON3_PIN, INPUT_PULLUP);
  pinMode(BUTTON4_PIN, INPUT_PULLUP);
  
  Serial.println("Four Button Test Program Started");
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
    }
    // Small delay to debounce
    delay(50);
    lastButton1State = button1State;
  }
  
  // Check Button 2
  if (button2State != lastButton2State) {
    if (button2State == LOW) {
      Serial.println("Button 2");
    }
    delay(50);
    lastButton2State = button2State;
  }
  
  // Check Button 3
  if (button3State != lastButton3State) {
    if (button3State == LOW) {
      Serial.println("Button 3");
    }
    delay(50);
    lastButton3State = button3State;
  }
  
  // Check Button 4
  if (button4State != lastButton4State) {
    if (button4State == LOW) {
      Serial.println("Button 4");
    }
    delay(50);
    lastButton4State = button4State;
  }
}