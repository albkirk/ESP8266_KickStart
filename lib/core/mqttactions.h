// Adding function with project's customized MQTT actions
#include <custommqtt.h>

// Handling of received message
void on_message(const char* topic, byte* payload, unsigned int length) {

    telnet_println("New message received from Broker");

    char msg[length + 1];
    strncpy (msg, (char*)payload, length);
    msg[length] = '\0';

    telnet_println("Topic: " + String(topic));
    telnet_println("Payload: " + String((char*)msg));

    // Decode JSON request
    StaticJsonDocument<200> data;
    DeserializationError JSONerror = deserializeJson(data, msg);

    if (JSONerror) {
      telnet_println("JSON deserialization failed!. Error code: " + String(JSONerror.c_str()));
      return;
    }

    // Check request method
    String reqparam = String((const char*)data["param"]);
    String reqvalue = String((const char*)data["value"]);
    telnet_println("Received Data: " + reqparam + " = " + reqvalue);


    // System Configuration 
    if ( reqparam == "DeviceName") strcpy(config.DeviceName, (const char*)data["value"]);
    if ( reqparam == "Location") strcpy(config.Location, (const char*)data["value"]);
    if ( reqparam == "ClientID") strcpy(config.ClientID, (const char*)data["value"]);
    if ( reqparam == "DEEPSLEEP") { config.DEEPSLEEP = bool(data["value"]);storage_write(); }
    if ( reqparam == "SLEEPTime") { config.SLEEPTime = data["value"];storage_write(); }
    if ( reqparam == "ONTime") { config.ONTime = data["value"];storage_write(); }
    if ( reqparam == "ExtendONTime") if (bool(data["value"]) == true) Extend_time = 60;
    if ( reqparam == "LED") config.LED = bool(data["value"]);
    if ( reqparam == "TELNET") { config.TELNET = bool(data["value"]); storage_write(); mqtt_restart(); }
    if ( reqparam == "OTA") { config.OTA = bool(data["value"]); storage_write(); mqtt_restart(); }
    if ( reqparam == "WEB") { config.WEB = bool(data["value"]); storage_write(); mqtt_restart(); }
    if ( reqparam == "STAMode") config.STAMode = bool(data["value"]);
    if ( reqparam == "ssid") strcpy(config.ssid, (const char*)data["value"]);
    if ( reqparam == "WiFiKey") strcpy(config.WiFiKey, (const char*)data["value"]);
    if ( reqparam == "NTPServerName") strcpy(config.NTPServerName, (const char*)data["value"]);
    if ( reqparam == "Update_Time_Via_NTP_Every") config.Update_Time_Via_NTP_Every = data["value"];
    if ( reqparam == "TimeZone") config.TimeZone = data["value"];
    if ( reqparam == "isDayLightSaving") config.isDayLightSaving = bool(data["value"]);
    if ( reqparam == "Store") if (bool(data["value"]) == true) storage_write();
    if ( reqparam == "Boot") if (bool(data["value"]) == true) mqtt_restart();
    if ( reqparam == "Reset") if (bool(data["value"]) == true) storage_reset();
    if ( reqparam == "Temp_Corr") { 
            config.Temp_Corr = float(data["value"]);
            storage_write();
            mqtt_publish(mqtt_pathtele(), "Temperatura", String(getTemperature()));
       }
    if ( reqparam == "LDO_Corr") { 
            config.LDO_Corr = float(data["value"]);
            storage_write();
            mqtt_publish(mqtt_pathtele(), "BattLevel", String(getVoltage()));
       }

    // Standard Actuators/Actions 
    if ( reqparam == "Level") LEVEL = uint(data["value"]);
    if ( reqparam == "Position") POSITION = int(data["value"]);
    if ( reqparam == "Switch") SWITCH = bool(data["value"]);
    if ( reqparam == "Timer") TIMER = ulong(data["value"]);

    mqtt_custom(reqparam, reqvalue, data);

    storage_print();
}


// The callback to handle the MQTT PUBLISH messages received from Broker.
void mqtt_actions() {
    MQTTclient.setCallback(on_message);
}
