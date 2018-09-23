/*
 * * ESP8266 template with config web page
 * based on BVB_WebConfig_OTA_V7 from Andreas Spiess https://github.com/SensorsIot/Internet-of-Things-with-ESP8266
 * to do: create button on web page to setup AP or Client WiFi Mode
 */
#include <ESP8266WiFi.h>


#define SWVer 1                                 // Software version (use byte 1-255 range values!)
#define ChipID String(ESP.getChipId())          // ChipID is taken internally
#define ESP_SSID String("ESP8266-" + ChipID)    // SSID used as Acces Point
#define LED_esp 2                               // ESP Led in connected to GPIO 2
#define DHTPIN -1                               // GPIO Connected to DHT22 Data PIN. -1 means NO DHT used!
#define GPIO04 4                                // GPIO 4 for future usage
#define GPIO05 5                                // GPIO 5 for future usage
#define BUZZER 0                                // (Active) Buzzer



struct strConfig {
  String DeviceName;                      //
  String Location;                 		  //
  String ClientID;                        //
  byte  ONTime;                           //
  byte  SLEEPTime;                        //
  boolean DEEPSLEEP;                      //
  boolean LED;                            //
  boolean TELNET;                         //
  boolean OTA;                            //
  boolean WEB;                            //
  boolean Remote_Allow;                   //
  boolean STAMode;                        //
  String ssid;                            //
  String WiFiKey;                         //
  boolean dhcp;                           //
  byte IP[4];                             //
  byte Netmask[4];                        //
  byte Gateway[4];                        //
  String NTPServerName;                   //
  long TimeZone;                          //
  long Update_Time_Via_NTP_Every;         //
  boolean isDayLightSaving;               //
  String MQTT_Server;                     //
  long MQTT_Port;                         //
  String MQTT_User;                       //
  String MQTT_Password;                   //
  long Temp_Corr;                         // Sensor Temperature correction Factor, typically due to electronic self heat.
} config;

void config_defaults() {
    Serial.println("Setting config Default values");

    config.DeviceName = String("ESP_Generic");        // Device Name
    config.Location = String("MainRoom");             // Device Location
    config.ClientID = String("001001");               // Client ID (used on MQTT)
    config.ONTime = 60;                               // 0-255 seconds (Byte range)
    config.SLEEPTime = 0;                             // 0-255 minutes (Byte range)
    config.DEEPSLEEP = false;                         // 0 - Disabled, 1 - Enabled
    config.LED = true;                                // 0 - OFF, 1 - ON
    config.TELNET = false;                            // 0 - Disabled, 1 - Enabled
    config.OTA = true;                                // 0 - Disabled, 1 - Enabled
    config.WEB = false;                               // 0 - Disabled, 1 - Enabled
    config.Remote_Allow = true;                       // 0 - Not Allow, 1 - Allow remote operation
    config.STAMode = true;                            // 0 - AP Mode, 1 - Station Mode
//    config.ssid = ESP_SSID;                           // SSID of access point
//    config.WiFiKey = "";                              // Password of access point
    config.ssid = String("ThomsonCasaN");             // Testing SSID
    config.WiFiKey = String("12345678");              // Testing Password
    config.dhcp = true;
    config.IP[0] = 192; config.IP[1] = 168; config.IP[2] = 1; config.IP[3] = 10;
    config.Netmask[0] = 255; config.Netmask[1] = 255; config.Netmask[2] = 255; config.Netmask[3] = 0;
    config.Gateway[0] = 192; config.Gateway[1] = 168; config.Gateway[2] = 1; config.Gateway[3] = 254;
    config.NTPServerName = String("pt.pool.ntp.org");
    config.Update_Time_Via_NTP_Every = 1200;          // Time in minutes
    config.TimeZone = 10;                             // -120 to 130 see Page_NTPSettings.h
    config.isDayLightSaving = 1;                      // 0 - Disabled, 1 - Enabled
    config.MQTT_Server = String("iothubna.hopto.org");// MQTT Broker Server (URL or IP)
    config.MQTT_Port = 1883;                          // MQTT Broker TCP port
    config.MQTT_User = String("admin");               // MQTT Broker username
    config.MQTT_Password = String("admin");           // MQTT Broker password
    config.Temp_Corr = 0;     // Sensor Temperature Correction Factor, typically due to electronic self heat.
}


