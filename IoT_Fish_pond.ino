#include "LCD.h"                // Including header file for LCD functions
#include "ultrasonicSensor.h"   // Including header file for ultrasonic sensor functions
#include "Blynk.h"              // Including header file for Blynk functions
#include "ThingSpeak.h"         // Including header file for ThingSpeak functions
#include "pHSensor.h"           // Including header file for pH sensor functions
#include "twilio.h"             // Including header file for Twilio functions
#include "DOSensor.h"           // Including header file for dissolved oxygen sensor functions
#include <OneWire.h>            // Including library for OneWire communication protocol
#include <base64.h>             // Including library for Base64 encoding/decoding
#include <DallasTemperature.h>  // Including library for DallasTemperature sensor functions
#include <TimeLib.h>            // Including library for time-related functions

#define ONE_WIRE_BUS 27  // Defining the digital pin for OneWire bus
#define Irsensor 14      // Defining the digital pin for IR sensor
#define motor 25         // Defining the digital pin for motor
#define WaterMotor 19    // Defining the digital pin for water motor
#define feedMotor 17     // Defining the digital pin for feed motor

OneWire oneWire(ONE_WIRE_BUS);        // Instantiating OneWire bus object
DallasTemperature sensors(&oneWire);  // Instantiating DallasTemperature sensor object with OneWire bus
WiFiClient client;                    // Creating a WiFi client object for network communication

unsigned long myChannelNumber = "YOUR_CHANNEL_NUMBER";         // ThingSpeak channel number
const char *myWriteAPIKey = "YOUR_THINGSPEAK_WRITE_API_KEY";  // ThingSpeak write API key

const char *account_sid = "YOUR_TWILIO_ACCOUNT_SID";            // Twilio account SID
const char *auth_token = "YOUR_TWILIO_AUTH_TOKEN";              // Twilio authentication token
const char *messaging_service_sid = "YOUR_TWILIO_MESSAGING_SERVICE_SID";  // Twilio messaging service SID
const char *to_number = "RECIPIENT_PHONE_NUMBER";               // Recipient's phone number


struct SensorData {
  int UltrasonicData;  // Structure member for ultrasonic sensor data
  float pHValue;       // Structure member for pH sensor data
  int IrSensor;        // Structure member for IR sensor data
  float DOsensor;      // Structure member for dissolved oxygen sensor data
  float DS18B;         // Structure member for temperature sensor data
} Sensor;              // Struct variable to hold sensor readings

bool Enable = true;                       // Flag to indicate if system is in manual or automatic mode
bool motorState = false;                  // Flag to indicate motor state
bool watermotorState = false;             // Flag to indicate water motor state
bool feedMotorState = false;              // Flag to indicate feed motor state
unsigned long lastManualModeCommand = 0;  // Timestamp of the last manual mode command
unsigned long alertInterval = 180000;     // Interval between alerts
unsigned long ResetInterval = 60000;      // Interval between alerts
int currentHour;                          // Variable to hold current hour of the day

bool ultrasonicLowAlertSent = false;
bool ultrasonicHighAlertSent = false;
bool pHLowAlertSent = false;
bool doLowAlertSent = false;
bool tempLowAlertSent = false;
bool OxygenmotorMessageSent = false;
bool FeedermotorMessageSent = false;
bool waterMotorMessageSent = false;
bool ManualAlertSent = false;
bool AutoAlertSent = false;
unsigned long lastAlertTime = 0;  // Timestamp of the last alert

BLYNK_WRITE(V0) {
  int button = param.asInt();              // Read the value sent by the virtual pin
  if (button == 1) { motorState = true; }  // Set motor state to ON if button is pressed
  else {
    motorState = false;
  }  // Set motor state to OFF if button is released
}

BLYNK_WRITE(V1) {
  int button = param.asInt();                   // Read the value sent by the virtual pin
  if (button == 1) { watermotorState = true; }  // Set water motor state to ON if button is pressed
  else {
    watermotorState = false;
  }  // Set water motor state to OFF if button is released
}

BLYNK_WRITE(V2) {
  int button = param.asInt();  // Read the value sent by the virtual pin
  if (button == 1) {
    Enable = false;
    lastManualModeCommand = millis();
  }                        // Set manual mode if button is pressed
  else { Enable = true; }  // Set automatic mode if button is released
}

BLYNK_WRITE(V3) {
  int button = param.asInt();                  // Read the value sent by the virtual pin
  if (button == 1) { feedMotorState = true; }  // Set feed motor state to ON if button is pressed
  else {
    feedMotorState = false;
  }  // Set feed motor state to OFF if button is released
}

void SensorsReading() {
  Sensor.UltrasonicData = ultrasonicSensorReading();  // Read ultrasonic sensor data
  Sensor.pHValue = pHvalue();                         // Read pH sensor data
  Sensor.DOsensor = DOSensor();                       // Read dissolved oxygen sensor data
  sensors.requestTemperatures();                      // Request temperature readings
  Sensor.DS18B = sensors.getTempCByIndex(0);          // Read temperature sensor data
  Sensor.IrSensor = digitalRead(Irsensor);            // Read IR sensor data
  currentHour = hour();                               // Update current hour
}

