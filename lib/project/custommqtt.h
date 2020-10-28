// Function to insert customized MQTT Configuration actions

void  mqtt_custom(String command, String cmd_value) {

  if ( command == "Info" && bool(cmd_value.toInt())) {
    //status_report();
    if (BattPowered) mqtt_publish(mqtt_pathtele, "BattLevel", String(getVoltage()));
    mqtt_publish(mqtt_pathtele, "Switch", String(SWITCH));
    mqtt_publish(mqtt_pathtele, "Timer", String(TIMER));
    mqtt_publish(mqtt_pathtele, "NTP_Sync", String(NTP_Sync));
    mqtt_publish(mqtt_pathtele, "DateTime", String(curDateTime()));
  }
  if ( command == "Config" && bool(cmd_value.toInt())) {
    mqtt_publish(mqtt_pathtele, "OTA", String(config.OTA));
    mqtt_publish(mqtt_pathtele, "TELNET", String(config.TELNET));
    mqtt_publish(mqtt_pathtele, "WEB", String(config.WEB));
    mqtt_publish(mqtt_pathtele, "Interval", String(config.SLEEPTime));   
  }

}
