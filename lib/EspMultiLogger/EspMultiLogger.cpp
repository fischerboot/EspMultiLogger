// inspired from https://github.com/atomic14/ESP32RemoteLogging
#include <Arduino.h>
#include <EspMultiLogger.h>
#include "version.h"


#define LoggingWithTimeout

#ifdef LoggingWithTimeout
#define logTimeout (1800) // 60 min * 60sec = 1 std
#endif 

//#define MAX_TELNET_CLIENTS 4

const uint16_t TelnetPort = 23;
uint8_t i;
bool ConnectionEstablished; // Flag for successfully handled connection
WiFiServer TelnetServer(TelnetPort);
WiFiClient TelnetClient[MAX_TELNET_CLIENTS];

EspMultiLogger::EspMultiLogger(LogLevel level) {
  mLevel = level;
  mBufferPos = 0;
}

size_t EspMultiLogger::write(uint8_t c) {
  // only print if the instance loglevel is lower or equal than the overall loglevel.
  if(mLevel<=AllLevel){
  //  if(true){
  // check to see if we've reached the end of our buffer or received a newline character
    if (c == '\n' ) {
      // add a null terminating byte to the buffer
      mBuffer[mBufferPos] = 0;
      // send line to logger
      Serial.println((const char *) mBuffer);
      // send line to telnet
      for(i = 0; i < MAX_TELNET_CLIENTS; i++)
      {
        if (TelnetClient[i] || TelnetClient[i].connected())
        {
          TelnetClient[i].println((const char *)mBuffer);
        }
      }
    delay(10);  // to avoid strange characters left in buffer
      mBufferPos = 0;
    }else if(mBufferPos == BUFFER_SIZE - 1){
      // add a null terminating byte to the buffer
      mBuffer[mBufferPos] = 0;
      // send the message to logger
      Serial.print((const char *) mBuffer);
      //send message to telnet
      for(i = 0; i < MAX_TELNET_CLIENTS; i++)
      {
        if (TelnetClient[i] || TelnetClient[i].connected())
        {
          TelnetClient[i].print((const char *)mBuffer);
        }
      }
      mBufferPos = 0;
    } 
    else {
      // buffer the character up for sending later
      mBuffer[mBufferPos] = c;
      mBufferPos++;
    }
  }
  return 1;
}

void EspMultiLogger::setLogLevel(LogLevel level){
    AllLevel = level;
}

void EspMultiLogger::initLogger(){
  // todo generic implementation for init
  // tood check if Serial was already initialized
  //Serial.begin(115200);

  TelnetServer.begin();
  TelnetServer.setNoDelay(true);
}

void EspMultiLogger::setUserVersionString(const char* version) {
    userVersionString = version;
}

void EspMultiLogger::loopLogger(){
  // todo generic implementation for loop
   // Cleanup disconnected session
  for(i = 0; i < MAX_TELNET_CLIENTS; i++)
  {
    if (TelnetClient[i] && !TelnetClient[i].connected())
    {
      Serial.print("Client disconnected ... terminate session "); Serial.println(i+1); 
      TelnetClient[i].stop();
    }
  }
  
  // Check new client connections
  if (TelnetServer.hasClient())
  {
    ConnectionEstablished = false; // Set to false
    
    for(i = 0; i < MAX_TELNET_CLIENTS; i++)
    {
      // Serial.print("Checking telnet session "); Serial.println(i+1);
      
      // find free socket
      if (!TelnetClient[i])
      {
        TelnetClient[i] = TelnetServer.available(); 
        
        Serial.print("New Telnet client connected to session "); Serial.println(i+1);
        
        TelnetClient[i].flush();  // clear input buffer, else you get strange characters
        TelnetClient[i].println("Welcome!");
        
        TelnetClient[i].print("Millis since start: ");
        TelnetClient[i].println(millis());
        
        TelnetClient[i].print("Free Heap RAM: ");
        TelnetClient[i].println(ESP.getFreeHeap());

        TelnetClient[i].print("ESPMultiLogger Version: ");
        TelnetClient[i].println(ESPMULTILOGGER_VERSION);

        if (userVersionString) { // Print user version string if set
          TelnetClient[i].print("User Version: ");
          TelnetClient[i].println(userVersionString);
        }

        TelnetClient[i].println("----------------------------------------------------------------");
        
        ConnectionEstablished = true; 
        
        break;
      }
      else
      {
        // Serial.println("Session is in use");
      }
    }

    if (ConnectionEstablished == false)
    {
      Serial.println("No free sessions ... drop connection");
      TelnetServer.available().stop();
      // TelnetMsg("An other user cannot connect ... MAX_TELNET_CLIENTS limit is reached!");
    }
  }

  for(i = 0; i < MAX_TELNET_CLIENTS; i++)
  {
    if (TelnetClient[i] && TelnetClient[i].connected())
    {
      if(TelnetClient[i].available())
      { 
        //get data from the telnet client
        while(TelnetClient[i].available())
        {
          // todo add remote loglevel parser 
          // uncommenting this command result in a crash!
          Serial.write(TelnetClient[i].read());
        }
      }
    }
  }
}