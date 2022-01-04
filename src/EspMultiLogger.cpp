// inspired from https://github.com/atomic14/ESP32RemoteLogging

#include "EspMultiLogger.h"

EspMultiLogger::EspMultiLogger(LogLevel level) {
  mLevel = level;
  mBufferPos = 0;
}

size_t EspMultiLogger::write(uint8_t c) {
  // check to see if we've reached the end of our buffer or received a newline character
  if (c == '\n' || mBufferPos == BUFFER_SIZE - 1) {
    // add a null terminating byte to the buffer
    mBuffer[mBufferPos] = 0;
    // send the message to the Serial port
    Serial.println((const char *) mBuffer);
    mBufferPos = 0;
  } else {
    // buffer the character up for sending later
    mBuffer[mBufferPos] = c;
    mBufferPos++;
  }
  return 1;
}

void setLogLevel(LogLevel){
    mLevel = level;
}