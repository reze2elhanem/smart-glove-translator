/*
 * Smart Translator Gloves
 * Gesture Recognition System for Communication Assistance
 * 
 * Features:
 * - Reads 5 flex sensors and MPU6050 (accelerometer + gyroscope)
 * - Maps gestures to 10 predefined words/phrases
 * - Displays results on LCD screen
 * - Includes voice output capability
 * - Real-time gesture recognition with filtering
 */

// Include necessary libraries
#include <Wire.h>
#include <LiquidCrystal_I2C.h>  // For I2C LCD
#include <MPU6050.h>            // For MPU6050 sensor

// LCD Configuration (I2C address typically 0x27 or 0x3F)
LiquidCrystal_I2C lcd(0x27, 16, 2);  // 16x2 LCD

// MPU6050 sensor object
MPU6050 mpu;

// Flex Sensor Pin Definitions
const int FLEX_PINS[] = {A0, A1, A2, A3, A4};  // Thumb to Pinky
const int NUM_FLEX_SENSORS = 5;

// Gesture Vocabulary
const String GESTURE_WORDS[] = {
  "Hello",        // Gesture 0
  "Yes",          // Gesture 1
  "No",           // Gesture 2
  "Thank you",    // Gesture 3
  "Please",       // Gesture 4
  "Help",         // Gesture 5
  "I'm fine",     // Gesture 6
  "Good morning", // Gesture 7
  "Sorry",        // Gesture 8
  "Goodbye"       // Gesture 9
};
const int NUM_GESTURES = 10;

// Sensor Data Storage
int flexValues[NUM_FLEX_SENSORS];
int flexBaseline[NUM_FLEX_SENSORS];  // Calibration values
float accel[3];  // x, y, z acceleration
float gyro[3];   // x, y, z gyroscope

// Filtering and Timing
unsigned long lastGestureTime = 0;
const unsigned long GESTURE_COOLDOWN = 2000;  // 2 seconds between detections
const int SMOOTHING_WINDOW = 10;
int flexHistory[NUM_FLEX_SENSORS][SMOOTHING_WINDOW];
int historyIndex = 0;

// Voice Module Pins (Optional - uncomment when connected)
// const int VOICE_PIN = 3;

void setup() {
  // Initialize Serial Communication
  Serial.begin(9600);
  Serial.println("Smart Translator Gloves Initializing...");

  // Initialize LCD
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Smart Gloves");
  lcd.setCursor(0, 1);
  lcd.print("Initializing...");
  delay(1000);

  // Initialize MPU6050
  Wire.begin();
  mpu.initialize();
  
  // Verify MPU6050 connection
  if (mpu.testConnection()) {
    Serial.println("MPU6050 connection successful");
    lcd.clear();
    lcd.print("MPU6050: OK");
  } else {
    Serial.println("MPU6050 connection failed");
    lcd.clear();
    lcd.print("MPU6050: ERROR");
    while(1);  // Halt if sensor not found
  }
  delay(1000);

  // Initialize Flex Sensor Pins
  for (int i = 0; i < NUM_FLEX_SENSORS; i++) {
    pinMode(FLEX_PINS[i], INPUT);
  }

  // Calibrate Sensors
  calibrateSensors();
  
  // Initialize smoothing history
  initializeSmoothing();

  // Display ready message
  lcd.clear();
  lcd.print("Ready for");
  lcd.setCursor(0, 1);
  lcd.print("Gestures!");
  
  Serial.println("System Ready!");
}

void loop() {
  // Read all sensors
  readSensors();
  
  // Process gesture recognition
  int detectedGesture = recognizeGesture();
  
  // Display result if valid gesture detected
  if (detectedGesture != -1 && 
      (millis() - lastGestureTime) > GESTURE_COOLDOWN) {
    
    displayGesture(detectedGesture);
    // speakGesture(detectedGesture);  // Uncomment when voice module connected
    
    lastGestureTime = millis();
  }
  
  // Small delay for stability
  delay(50);
}

