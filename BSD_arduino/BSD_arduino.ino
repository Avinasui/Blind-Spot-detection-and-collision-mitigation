#define TRIG_PIN_1 4  // Trigger pin for Sensor 1
#define ECHO_PIN_1 5  // Echo pin for Sensor 1
#define TRIG_PIN_2 18 // Trigger pin for Sensor 2
#define ECHO_PIN_2 19 // Echo pin for Sensor 2

#define BUZZER_PIN 21 // Buzzer pin
#define REL_PIN 32   // Relay pin
#define TOGGLE_PIN_1 34 // First switch pin
#define TOGGLE_PIN_2 35 // Second switch pin
#define led_1 2 // First LED pin
#define led_2 0 // Second LED pin

const int MINIMUM_DISTANCE = 10;  // Minimum range in cm
const int MAXIMUM_DISTANCE = 30; // Maximum range in cm

bool useSensor1 = true;  // Flag to toggle between sensors

// Function to measure distance
long getDistance(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH);
  long distance = duration * 0.034 / 2; // Convert to cm
  return distance;
}

// Function to beep the buzzer with variable intervals
void beepBuzzer(int interval) {
  digitalWrite(BUZZER_PIN, HIGH);
  delay(interval / 2);
  digitalWrite(BUZZER_PIN, LOW);
  delay(interval / 2);
}

void setup() {
  Serial.begin(115200); // Initialize serial communication

  // Set pin modes for sensors, buzzer, relay, and toggle switches
  pinMode(TRIG_PIN_1, OUTPUT);
  pinMode(ECHO_PIN_1, INPUT);
  pinMode(TRIG_PIN_2, OUTPUT);
  pinMode(ECHO_PIN_2, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(REL_PIN, OUTPUT);
  pinMode(led_1, OUTPUT);
  pinMode(led_2, OUTPUT);
  pinMode(TOGGLE_PIN_1, INPUT_PULLUP);  // Set toggle switch pins as input with pull-up
  pinMode(TOGGLE_PIN_2, INPUT_PULLUP);

  Serial.println("Blind Spot Detection System Initialized");
}

void loop() {
  // Read the state of the toggle switches (using two pins for toggle functionality)
  int toggleState1 = digitalRead(TOGGLE_PIN_1);
  int toggleState2 = digitalRead(TOGGLE_PIN_2);

  // If either of the toggle pins is pressed (assuming LOW for pressed), change the sensor
  if (toggleState1 == LOW) {
    useSensor1 = true;  // Use Sensor 1
    delay(200); // Debounce delay to prevent multiple toggles from a single press
  } else if (toggleState2 == LOW) {
    useSensor1 = false; // Use Sensor 2
    delay(200); // Debounce delay to prevent multiple toggles from a single press
  }

  // Select the active sensor based on the flag
  long distance = 0;
  if (useSensor1) {
    // Get distance from Sensor 1
    distance = getDistance(TRIG_PIN_1, ECHO_PIN_1);
    digitalWrite(led_1, HIGH);
    digitalWrite(led_2, LOW);
    Serial.print("Sensor 1 Distance: ");
  } else {
    // Get distance from Sensor 2
    distance = getDistance(TRIG_PIN_2, ECHO_PIN_2);
    digitalWrite(led_2, HIGH);
    digitalWrite(led_1, LOW);
    Serial.print("Sensor 2 Distance: ");
  }

  Serial.print(distance);
  Serial.println(" cm");

  // Control relay, LED, and buzzer based on distance
  if (distance < MINIMUM_DISTANCE) {
    digitalWrite(BUZZER_PIN, HIGH);  // Turn on buzzer continuously when distance < 10cm
    digitalWrite(REL_PIN, LOW);     // Turn off relay
    Serial.println("Object very close! Relay OFF, Buzzer ON");
  }
  else if (distance >= MINIMUM_DISTANCE && distance <= MAXIMUM_DISTANCE) {
    digitalWrite(REL_PIN, HIGH);    // Turn on relay

    // Buzzer frequency logic based on distance
    if (distance >= 10 && distance <= 14) {
      beepBuzzer(100); // Fast beeping
    } 
    else if (distance >= 15 && distance <= 19) {
      beepBuzzer(300); // Medium-speed beeping
    } 
    else if (distance == 20) {
      beepBuzzer(500); // Slow beeping
    }
  } else {
    digitalWrite(REL_PIN, HIGH);    // Turn on relay
    digitalWrite(BUZZER_PIN, LOW); // Turn off buzzer
    Serial.println("No object within specified range. Relay ON");
  }

  delay(100); // Delay before the next reading
}