void updateThingSpeak() {
  ThingSpeak.setField(1, Sensor.UltrasonicData);
  ThingSpeak.setField(2, Sensor.pHValue);
  ThingSpeak.setField(3, Sensor.DS18B);
  ThingSpeak.setField(4, Sensor.IrSensor);
  ThingSpeak.setField(5, Sensor.DOsensor);
  ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
}

void upDateLCD() {
  page_1(Sensor.UltrasonicData);  // Display ultrasonic sensor reading
  delay(1500);
  page_2(Sensor.pHValue);  // Display pH sensor reading
  delay(1500);
  page_3(Sensor.DOsensor);  // Display dissolved oxygen sensor reading
  delay(1500);
  page_4(Sensor.DS18B);  // Display temperature sensor reading
  delay(1500);
  page_5(Sensor.IrSensor);  // Display IR sensor reading
  delay(1500);
}

void Manual_Auto_mode() {
  if (Enable == false)  // If manual mode is enabled
  {
    if (motorState == true) { digitalWrite(motor, HIGH); }  // Turn on motor if motor state is ON
    else {
      digitalWrite(motor, LOW);
    }  // Turn off motor if motor state is OFF

    if (watermotorState == true) { digitalWrite(WaterMotor, HIGH); }  // Turn on water motor if water motor state is ON
    else {
      digitalWrite(WaterMotor, LOW);
    }  // Turn off water motor if water motor state is OFF

    if (feedMotorState == true) { digitalWrite(feedMotor, HIGH); }  // Turn on feed motor if feed motor state is ON
    else {
      digitalWrite(feedMotor, LOW);
    }     // Turn off feed motor if feed motor state is OFF
  } else  // If automatic mode is enabled
  {
    if (Sensor.UltrasonicData > 8) { digitalWrite(WaterMotor, HIGH); }  // Turn on water motor if ultrasonic sensor reading is high
    else {
      digitalWrite(WaterMotor, LOW);
    }  // Turn off water motor if ultrasonic sensor reading is low

    if (Sensor.DOsensor < 14) { digitalWrite(motor, HIGH); }  // Turn on motor if dissolved oxygen sensor reading is low
    else {
      digitalWrite(motor, LOW);
    }  // Turn off motor if dissolved oxygen sensor reading is high

    if (currentHour == 6 || currentHour == 16) {  // If it's feeding time (6 AM or 4 PM)
      // Turn on feeder motor
      digitalWrite(feedMotor, HIGH);
      // Wait for feeding duration (adjust as needed)
      delay(3600000);  // 1 hour (adjust as needed)
      // Turn off feeder motor
      digitalWrite(feedMotor, LOW);
    }
  }
}

void BDisp() {
  if (Enable == false) {  // If manual mode is enabled
    lcd.clear();
    lcd.setCursor(2, 0);
    lcd.print("Manual Mode");

    Blynk.virtualWrite(V10, "Water: " + String(Sensor.UltrasonicData));
    delay(1500);
    Blynk.virtualWrite(V10, "pH: " + String(Sensor.pHValue));
    delay(1500);
    Blynk.virtualWrite(V10, "DO: " + String(Sensor.DOsensor));
    delay(1500);
    Blynk.virtualWrite(V10, "Temp: " + String(Sensor.DS18B));
  } else {  // If automatic mode is enabled
    lcd.clear();
    lcd.setCursor(1, 0);
    lcd.print("Automatic Mode");

    Blynk.virtualWrite(V10, LOW);  // Reset button widget on V10 pin
    Blynk.virtualWrite(V1, LOW);   // Reset button widget on V1 pin
    Blynk.virtualWrite(V0, LOW);   // Reset button widget on V0 pin
    Blynk.virtualWrite(V3, LOW);   // Reset button widget on V3 pin
    delay(1000);
  }
}

void checkManualModeTimeout() {
  if (Enable == false && (millis() - lastManualModeCommand) > 3600000) {  // If it's been more than 1 hour since last manual mode command
    Enable = true;                                                        // Switch to automatic mode
    lcd.clear();
    lcd.setCursor(1, 0);
    lcd.print("Automatic Mode");
    Blynk.virtualWrite(V10, LOW);  // Reset button widget on V10 pin
    Blynk.virtualWrite(V1, LOW);   // Reset button widget on V1 pin
    Blynk.virtualWrite(V0, LOW);   // Reset button widget on V0 pin
    Blynk.virtualWrite(V3, LOW);   // Reset button widget on V3 pin
  }
}

