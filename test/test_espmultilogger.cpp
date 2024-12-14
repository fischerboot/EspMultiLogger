#include <Arduino.h>
#include <unity.h>
#include "EspMultiLogger.h"

// Mock the WiFiClient and WiFiServer for the tests
class MockWiFiClient : public WiFiClient {
public:
    bool connected() override {
        return true;
    }
    size_t println(const char* message) override {
        return strlen(message);
    }
    size_t print(const char* message) override {
        return strlen(message);
    }
    int available() override {
        return 0;
    }
    int read() override {
        return -1;
    }
};

class MockWiFiServer : public WiFiServer {
public:
    MockWiFiServer(uint16_t port) : WiFiServer(port) {}
    WiFiClient available() override {
        return MockWiFiClient();
    }
};

MockWiFiServer TelnetServer(23);
MockWiFiClient TelnetClient[MAX_TELNET_CLIENTS];

EspMultiLogger logger(INFO);

void test_write_function() {
    logger.setLogLevel(INFO);
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