#include <storage.h>
#include <global.h>
#include <wifi.h>
#include <telnet.h>
//#include <ntp.h>
#include <web.h>
#include <ota.h>
#include <mqtt.h>


// **** Normal code definition here ...



void setup() {
// Output GPIOs
  pinMode(LED_esp, OUTPUT);
  digitalWrite(LED_esp, HIGH);    // Turn LED off

// Input GPIOs
  pinMode(GPIO04, INPUT_PULLUP);

  Serial.begin(115200);
  WiFi.mode(WIFI_OFF);

  Serial.println(" ");
  Serial.println("Hello World!");
  Serial.println("My ID is " + ChipID + " and I'm running version " + String(SWVer));
  Serial.println(ESP.getResetReason());

  // Start Storage service and read stored configuration
      storage_setup();

  // Start WiFi service (Station or as Access Point)
      wifi_setup();

  // Start TELNET service
      if (config.TELNET) telnet_setup();

  // Start OTA service
      if (config.OTA) ota_setup();

  // Start ESP Web Service
      if (config.WEB) web_setup();

  // Start MQTT service
      int mqtt_status = mqtt_setup();

  // Start DHT device
      if (DHTPIN>=0) dht_val.begin();

  // **** Normal SETUP Sketch code here...
  if (mqtt_status == MQTT_CONNECTED) {
      mqtt_publish(mqtt_pathtele(), "Boot", ESP.getResetReason());
      mqtt_publish(mqtt_pathtele(), "ChipID", ChipID);
      mqtt_publish(mqtt_pathtele(), "SWVer", String(SWVer));
      mqtt_publish(mqtt_pathtele(), "Status", "Mains");
      //mqtt_publish(mqtt_pathtele(), "BatLevel", String(getInternalVoltage()));
      mqtt_publish(mqtt_pathtele(), "RSSI", String(getRSSI()));
      mqtt_publish(mqtt_pathtele(), "IP", WiFi.localIP().toString());
  }

//  // Check Battery Level
//      Batt_Level = getInternalVoltage();
//      mqtt_publish(mqtt_pathtele(), "BatLevel", String(Batt_Level));
//      if (Batt_Level < Batt_L_Thrs) {
//        mqtt_publish(mqtt_pathtele(), "Status", "LOW Battery");
//        mqtt_disconnect();
//        telnet_println("Going to sleep forever.  Please, recharge the battery ! ! ! ");
//        delay(500);
//        ESP.deepSleep(0);   // Sleep forever
//      }
//      else mqtt_publish(mqtt_pathtele(), "Status", "Battery");

} // end of setup()


void loop() {
  // allow background process to run.
      yield();

  // LED handling usefull if you need to identify the unit from many
      digitalWrite(LED_esp, boolean(!config.LED));  // Values is reversed due to Pull-UP configuration

  // TELNET handling
      if (config.TELNET) telnet_loop();

  // OTA request handling
      if (config.OTA) ota_loop();

  // ESP WEBServer requests handling
      if (config.WEB) web_loop();

  // WiFi handling
      wifi_loop();

  // MQTT handling
      mqtt_loop();


  // **** Normal LOOP Skecth code here ...
  if (config.DEEPSLEEP && !config.TELNET && !config.OTA && !config.WEB && (millis()/1000) > (ulong(config.ONTime) + Extend_time)) {
    mqtt_publish(mqtt_pathtele(), "Status", "DeepSleep");
    mqtt_disconnect();
    telnet_println("Going to sleep until next event... zzZz :) ");
    delay(1500);
    ESP.deepSleep(config.SLEEPTime * 60 * 1000000);   // time in minutes converted to microseconds
  }

  if (digitalRead(GPIO04) && !config.LED) config.LED = true;
  if (!digitalRead(GPIO04) && config.LED) config.LED = false;




}  // end of loop()
