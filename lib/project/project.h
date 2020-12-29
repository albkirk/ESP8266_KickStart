// **** Project code definition here ...
//#include <flash.h>
//#include <ambient.h>
#include <mygps.h>

//JSON Variables
char telemetry_jsonString[256];
DynamicJsonDocument telemetry_doc(256);


// **** Project code functions here ...
// **** Project code functions here ...
void print_gps_data () {
        if (GPS_Valid) {
            telnet_print("Sats: " + String(GPS_Sat) + "\t");
            telnet_print("Lat: " + String(GPS_Lat, 4) + "\t");
            telnet_print("Lng: " + String(GPS_Lng, 4) + "\t");
            telnet_print("Alt: " + String(GPS_Alt, 1) + "\t");
            telnet_print("Course: " + String(GPS_Course, 1) + "\t");
            telnet_print("Speed: " + String(GPS_Speed, 1) + "\t");
            telnet_println("");
        }
        else telnet_println("GPS Not FIX");
}


void send_Telemetry() {
    char fbat[3];        // long enough to hold complete floating string

    // Purge old JSON data and Load new values
    telemetry_doc.clear();
    telemetry_doc["Interval"] = config.SLEEPTime;
    telemetry_doc["Timestamp"] = curUnixTime();
    telemetry_doc["BatLevel"] = String(dtostrf(getBattLevel(),3,0,fbat)).toFloat();
    telemetry_doc["RSSI"] = getRSSI();
    if (config.HW_Module){
        gps_update();
        if (GPS_Valid){
            telemetry_doc["Sat"] = String(GPS_Sat).toInt();
            telemetry_doc["Lat"] = String(GPS_Lat, 6).toFloat();
            telemetry_doc["Lng"] = String(GPS_Lng, 6).toFloat();
            telemetry_doc["Speed"] = String(GPS_Speed, 1).toFloat();
        }
    }

    serializeJson(telemetry_doc, telemetry_jsonString);             //Serialize JSON data to string
    //telnet_println("Telemetry: " + String(telemetry_jsonString));
    mqtt_publish(mqtt_pathtele, "Telemetry", String(telemetry_jsonString));
}

void project_hw() {
 // Output GPIOs


 // Input GPIOs

    //flash_setup();                                // Required for telemetry storage in Flash
}


void project_setup() {
  // Start Ambient devices
      //ambient_setup();
      //TIMER = (ulong)config.SLEEPTime;            // TIMER value (Recommended 15 minutes) to get Ambient data.
      //ambient_data();
    gps_setup();
    modem_setup();
    send_Telemetry();     
}

void project_loop() {
  // Ambient handing
      //if (TIMER >0) if ((millis() - 3500) % (TIMER * 60000) < 5) ambient_data();      // TIMER bigger than zero on div or dog bites!!
    if (config.SLEEPTime >0) if ((millis() + 3500) % (config.SLEEPTime * 60000) < 5) send_Telemetry();  // TIMER bigger than zero on div or dog bites!!

}
