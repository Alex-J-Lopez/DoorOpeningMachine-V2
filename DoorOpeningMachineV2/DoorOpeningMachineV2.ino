#include <WiFi.h>
#include <WebServer.h>
#include <WiFiClient.h>
#include <ESPmDNS.h>
#include <LiquidCrystal.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include "index.h"

const char* ssid = "VTOW-Res308-2.4ghz";
const char* password = "relentless49rpc45";


const String root = Main_Page;
const int stepsToLatch = 241;
boolean isOpen = false;

//Pin Declaration
const int dirPin = 23; //Pin for setting direction
const int stepPin = 22; //Pin for sending single step signal

//LCD object instance and pins
LiquidCrystal lcd(15, 2, 21, 16, 17, 5);

//Create web server instance
WebServer server(80);

String sendDoorStatus(){
  String status;
  if(!isOpen){
    status = "Open";
  } else if (isOpen){
    status = "Closed";
  }
  return status;
}

void cycleDoor(int direction){
  digitalWrite(dirPin, direction);
  for(int i = 0; i<stepsToLatch; i++){
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(1000);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(1000);
  }
  if(direction == 1){
    isOpen = true;
  } else if(direction == 0){
    isOpen = false;
  }
}

void handleRoot(){
  server.send(200, "text/html", root);
  lcd.clear(); lcd.print("R308.local");
  lcd.setCursor(0, 1); lcd.print(WiFi.localIP());
}

void doorCycleOnClick(){
  server.send(200, "text/html", root);
  lcd.clear(); lcd.print("Opening Door"); //Display that the door is opening on the LCD Screen.
  if(!isOpen){                            //Check if the door is already open.
    cycleDoor(1);                            //Run the openDoor() method to open the door.
    delay(5000);                         //Hold door open for 5 seconds.
  }
  lcd.clear(); lcd.print("Open");         //Display that the door is open on the LCD Screen.
  lcd.clear(); lcd.print("Closing Door"); //Display that the door is closing.
  if(isOpen){                           //Check if the door is open.    
    cycleDoor(0);                        //Run the closeDoor() method to close the door.
  }
  lcd.clear(); lcd.print("Closed");     //Display that the door is closed.
  delay(1000);                            //Wait 1 second to allow for previous message to be read.
  lcd.clear(); lcd.print("R308.local");
  lcd.setCursor(0, 1); lcd.print(WiFi.localIP());
}

void doorHoldOpenOnClick(){
  server.send(200, "text/html", root + "<p style=\"text-align: center; font-size: 3vh;\">Door Status: "+sendDoorStatus()+"</p>");
  lcd.clear(); lcd.print("Opening Door"); //Display that the door is opening.
  if(!isOpen){                            //Check if the door is alreay open.
    cycleDoor(1);                           //Run the openDoor() method to open the door.
    lcd.clear(); lcd.print("Door Open");
    lcd.setCursor(0,1); lcd.print("Holding");
  } else if (isOpen){
    lcd.clear(); lcd.print("Closing Door");
    cycleDoor(0);
    lcd.clear(); lcd.print("Closed");     //Display that the door is closed.
    delay(1000);                            //Wait 1 second to allow for previous message to be read.
    lcd.clear(); lcd.print("R308.local");
    lcd.setCursor(0, 1); lcd.print(WiFi.localIP());
  }
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

  /*
  New OTA feature  
  */
  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else {  // U_FS
      type = "filesystem";
    }

    // NOTE: if updating FS this would be the place to unmount FS using FS.end()
    Serial.println("Start updating " + type);
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) {
      Serial.println("Auth Failed");
    } else if (error == OTA_BEGIN_ERROR) {
      Serial.println("Begin Failed");
    } else if (error == OTA_CONNECT_ERROR) {
      Serial.println("Connect Failed");
    } else if (error == OTA_RECEIVE_ERROR) {
      Serial.println("Receive Failed");
    } else if (error == OTA_END_ERROR) {
      Serial.println("End Failed");
    }
  });
  ArduinoOTA.begin();
  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  /*
  End of OTA Feature.
  */


  MDNS.begin("R308");

  server.on("/", handleRoot);
  server.on("/doorCycle", doorCycleOnClick);
  server.on("/holdOpen", doorHoldOpenOnClick);
  server.begin();
  lcd.clear(); lcd.print("R308.local");
  lcd.setCursor(0, 1); lcd.print(WiFi.localIP());
}
  
void loop() {  
  ArduinoOTA.handle();
  server.handleClient();
}
