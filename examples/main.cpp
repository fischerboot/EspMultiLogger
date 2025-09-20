/*********
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/vs-code-platformio-ide-esp32-esp8266-arduino/
*********/

#include <Arduino.h>
#include <EspMultiLogger.h>
/*
Configuration
*/
#define LED 2

#ifndef WlanConfig_h
#define STASSID "------------"
#define STAPSK  "------------"
#endif
const char* ssid = STASSID;
const char* password = STAPSK;

EspMultiLogger* InfoLogger;
EspMultiLogger* DebugLogger;
bool on = true;
void setup() {
  pinMode(LED, OUTPUT);
  // W-Lan Activating
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }
  
  InfoLogger = new EspMultiLogger(Info);
  DebugLogger = new EspMultiLogger(Debug);
  EspMultiLogger::setLogLevel(Debug);
  EspMultiLogger::initLogger();
}

void loop() {
  // Logger need to be looped
  EspMultiLogger::loopLogger();
  // put your main code here, to run repeatedly:
  if(millis()%1000==0){
    if(on == true){
      on = false;
      digitalWrite(LED, HIGH);
      InfoLogger->println("LED is on");
    }else{
      on = true;
      digitalWrite(LED, LOW);
      InfoLogger->println("LED is off");
    }
  }
}