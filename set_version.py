import json

with open("library.json") as f:
    data = json.load(f)
    version = data.get("version", "unknown")

with open("lib/EspMultiLogger/version.h", "w") as f:
    f.write(f'#define ESPMULTILOGGER_VERSION "{version}"\n')