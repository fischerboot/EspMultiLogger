// inspired from https://github.com/atomic14/ESP32RemoteLogging
#include <Arduino.h>
#include <EspMultiLogger.h>
#include "version.h"
#include <time.h>


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

TelnetWelcomeCallback EspMultiLogger::welcomeCallback = nullptr;

EspMultiLogger::EspMultiLogger(LogLevel level) {
  mLevel = level;
  mBufferPos = 0;
  userVersionString[0] = 0; // Define and initialize
}

// Remove these lines (they are already defined as static inline in the header):
// char EspMultiLogger::logCache[LOG_CACHE_SIZE][LOG_MESSAGE_MAX_LENGTH];
// int EspMultiLogger::logCacheIndex = 0;
// int EspMultiLogger::logCacheCount = 0;

// Keep only the userVersionString definition if it's not inline:

void EspMultiLogger::addToLogCache(const char* message) {
    // Copy message to cache (circular buffer)
    strncpy(logCache[logCacheIndex], message, LOG_MESSAGE_MAX_LENGTH - 1);
    logCache[logCacheIndex][LOG_MESSAGE_MAX_LENGTH - 1] = '\0';
    
    logCacheIndex = (logCacheIndex + 1) % LOG_CACHE_SIZE;
    if (logCacheCount < LOG_CACHE_SIZE) {
        logCacheCount++;
    }
}

void EspMultiLogger::printLogCache(WiFiClient& client) {
    client.print("=== Last "); client.print(logCacheCount); client.print(" Log Messages ===\r\n");
    
    int startIndex;
    if (logCacheCount < LOG_CACHE_SIZE) {
        startIndex = 0;
    } else {
        startIndex = logCacheIndex;
    }
    
    for (int i = 0; i < logCacheCount; i++) {
        int index = (startIndex + i) % LOG_CACHE_SIZE;
        client.print(logCache[index]);
    }
    client.print("==============================\r\n");
}

size_t EspMultiLogger::write(uint8_t c) {
  if(mLevel<=AllLevel){
    if (c == '\n' || mBufferPos >= BUFFER_SIZE - 1) {
      mBuffer[mBufferPos] = '\0';
      
      // Create timestamp (try NTP time first, fallback to millis)
      char timestampedMessage[LOG_MESSAGE_MAX_LENGTH];
      time_t now = time(nullptr);
      
      if (now > 8 * 3600 * 2) { // Valid NTP time available
        struct tm* timeinfo = localtime(&now);
        snprintf(timestampedMessage, sizeof(timestampedMessage), 
                "[%04d-%02d-%02d %02d:%02d:%02d] %s\r\n",
                timeinfo->tm_year + 1900, timeinfo->tm_mon + 1, timeinfo->tm_mday,
                timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec,
                (char*)mBuffer);
      } else { // Fallback to millis-based timestamp
        unsigned long ms = millis();
        unsigned long seconds = ms / 1000;
        unsigned long minutes = seconds / 60;
        unsigned long hours = minutes / 60;
        
        snprintf(timestampedMessage, sizeof(timestampedMessage), 
                "[%02lu:%02lu:%02lu] %s\r\n", 
                hours % 24, minutes % 60, seconds % 60, 
                (char*)mBuffer);
      }
      
      // Add to cache
      addToLogCache(timestampedMessage);
      
      // Send to Serial
      if (c == '\n') {
        Serial.println((const char *) mBuffer);
      } else {
        Serial.print((const char *) mBuffer);
      }
      
      // Send to Telnet clients
      for(i = 0; i < MAX_TELNET_CLIENTS; i++) {
        if (TelnetClient[i] && TelnetClient[i].connected()) {
          if (c == '\n') {
            TelnetClient[i].println((const char *)mBuffer);
          } else {
            TelnetClient[i].print((const char *)mBuffer);
          }
        }
      }
      
      if (c == '\n') {
        delay(10);
      }
      
      mBufferPos = 0;
    } else {
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
    if (version) {
        strncpy(userVersionString, version, sizeof(userVersionString) - 1);
        userVersionString[sizeof(userVersionString) - 1] = '\0';
    } else {
        userVersionString[0] = '\0';
    }
}

void EspMultiLogger::setTelnetWelcomeCallback(TelnetWelcomeCallback cb) {
    welcomeCallback = cb;
}

void EspMultiLogger::loopLogger(){
  // Cleanup disconnected sessions
  for(i = 0; i < MAX_TELNET_CLIENTS; i++)
  {
    if (TelnetClient[i] && !TelnetClient[i].connected())
    {
      Serial.print("Client disconnected ... terminate session "); Serial.println(i+1); 
      TelnetClient[i].stop();
      TelnetClient[i] = WiFiClient(); // Reset the client object to invalid state
    }
  }
  
  // Check new client connections
  if (TelnetServer.hasClient()) {
    for(i = 0; i < MAX_TELNET_CLIENTS; i++) {
      if (!TelnetClient[i] || !TelnetClient[i].connected()) { // Check both conditions
        // Fix deprecated warning:
        TelnetClient[i] = TelnetServer.accept(); // Changed from available() to accept()
        
        Serial.print("New Telnet client connected to session ");
        Serial.println(i+1);
        
        // Print welcome message first
        if (welcomeCallback) {
          welcomeCallback(TelnetClient[i]);
        }
        
        // Then print log cache
        printLogCache(TelnetClient[i]);
        
        ConnectionEstablished = true;
        break;
      }
    }
  }
}