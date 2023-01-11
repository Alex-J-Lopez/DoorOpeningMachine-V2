#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <LiquidCrystal.h>

const char* ssid = "Room308";
const char* password = "notroom308";

//Pin Declaration
const int dirPin = 16; //Pin for setting direction
const int stepPin = 5; //Pin for sending single step signal

//LCD object instance and pins
LiquidCrystal lcd(1, 3, 2, 13, 12, 14);

WiFiServer Server(80);

void setup() {
  Serial.begin(9600);

  pinMode(dirPin, OUTPUT);
  pinMode(stepPin, OUTPUT);

  lcd.begin(16, 2);
  lcd.print("Start Up");
  Serial.print("Start UP");  

  //Set up AP
  WiFi.softAP(ssid, password);
  lcd.setCursor(0, 1);
  lcd.print("AP: On");
  Serial.print("Station mode enabled and wifi ssid broadcasting");
  lcd.print(", Online");

  lcd.setCursor(0, 0);
  Server.begin();
}

void OpenDoor(){
  lcd.clear();
  lcd.print("Opening");
  digitalWrite(dirPin, 1);
  for(int i = 0; i<169; i++){
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(1000);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(1000);
  }
  lcd.clear();
  lcd.print("Open for 3 seconds.");
  delay(3000);
}

void CloseDoor(){
  lcd.clear();
  lcd.print("Closing");
  digitalWrite(dirPin, 0);
  for(int i = 0; i<169; i++){
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(1000);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(1000);
  }
  lcd.clear();
  lcd.print("Closed for 3 seconds");
  delay(3000);
}
  
void loop() {
  OpenDoor();
  CloseDoor();
}
