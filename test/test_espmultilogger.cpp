#include <Arduino.h>
#include <unity.h>
#include "EspMultiLogger.h"

#ifndef MAX_TELNET_CLIENTS
#define MAX_TELNET_CLIENTS 4
#endif

// Mock the WiFiClient and WiFiServer for the tests
class MockWiFiClient : public WiFiClient {
public:
    uint8_t connected() override { return 1; }
    size_t println(const char* message) { return 0; }
    size_t print(const char* message) { return 0; }
};

class MockWiFiServer : public WiFiServer {
public:
    MockWiFiServer(uint16_t port) : WiFiServer(port) {}
    WiFiClient available(uint8_t* status = NULL) { return WiFiClient(); }
};


EspMultiLogger logger(Info);

void test_write_function() {
    logger.setLogLevel(Info);
    logger.initLogger();

    // Test writing a single character
    size_t result = logger.write('A');
    TEST_ASSERT_EQUAL(1, result);

    // Test writing a newline character
    result = logger.write('\n');
    TEST_ASSERT_EQUAL(1, result);
}

void setup() {
    UNITY_BEGIN();
    RUN_TEST(test_write_function);
    UNITY_END();
}

void loop() {
    // Nothing to do here
}