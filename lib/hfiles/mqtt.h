// MQTT Constants
#define MQTT_MAX_PACKET_SIZE 512                 // Default: 128 bytes

#include <PubSubClient.h>


// MQTT PATH Structure
// /clientid/location/deviceName/telemetry/<topic>    --> typically, used when publishing info/status
// /clientid/location/deviceName/configure/<topic>    --> typically, used when subscribing for actions

// EXAMPLEs
// /001001/room/Estore/telemetry/RSSI                 --> value in dBm
// /001001/kitchen/AmbiSense/telemetry/BatLevel       --> 0 - 100
// /001001/outside/MailBox/telemetry/Status           --> OK / LOWBat
// /001001/kitchen/AmbSense/telemetry/DeepSleep       --> time in sec
// /001001/outside/MailBox/telemetry/LED              --> True / False  to turn LED ON/OFF
// /001001/outside/MailBox/configure/LED              --> Set True / False to turn LED ON/OFF


// MQTT Variables
unsigned int MQTT_Retry = 125;                             // Timer to retry the MQTT connection
long MQTT_LastTime = 0;                           // Last MQTT connection attempt time stamp
int MQTT_errors = 0;                              // MQTT errors Counter

// Initialize MQTT Client
PubSubClient MQTTclient(wifiClient);


// MQTT Functions //
String mqtt_pathtele() {
  return "/" + config.ClientID + "/" + config.Location + "/" + config.DeviceName + "/telemetry/";
}


String mqtt_pathconf() {
  return "/" + config.ClientID + "/" + config.Location + "/" + config.DeviceName + "/configure/";
}


void mqtt_publish(String pubpath, String pubtopic, String pubvalue, boolean toretain = false) {
  String topic = "";
  topic += pubpath; topic += pubtopic;     //topic += "/";
  // Send payload
  if (MQTTclient.publish(topic.c_str(), pubvalue.c_str(), toretain) == 1) telnet_println("MQTT published:  " + String(topic.c_str()) + " = " + String(pubvalue.c_str()));
  else {
      //flash_LED(2);
      telnet_println("");
      telnet_println("!!!!! MQTT message NOT published. Try uncomment #define MQTT_MAX_PACKET_SIZE 512 at the beginning of mqtt.h file");
      telnet_println("");
  }
}


void mqtt_subscribe(String subpath, String subtopic) {
    String topic = "";
    topic += subpath; topic += subtopic;
    if( MQTTclient.subscribe(topic.c_str())) telnet_println("subscribed to topic: " + topic);
    else telnet_println("Error on MQTT subscription!");
}

void mqtt_unsubscribe(String subpath, String subtopic) {
    String topic = "";
    topic += subpath; topic += subtopic;
    if( MQTTclient.unsubscribe(topic.c_str())) telnet_println("unsubscribed to topic: " + topic);
    else telnet_println("Error on MQTT unsubscription!");
}

// Handling of received message
void on_message(const char* topic, byte* payload, unsigned int length) {

    telnet_println("New message received from Broker");

    char msg[length + 1];
    strncpy (msg, (char*)payload, length);
    msg[length] = '\0';

    telnet_println("Topic: " + String(topic));
    telnet_println("Payload: " + String((char*)msg));

    // Decode JSON request
    StaticJsonBuffer<200> jsonBuffer;
    JsonObject& data = jsonBuffer.parseObject((char*)msg);

    if (!data.success()) {
      telnet_println("JSON Object parsing failed");
      return;
    }

    // Check request method
    String reqparam = String((const char*)data["param"]);
    String reqvalue = String((const char*)data["value"]);
    telnet_println("Received Data: " + reqparam + " = " + reqvalue);

    if ( reqparam == "DeviceName") config.DeviceName = String((const char*)data["value"]);
    if ( reqparam == "Location") config.Location = String((const char*)data["value"]);
    if ( reqparam == "ClientID") config.ClientID = String((const char*)data["value"]);
    if ( reqparam == "ONTime") config.ONTime = data["value"];
    if ( reqparam == "SLEEPTime") config.SLEEPTime = data["value"];
    if ( reqparam == "DEEPSLEEP") { config.DEEPSLEEP = data["value"]; storage_write(); BootESP(); }
    if ( reqparam == "LED") config.LED = data["value"];
    if ( reqparam == "TELNET") { config.TELNET = data["value"]; storage_write(); BootESP(); }
    if ( reqparam == "OTA") { config.OTA = data["value"]; storage_write(); BootESP(); }
    if ( reqparam == "WEB") { config.WEB = data["value"]; storage_write(); BootESP(); }
    if ( reqparam == "STAMode") config.STAMode = data["value"];
    if ( reqparam == "ssid") config.ssid = String((const char*)data["value"]);
    if ( reqparam == "WiFiKey") config.WiFiKey = String((const char*)data["value"]);
    if ( reqparam == "NTPServerName") config.NTPServerName = String((const char*)data["value"]);
    if ( reqparam == "Update_Time_Via_NTP_Every") config.Update_Time_Via_NTP_Every = data["value"];
    if ( reqparam == "TimeZone") config.TimeZone = data["value"];
    if ( reqparam == "isDayLightSaving") config.isDayLightSaving = data["value"];
    if ( reqparam == "Store") if (data["value"] == true) storage_write();
    if ( reqparam == "Boot") if (data["value"] == true) BootESP();
    if ( reqparam == "Reset") if (data["value"] == true) storage_reset();
	if ( reqparam == "Temp_Corr") {
		config.Temp_Corr = data["value"];
		storage_write();
		mqtt_publish(mqtt_pathtele(), "Temperatura", String(getNTCThermister()));
	}
    storage_print();
    Extend_time=60;
}


// The callback for when a PUBLISH message is received from the server.
void mqtt_callback() {
    MQTTclient.setCallback(on_message);
}


int mqtt_connect() {
    telnet_print("Connecting to MQTT Broker ...");
    MQTTclient.setServer(config.MQTT_Server.c_str(), config.MQTT_Port);
    // Attempt to connect (clientId, mqtt username, mqtt password)
    if ( MQTTclient.connect(ChipID.c_str(), config.MQTT_User.c_str(), config.MQTT_Password.c_str())) {
        telnet_println( "[DONE]" );
        mqtt_subscribe(mqtt_pathconf(), "+");
    }
    else {
        telnet_print( "MQTT ERROR! ==> " );
        telnet_println( String(MQTTclient.state()) );
        }
    return MQTTclient.state();
}

void mqtt_disconnect() {
    MQTTclient.disconnect();
    telnet_print("Disconnected from MQTT Broker.");
}

// MQTT commands to run on setup function.
int mqtt_setup() {
    int MQTTstatus = mqtt_connect();
    mqtt_callback();
    return MQTTstatus;
}


// MQTT commands to run on loop function.
void mqtt_loop() {
    if (!MQTTclient.loop()) {
        if ( millis() - MQTT_LastTime > (MQTT_Retry * 1000)) {
            MQTT_errors ++;
            Serial.print( "in loop function MQTT ERROR! #: " + String(MQTT_errors) + "  ==> "); Serial.println( MQTTclient.state() );
            MQTT_LastTime = millis();
            mqtt_connect();
        }
    }
    yield();
}
