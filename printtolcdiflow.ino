#include <ESP8266WiFi.h>
#include <FirebaseESP8266.h>
#include <Wire.h> // Include the Wire.h library for I2C communication
#include <LiquidCrystal_I2C.h>
#include <Servo.h> // Include the Servo library for servo motor control

#define WIFI_SSID "POCO X3"
#define WIFI_PASSWORD "skadyoosh"
#define FIREBASE_HOST "rit24safaai-default-rtdb.asia-southeast1.firebasedatabase.app"
#define FIREBASE_AUTH "oTNDqOODORkq8tcz2T3OgtrQt66xqyZVKE4bB6iu"
#define IR_PIN D5  // Pin connected to the IR object detector
#define TRANSPARENT_SENSOR_PIN A0 // Pin connected to the transparent object sensor
#define LCD_ADDRESS 0x27  // I2C address of the LCD module
#define LCD_COLUMNS 16
#define LCD_ROWS 2
#define SERVO_PIN D6 // Pin connected to the servo motor

FirebaseData firebaseData;
LiquidCrystal_I2C lcd(LCD_ADDRESS, LCD_COLUMNS, LCD_ROWS);
Servo servoMotor; // Create a servo object
String codes[10]; // Assuming you have 10 codes

void setup() {
  Serial.begin(9600);
  delay(1000);  // Delay to let the serial console initialize
  
  pinMode(IR_PIN, INPUT); // Set IR pin as input
  
  lcd.init();  // Initialize the LCD
  lcd.backlight();  // Turn on the backlight
  
  servoMotor.attach(SERVO_PIN); // Attach the servo motor to the pin
  servoMotor.write(0); // Set the initial position of the servo motor to 0 degrees

  // Connect to WiFi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to ");
  Serial.println(WIFI_SSID);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi.");

  // Connect to Firebase
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);
  Serial.println("Connected to Firebase.");
  
  // Read codes from Firebase
  readCodes();
}


void loop() { 
 // Serial.println(analogRead(TRANSPARENT_SENSOR_PIN));
  delay(3000);
  // Check if the signal from IR object detector is low and analog value is less than 11
  if (digitalRead(IR_PIN) == LOW && analogRead(TRANSPARENT_SENSOR_PIN) < 11) {
    // Move the servo motor to 180 degrees
    servoMotor.write(180);
    delay(2000);
     servoMotor.write(0);

    // Read the random code from the array
    String randomCode = getRandomCode();
    Serial.println(randomCode);
    // Display the random code on the LCD
    lcd.clear();  // Clear the LCD display
    lcd.setCursor(0, 0);  // Set the cursor to the first row
    lcd.print("The Unique Code:");
    lcd.setCursor(0, 1);  // Set the cursor to the second row
    lcd.print(randomCode);  // Print the random code on the LCD

    
    
    delay(7000); // Wait for 7 seconds

    lcd.clear();  // Clear the LCD display
  } else if (analogRead(TRANSPARENT_SENSOR_PIN) > 11 && digitalRead(IR_PIN) == LOW) {
    // If object is not transparent, print "Not tranparent "
    lcd.clear();  // Clear the LCD display
    lcd.setCursor(0, 0);  // Set the cursor to the first row
    lcd.print("Not plastic");
    lcd.setCursor(0,1);
    lcd.print("bottle");
    delay(1000); // Wait for 1 second
    lcd.clear();
  }
}

void readCodes() {
  // Read codes from Firebase
  for (int i = 0; i < 10; i++) {
    String codePath = "codesSet/code" + String(i + 1); // Construct path to each code
    FirebaseData fbdo;                                 // Create FirebaseData object
    if (Firebase.getString(fbdo, codePath)) {         // Read code value from Firebase
      codes[i] = fbdo.to<String>();                   // Store code value into array
    } else {
      Serial.println("Error reading code " + String(i + 1));
    }
  }
}

String getRandomCode() {
  // Generate a random index
  int randomIndex = random(0, 10);
  return codes[randomIndex];
}
