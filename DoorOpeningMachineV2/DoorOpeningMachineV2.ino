#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <LiquidCrystal.h>

const char* ssid = "Room308";
const char* password = "notroom308";

const int stepsToLatch = 169;
boolean isOpen = false;

//Pin Declaration
const int dirPin = 16; //Pin for setting direction
const int stepPin = 5; //Pin for sending single step signal

//LCD object instance and pins
LiquidCrystal lcd(1, 3, 2, 13, 12, 14);

WiFiServer Server(80);

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
}

void doorCycleOneOnClick(){
  server.send(200, "text/html", root); //Send root page to the client.
  lcd.clear(); lcd.print("Opening Door"); //Display that the door is opening on the LCD Screen.
  if(!isOpen){                            //Check if the door is already open.
    openDoor();                            //Run the openDoor() method to open the door. 
  }
  lcd.clear(); lcd.print("Open");         //Display that the door is open on the LCD Screen.
  delay(5000);                         //Hold door open for 5 seconds.
  lcd.clear(); lcd.print("Closing Door"); //Display that the door is closing.
  if(isOpen){                           //Check if the door is open.    
    closeDoor();                        //Run the closeDoor() method to close the door.
  }
  lcd.clear(); lcd.print("Closed");     //Display that the door is closed.
  delay(1000);                            //Wait 1 second to allow for previous message to be read.
  lcd.clear(); lcd.print("Idle");       //Display that the machine is idle on the LCD Screen. 
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
  Serial.begin(9600);

  //Set up step and directions pins to output mode.
  pinMode(dirPin, OUTPUT);
  pinMode(stepPin, OUTPUT);

  lcd.begin(16, 2);
  lcd.print("Start Up");

  //Set up AP
  WiFi.softAP(ssid, password);
  lcd.setCursor(0, 1);
  lcd.print("AP: On");
  lcd.print(", Online");
  lcd.setCursor(0, 0);

  server.on("/", handleRoot);
  server.on("/doorCycle", doorCycleOnClick);
  server.on("/holdOpen", doorHoldOpenOnClick);
  Server.begin();
}
  
void loop() {
  server.handleClient();
}