void calibrateSensors() {
  Serial.println("Calibrating sensors...");
  lcd.clear();
  lcd.print("Calibrating...");
  lcd.setCursor(0, 1);
  lcd.print("Keep hand flat");
  
  delay(3000);  // Allow time to position hand
  
  // Take multiple readings for baseline
  for (int i = 0; i < NUM_FLEX_SENSORS; i++) {
    int sum = 0;
    for (int j = 0; j < 100; j++) {
      sum += analogRead(FLEX_PINS[i]);
      delay(10);
    }
    flexBaseline[i] = sum / 100;
  }
  
  Serial.println("Calibration complete:");
  for (int i = 0; i < NUM_FLEX_SENSORS; i++) {
    Serial.print("Flex "); Serial.print(i); 
    Serial.print(": "); Serial.println(flexBaseline[i]);
  }
}

void initializeSmoothing() {
  for (int i = 0; i < NUM_FLEX_SENSORS; i++) {
    for (int j = 0; j < SMOOTHING_WINDOW; j++) {
      flexHistory[i][j] = analogRead(FLEX_PINS[i]);
    }
  }
}

void readSensors() {
  // Read and smooth flex sensors
  for (int i = 0; i < NUM_FLEX_SENSORS; i++) {
    int rawValue = analogRead(FLEX_PINS[i]);
    
    // Update smoothing history
    flexHistory[i][historyIndex] = rawValue;
    
    // Calculate smoothed value
    int sum = 0;
    for (int j = 0; j < SMOOTHING_WINDOW; j++) {
      sum += flexHistory[i][j];
    }
    flexValues[i] = sum / SMOOTHING_WINDOW;
  }
  
  historyIndex = (historyIndex + 1) % SMOOTHING_WINDOW;
  
  // Read MPU6050
  readMPU6050();
}

void readMPU6050() {
  // Read accelerometer and gyroscope data
  int16_t ax, ay, az, gx, gy, gz;
  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
  
  // Convert to meaningful values (adjust scaling as needed)
  accel[0] = ax / 16384.0;  // ±2g range
  accel[1] = ay / 16384.0;
  accel[2] = az / 16384.0;
  
  gyro[0] = gx / 131.0;     // ±250°/s range
  gyro[1] = gy / 131.0;
  gyro[2] = gz / 131.0;
}

