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

class EspMultiLogger: public Print {
  private:
    LogLevel mLevel;
    uint8_t mBuffer[BUFFER_SIZE];
    int mBufferPos;
  public:

    static void initLogger();
    static void loopLogger();
    EspMultiLogger(LogLevel level);
    size_t write(uint8_t c);
    void setLogLevel(LogLevel level);
};

#endif