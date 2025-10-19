#ifndef __esp_multi_logger_h__
#define __esp_multi_logger_h__

// inspired from https://github.com/atomic14/ESP32RemoteLogging
#include <ESP8266WiFi.h>
enum LogLevel {
  Error = 3,
  Warning = 4,
  Notice = 5,
  Info = 6,
  Debug = 7
};

#define BUFFER_SIZE 200
#define LOG_CACHE_SIZE 10
#define LOG_MESSAGE_MAX_LENGTH 128

typedef void (*TelnetWelcomeCallback)(WiFiClient& client);

class EspMultiLogger: public Print {
  private:
    LogLevel mLevel;
    uint8_t mBuffer[BUFFER_SIZE];
    int mBufferPos;
    static inline LogLevel AllLevel;
    static inline char userVersionString[64]; // Changed: now a buffer, not a pointer
    
    // Log cache for recent messages
    static inline char logCache[LOG_CACHE_SIZE][LOG_MESSAGE_MAX_LENGTH];
    static inline int logCacheIndex;
    static inline int logCacheCount;
    
    void addToLogCache(const char* message);
    
  public:
    static TelnetWelcomeCallback welcomeCallback;
    static void initLogger();
    static void loopLogger();
    static void setLogLevel(LogLevel level);
    static void setUserVersionString(const char* version); // Unchanged
    static void setTelnetWelcomeCallback(TelnetWelcomeCallback cb);
    static void printLogCache(WiFiClient& client);
    
    EspMultiLogger(LogLevel level);
    size_t write(uint8_t c);

};

#ifndef MAX_TELNET_CLIENTS
#define MAX_TELNET_CLIENTS 2 // or your preferred default
#endif

extern WiFiServer TelnetServer;
extern WiFiClient TelnetClient[MAX_TELNET_CLIENTS];

#endif