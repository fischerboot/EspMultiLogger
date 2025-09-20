### EspMultiLogger
This Library should facilitate an logger which generates from a single input multiple outputs on ESPs (e,g, Serialoutput, Telnet, MQTT and many more)
## Usage

## Startup
For each Loglevel a new EspMultilogger instance is needed. 
```
  InfoLogger = new EspMultiLogger(Info);
  DebugLogger = new EspMultiLogger(Debug);
```
The whole (only once during startup for all logger) MultiLogger needed to be initalized during startup. (This could be necessary for different logger types. e.g. Serial must be intialized or simliar.)
The Same applies for the SetLogLevel as well. (planned to be opitmized out with a default value)
```
  EspMultiLogger::setLogLevel(Debug);
  EspMultiLogger::initLogger();
```
## loop

The EspMultilogger need to be called regularly to send out its logs or supervise the connections. 

```
EspMultiLogger::loopLogger();
```

## Logging
```
InfoLogger->println("LED is on"); 
```