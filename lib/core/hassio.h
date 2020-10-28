
#define discovery_prefix "homeassistant"

//JSON Variables
DynamicJsonDocument device_doc(128);
StaticJsonDocument<512> discovery_doc;
char device_jsonString[128];
char discovery_jsonString[512];
                                

void hassio_device() {
    // device":{"name":"AmbiSense","identifiers":"09B376","manufacturer":"albroteam","model":"Ambiv2","sw_version":"01.01"}
    // Purge old JSON data and Load new values
    device_doc.clear();
    device_doc["name"] = config.DeviceName;                     // Device friendly name
    device_doc["ids"] = ChipID;                                 // identifiers
    device_doc["mf"] = BRANDName;                               // manufacturer
    device_doc["mdl"] = MODELName;                              // model
    device_doc["sw"] = SWVer;                                   // sw_version
}

// HASSIO Configuration registration
void config_entity(String component = "sensor", String device_class = "Battery", String param = "") {
    //"sensor" have well defined classes: battery, humidity, illuminance, signal_strength, temperature, power, pressure, etc
    if(param == "") param = device_class;       // use the "device_class" as "param" value

    // Discovery Topic: // <discovery_prefix>/<component>/[<node_id>/]<object_id>/config
    String mqtt_pathhassio = String(discovery_prefix) + "/" + component + '/' + ChipID.c_str() + '-' + param + '/';

    
    // Purge old JSON data and Load new values
    discovery_doc.clear();
    discovery_doc["unique_id"] = ChipID + '-' + param;
    discovery_doc["name"] = String(config.DeviceName) + ' ' + param;
    discovery_doc["device"] = device_doc;

    if(component == "sensor" && device_class != "none") discovery_doc["device_class"] = device_class;
    discovery_doc["stat_t"] = mqtt_pathtele + param;  // state_topic

    if(device_class == "Battery" || device_class == "Humidity" ) {
        discovery_doc["unit_of_measurement"] = "%";
        discovery_doc["value_template"] = "{{ value | int }}";
    }
    if(device_class == "signal_strength") {
        discovery_doc["unit_of_measurement"] = "dBm";
        discovery_doc["value_template"] = "{{ value | int }}";
    }
    if(device_class == "Temperature") {
        discovery_doc["unit_of_measurement"] = "ºC";
        discovery_doc["value_template"] = "{{value | float }}";
    }

    if(component == "switch") {
        discovery_doc["cmd_t"] = mqtt_pathconf + param;   // command_topic
        //discovery_doc["stat_off"] = "0";
        //discovery_doc["stat_on"]  = "1";
        discovery_doc["pl_off"]  = "0";
        discovery_doc["pl_on"]   = "1";
    }
    

    serializeJson(discovery_doc, discovery_jsonString);                     //Serialize JSON data to string
    //telnet_println("HASSIO Config: " + String(discovery_jsonString));
    mqtt_publish(mqtt_pathhassio, "config", String(discovery_jsonString));
}


void delete_entity(String component = "sensor", String device_class = "Battery", String param = "") {
    if(param == "") param = device_class;       // use the "device_class" as "param" value

    String mqtt_pathhassio = String(discovery_prefix) + "/" + component + '/' + ChipID.c_str() + '-' + param + '/';

    mqtt_publish(mqtt_pathhassio, "config", "");
}
