#include <ESP8266mDNS.h>
#include <ArduinoOTA.h>


  void ota_setup() {
    // ***********  OTA SETUP
    // Port defaults to 8266
    // ArduinoOTA.setPort(8266);

    // Hostname defaults to esp8266-[ChipID]
    // ArduinoOTA.setHostname("myesp8266");

    // No authentication by default
    ArduinoOTA.setPassword((const char *)"12345678");
    // Password can be set with it's md5 value as well
    // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
    // ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");

    ArduinoOTA.onStart([]() { // what to do before OTA download insert code here
        telnet_println("Starting OTA update");
      });
    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
      Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    });
    ArduinoOTA.onEnd([]() { // do a fancy thing with our board led at end
        for (int i=1;i<12;i++){
          digitalWrite(LED_esp, boolean(i % 2));
          delay(120);
        }
        digitalWrite(LED_esp,HIGH); // Switch OFF ESP LED to save energy
        telnet_println("\nEnd");
        delay(500);
        ESP.restart();
      });

    ArduinoOTA.onError([](ota_error_t error) {
        Serial.printf("Error[%u]: ", error);
        if (error == OTA_AUTH_ERROR) telnet_println("Auth Failed");
        else if (error == OTA_BEGIN_ERROR) telnet_println("Begin Failed");
        else if (error == OTA_CONNECT_ERROR) telnet_println("Connect Failed");
        else if (error == OTA_RECEIVE_ERROR) telnet_println("Receive Failed");
        else if (error == OTA_END_ERROR) telnet_println("End Failed");
        ESP.restart();
      });

    ArduinoOTA.begin();
    telnet_println("Ready for OTA");
  }


  // OTA commands to run on loop function.
  void ota_loop() {
      blink_LED(1);
      ArduinoOTA.handle();
      yield();
  }