void NotiSensor() {
  unsigned long currentTime = millis();  // Get current time in milliseconds
  String alertMessage = "";              // Initialize an empty string for alert messages

  if (Sensor.UltrasonicData > 7 && !ultrasonicLowAlertSent) {
    alertMessage += "Water Level is low: " + String(Sensor.UltrasonicData) + "\n";
    Serial.println(alertMessage);
    ultrasonicLowAlertSent = true;
  }

  if (Sensor.pHValue < 6.5 && !pHLowAlertSent) {
    alertMessage += "Water is Acidic: " + String(Sensor.pHValue) + "\n";
    Serial.println(alertMessage);
    pHLowAlertSent = true;
  }

  if (Sensor.DOsensor < 14 && !doLowAlertSent) {
    alertMessage += "DO sensor reading is low: " + String(Sensor.DOsensor) + "\n";
    Serial.println(alertMessage);
    doLowAlertSent = true;
  }

  if (Sensor.DS18B < 24 && !tempLowAlertSent) {
    alertMessage += "Temperature sensor reading is low: " + String(Sensor.DS18B) + "\n";
    Serial.println(alertMessage);
    tempLowAlertSent = true;
  }

  if (digitalRead(motor) && !OxygenmotorMessageSent) {
    alertMessage += "Oxygen motor is on\n";
    Serial.println(alertMessage);
    OxygenmotorMessageSent = true;
  }

  if (digitalRead(feedMotor) && !FeedermotorMessageSent) {
    alertMessage += "Feed motor is on\n";
    Serial.println(alertMessage);
    FeedermotorMessageSent = true;
  }

  if (digitalRead(WaterMotor) && !waterMotorMessageSent) {
    alertMessage += "Water motor is on\n";
    Serial.println(alertMessage);
    waterMotorMessageSent = true;
  }

  if (!alertMessage.isEmpty() && (currentTime - lastAlertTime > alertInterval)) {
    c_alert(alertMessage.c_str());  // Send alert message
    Serial.println(alertMessage);
    lastAlertTime = currentTime;  // Update last alert time
  }

  if (alertMessage.isEmpty() && (currentTime - lastAlertTime > ResetInterval)) {
    ultrasonicLowAlertSent = false;
    ultrasonicHighAlertSent = false;
    pHLowAlertSent = false;
    doLowAlertSent = false;
    tempLowAlertSent = false;
    OxygenmotorMessageSent = false;
    FeedermotorMessageSent = false;
    waterMotorMessageSent = false;
    ManualAlertSent = false;
    AutoAlertSent = false;
  }
}

void c_alert(const char *message) {

  String response;

  bool success = Twilio::send_message_with_service(account_sid, auth_token, messaging_service_sid, to_number, message, response);

  if (success) {
    Serial.println("Sent message successfully!");
  } else {
    Serial.println("Failed to send message. Response: " + response);
  }
}

void setup() {
  Serial.begin(115200);     // Initialize serial communication
  lcdInit();                // Initialize LCD
  ultrasonicSensor_init();  // Initialize ultrasonic sensor
  sensors.begin();          // Begin communication with temperature sensors

  // Display welcome message on LCD
  lcd.clear();
  lcd.setCursor(3, 0);
  lcd.print("WELCOME TO");
  delay(2000);
  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.print("IoT BASED FISH");
  lcd.setCursor(0, 1);
  lcd.print("POND MONITORING ");
  delay(2000);
  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.print("AND PRODUCTION");
  lcd.setCursor(1, 1);
  lcd.print("ENHANCEMENT SYSTEM");
  delay(2000);
  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.print("By");
  lcd.setCursor(3, 1);
  lcd.print("-VIET.ECE.2k24");
  delay(2000);
  lcd.clear();
  lcd.print("Connecting WiFi");
  ConnectingBlynk();  // Connect to Blynk server
  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.print("WiFi Connected");
  delay(2000);
  lcd.clear();
  lcd.print("Connecting Blynk");
  delay(2000);
  lcd.clear();
  lcd.print("Blynk Connected");
  delay(1000);
  lcd.clear();
  Blynk.virtualWrite(V0, LOW);  // Reset button widget on V0 pin
  Blynk.virtualWrite(V1, LOW);  // Reset button widget on V1 pin
  Blynk.virtualWrite(V2, LOW);  // Reset button widget on V2 pin
  Blynk.virtualWrite(V3, LOW);  // Reset button widget on V3 pin
  Enable = true;                // Start in automatic mode
  lastManualModeCommand = 0;    // Reset last manual mode command time

  // Set pin modes for motors and sensors
  pinMode(motor, OUTPUT);
  pinMode(WaterMotor, OUTPUT);
  pinMode(feedMotor, OUTPUT);
  pinMode(Irsensor, INPUT_PULLUP);

  ThingSpeak.begin(client);  // Initialize ThingSpeak client
  // Set up timer intervals for various functions
  timer.setInterval(1000L, SensorsReading);
  timer.setInterval(2000L, upDateLCD);
  timer.setInterval(1000L, Manual_Auto_mode);
  timer.setInterval(25000L, updateThingSpeak);
  timer.setInterval(60000L, checkManualModeTimeout);  // Check for manual mode timeout every 60 seconds
}

void loop() {
  Blynk.run();  // Run Blynk
  timer.run();  // Run timer
  BDisp();      // Display mode information on LCD
  NotiSensor();// Check for sensor readings and send alerts
}
