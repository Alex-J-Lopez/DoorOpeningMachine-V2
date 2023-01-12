#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WiFiClient.h>
#include <ESP8266mDNS.h>
#include <LiquidCrystal.h>
#include "index.h"

const char* ssid = "VTOW-Res308-2.4ghz";
const char* password = "relentless49rpc45";


const String root = Main_Page;
const int stepsToLatch = 169;
boolean isOpen = false;

//Pin Declaration
const int dirPin = 16; //Pin for setting direction
const int stepPin = 5; //Pin for sending single step signal

//LCD object instance and pins
LiquidCrystal lcd(1, 3, 2, 13, 12, 14);

//Create web server instance
ESP8266WebServer server(80);

void openDoor(){
  digitalWrite(dirPin, 1);
  for(int i = 0; i<stepsToLatch; i++){
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(1000);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(1000);
  }
  isOpen = true;
}

void closeDoor(){
  digitalWrite(dirPin, 0);
  for(int i = 0; i<stepsToLatch; i++){
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(1000);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(1000);
  }
  isOpen = false;
}

void handleRoot(){
  server.send(200, "text/html", root);
  lcd.clear(); lcd.print("R308.local");
  lcd.setCursor(0, 1); lcd.print(WiFi.localIP());
}

void doorCycleOnClick(){
  server.send(200, "text/html", root); //Send root page to the client.
  lcd.clear(); lcd.print("Opening Door"); //Display that the door is opening on the LCD Screen.
  if(!isOpen){                            //Check if the door is already open.
    openDoor();                            //Run the openDoor() method to open the door.
    delay(5000);                         //Hold door open for 5 seconds.
  }
  lcd.clear(); lcd.print("Open");         //Display that the door is open on the LCD Screen.
  lcd.clear(); lcd.print("Closing Door"); //Display that the door is closing.
  if(isOpen){                           //Check if the door is open.    
    closeDoor();                        //Run the closeDoor() method to close the door.
  }
  lcd.clear(); lcd.print("Closed");     //Display that the door is closed.
  delay(1000);                            //Wait 1 second to allow for previous message to be read.
  lcd.clear(); lcd.print("R308.local");
  lcd.setCursor(0, 1); lcd.print(WiFi.localIP());
}

void doorHoldOpenOnClick(){
  server.send(200, "text/html", root);
  lcd.clear(); lcd.print("Opening Door"); //Display that the door is opening.
  if(!isOpen){                            //Check if the door is alreay open.
    openDoor();                           //Run the openDoor() method to open the door.
  }
  lcd.clear(); lcd.print("Door Open");
  lcd.setCursor(0,1); lcd.print("Holding");
}

void setup() {
  Serial.begin(115200);

  //Set up step and directions pins to output mode.
  pinMode(dirPin, OUTPUT);
  pinMode(stepPin, OUTPUT);

  lcd.begin(16, 2);
  lcd.print("Start Up");
  Serial.println("Start Up");

  //connect to wifi
  WiFi.begin(ssid, password);
  lcd.setCursor(0, 1);
  lcd.print("STA: On");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  lcd.print(", Online");
  lcd.setCursor(0, 0);
  Serial.println(WiFi.localIP());
  lcd.clear();
  lcd.print("IP: ");
  lcd.print(WiFi.localIP());

  MDNS.begin("R308", WiFi.localIP());

  server.on("/", handleRoot);
  server.on("/doorCycle", doorCycleOnClick);
  server.on("/holdOpen", doorHoldOpenOnClick);
  server.begin();
  MDNS.addService("http", "tcp", 80);
  lcd.clear(); lcd.print("R308.local");
  lcd.setCursor(0, 1); lcd.print(WiFi.localIP());
}
  
void loop() {  
  MDNS.update();
  server.handleClient();
}