int recognizeGesture() {
  /*
   * Gesture Mapping Logic:
   * Define your specific gesture patterns here based on:
   * - Flex sensor bending patterns
   * - Hand orientation from MPU6050
   * - Movement patterns from gyroscope
   */
  
  // Calculate finger bending states (0=straight, 1=bent)
  int fingerState[NUM_FLEX_SENSORS];
  for (int i = 0; i < NUM_FLEX_SENSORS; i++) {
    // Adjust threshold based on your calibration
    int threshold = 50;  // Change this based on testing
    fingerState[i] = (abs(flexValues[i] - flexBaseline[i]) > threshold) ? 1 : 0;
  }
  
  // Get hand orientation
  float pitch = atan2(-accel[0], sqrt(accel[1]*accel[1] + accel[2]*accel[2])) * 180/PI;
  float roll = atan2(accel[1], accel[2]) * 180/PI;
  
  // Example gesture definitions - ADJUST THESE BASED ON YOUR ACTUAL GESTURES
  
  // Gesture 0: "Hello" - Open hand wave
  if (fingerState[0] == 0 && fingerState[1] == 0 && fingerState[2] == 0 && 
      fingerState[3] == 0 && fingerState[4] == 0 && 
      abs(gyro[1]) > 100) {  // Sideways movement
    return 0;
  }
  
  // Gesture 1: "Yes" - Thumb up
  if (fingerState[0] == 0 && fingerState[1] == 1 && fingerState[2] == 1 && 
      fingerState[3] == 1 && fingerState[4] == 1 && 
      pitch > 45) {  // Hand tilted up
    return 1;
  }
  
  // Gesture 2: "No" - Index finger wag
  if (fingerState[0] == 1 && fingerState[1] == 0 && fingerState[2] == 1 && 
      fingerState[3] == 1 && fingerState[4] == 1 && 
      abs(gyro[0]) > 100) {  // Side-to-side movement
    return 2;
  }
  
  // Gesture 3: "Thank you" - Flat hand over heart
  if (fingerState[0] == 0 && fingerState[1] == 0 && fingerState[2] == 0 && 
      fingerState[3] == 0 && fingerState[4] == 0 && 
      roll > 45 && accel[2] < -0.5) {  // Hand on chest
    return 3;
  }
  
  // Gesture 4: "Please" - Cupped hand
  if (fingerState[0] == 1 && fingerState[1] == 1 && fingerState[2] == 1 && 
      fingerState[3] == 1 && fingerState[4] == 1 && 
      pitch < -30) {  // Hand facing up
    return 4;
  }
  
  // Gesture 5: "Help" - Waving with all fingers
  if (fingerState[0] == 0 && fingerState[1] == 0 && fingerState[2] == 0 && 
      fingerState[3] == 0 && fingerState[4] == 0 && 
      abs(gyro[1]) > 150 && abs(accel[0]) > 0.8) {
    return 5;
  }
  
  // Gesture 6: "I'm fine" - OK sign (thumb and index)
  if (fingerState[0] == 0 && fingerState[1] == 0 && fingerState[2] == 1 && 
      fingerState[3] == 1 && fingerState[4] == 1) {
    return 6;
  }
  
  // Gesture 7: "Good morning" - Salute
  if (fingerState[0] == 0 && fingerState[1] == 0 && fingerState[2] == 0 && 
      fingerState[3] == 1 && fingerState[4] == 1 && 
      pitch > 60) {
    return 7;
  }
  
  // Gesture 8: "Sorry" - Hand on chest circular
  if (fingerState[0] == 0 && fingerState[1] == 0 && fingerState[2] == 0 && 
      fingerState[3] == 0 && fingerState[4] == 0 && 
      abs(gyro[2]) > 100 && roll > 30) {
    return 8;
  }
  
  // Gesture 9: "Goodbye" - Wave with fingers together
  if (fingerState[0] == 1 && fingerState[1] == 0 && fingerState[2] == 0 && 
      fingerState[3] == 0 && fingerState[4] == 1 && 
      abs(gyro[1]) > 120) {
    return 9;
  }
  
  return -1;  // No gesture recognized
}

void displayGesture(int gestureIndex) {
  String word = GESTURE_WORDS[gestureIndex];
  
  Serial.print("Gesture Detected: ");
  Serial.println(word);
  
  // Display on LCD
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Detected:");
  lcd.setCursor(0, 1);
  
  // Handle long words by scrolling if necessary
  if (word.length() > 16) {
    lcd.print(word.substring(0, 16));
    // Add scrolling logic here if needed
  } else {
    // Center shorter text
    int spaces = (16 - word.length()) / 2;
    for (int i = 0; i < spaces; i++) {
      lcd.print(" ");
    }
    lcd.print(word);
  }
}

/*
 * Voice Output Function (Optional)
 * Uncomment and modify when you connect a voice module
 */
/*
void speakGesture(int gestureIndex) {
  // This function would send signals to a voice synthesis module
  // Example for DFPlayer Mini or similar module
  
  switch(gestureIndex) {
    case 0:  // Hello
      // Send command for track 1
      break;
    case 1:  // Yes
      // Send command for track 2
      break;
    // ... etc for all gestures
  }
  
  // Example simple tone output
  tone(VOICE_PIN, 1000, 200);  // Beep to indicate output
}
*/

/*
 * Debug Function - Uncomment to see sensor values in Serial Monitor
 */
/*
void printSensorData() {
  Serial.print("Flex: ");
  for (int i = 0; i < NUM_FLEX_SENSORS; i++) {
    Serial.print(flexValues[i]);
    Serial.print(" (");
    Serial.print(flexValues[i] - flexBaseline[i]);
    Serial.print(") ");
  }
  
  Serial.print(" | Accel: ");
  Serial.print(accel[0]); Serial.print(", ");
  Serial.print(accel[1]); Serial.print(", ");
  Serial.print(accel[2]);
  
  Serial.print(" | Gyro: ");
  Serial.print(gyro[0]); Serial.print(", ");
  Serial.print(gyro[1]); Serial.print(", ");
  Serial.print(gyro[2]);
  
  Serial.println();
}
*/
