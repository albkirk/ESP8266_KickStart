# ESP 8266 12F Project "Kick Start" Template


This is a personal project to create a starting point for all my ESP projects.
It is written in C++ under PlatformIO IDE (integrated on ATOM platform).
I'm coding my own variant of this popular project, with some inspiration and lessons (code Snippets) from some well know projects like:
    ESPURNA: https://github.com/SensorsIot/Espurna-Framework
    TASMOTA: https://github.com/arendst/Sonoff-Tasmota


    Goals of this project:
    1. Project should run locally, even when WiFi newtork is down (it seems obvious, but ...)
    2. Remote upgrade Over-the-Air (OTA).
    3. MQTT Publish/Subscribe support
        3.1.  ALl data is sent under "/<clientID>/<location>/<device name>/telemetry/<topic name>" in string format
        3.2.  ALl data received sent under "/<clientID>/<location>/<device name>/configure/<topic name>" in string format
    4. User commands feedback by flashing the "internal" LED and/or adittional active buzzer.
	5. OTA, TELNET, LED, RESET, REBOOT, STORE and DEEPSLEEP functionalities can be remotely enabled/disabled/triggered via defined "true"/"false" MQTT commands.
	6. WiFi and Battery status sent via MQTT
	7. DeepSleep support with configurable On-Time and Sleep-Time
    8. Remote "Debug" using Telnet (enabled viua MQTT)
    9. Long operational live (it will be running 365 days a year, so, it recovers from Wifi or MQTT loss)
	
    Future features!!
    10. NTP sync
	11. Web Page for "minimum" configuration
	12. Power consumption optimization
   
