#include <DHT.h>


// Battery & ESP Voltage
#define Diode_Corr float(0.4)             // Battery Voltage corrective Factor due to diode voltage drop
#define Batt_Max float(4.2)               // Battery Highest voltage.  [v]
#define Batt_Min float(3.0)               // Battery lowest voltage.   [v]
#define Batt_L_Thrs 40                    // Battery threshold voltage [0%-100%] (before slepping forever).
#define Vcc float(3.3)                    // Theoretical/Typical ESP voltage. [v]
#define VADC_MAX float(1.0)               // Maximum ADC Voltage input
float voltage = 0.0;                      // Input Voltage [v]
float Batt_Level = 0.0;                   // Battery level [0%-100%]

// Temperature Variables and Sensors
#define samp_intv int(300)                // Temperature and Humidity Sampling Interval value in seconds
unsigned long curr_time;                  // Variable used to write the current time
unsigned long last_sample;                // Variable used to write time the last sample was taken

// Initialize DHT sensor.
#define DHTTYPE DHT22                     // using the DHT22 Model
DHT dht_val(DHTPIN, DHTTYPE);

//NTC temperature
#define Rs 47000                         // 47KOhm Voltage Dividor Resistor

// Timers for millis used on Sleeping and LED flash
unsigned long Extend_time=0;
unsigned long now_millis=0;
unsigned long Pace_millis=3000;
unsigned long LED_millis=300;             // 10 slots available (3000 / 300)
unsigned long BUZZER_millis=500;          // 6 Buzz beeps maximum  (3000 / 500)

// Initialize internal ADC .
ADC_MODE(ADC_VCC);                        // Comment it if you will use the ADC

// Functions //
float getInternalVoltage() {
      // 10 times averaged ADC value read
	  voltage = 0;
      for(int i = 0; i < 10; i++) {
          voltage += ESP.getVcc();
          delay(10);
      }
	  voltage = voltage / 10;
	  
      voltage = voltage / 1000.0 + Diode_Corr;
      return ((voltage - Batt_Min) / (Batt_Max - Batt_Min)) * 100.0;
}

float getVoltage() {

  do {
      delay(100);
      voltage = (analogRead(A0) / 1000.0) * Vcc + Diode_Corr;
      // Serial.print("VDD value= "); Serial.println(String(vdd));
  } while (voltage < 2);               // the ESP do not run with a supply voltage lower than 2v!! So, if it is measuring it, it's not true!
  return ((voltage - Batt_Min) / (Batt_Max - Batt_Min)) * 100.0;
}

long getRSSI() {
    // Read WiFi RSSI Strength signal

  long r = 0;
  int n = 0;

  while (n < 10) {
    r += WiFi.RSSI();
    n ++;
    }
  r = r /10;
  return r;
}


double getNTCThermister() {

  // 10 times averaged ADC value read
  int val = 0;
  for(int i = 0; i < 10; i++) {
      val += analogRead(A0);
      delay(10);
      }
  val = val / 10;

  double V_NTC = (double)val / 1024;
  double R_NTC = (Rs * V_NTC) / (Vcc - V_NTC);
  R_NTC = log(R_NTC);
  double Temp = 1 / (0.001129148 + (0.000234125 + (0.0000000876741 * R_NTC * R_NTC ))* R_NTC );
  Temp = Temp - 273.15 + config.Temp_Corr;
    return Temp;
}


float getTemperature() {
    // Read temperature as Celsius (DHT22 range -40 to +125 degrees Celsius) or -100 if error

  float t;
  int n = 0;

  while (n < 10) {
    t = dht_val.readTemperature() + config.Temp_Corr;
    // Check if any reads failed and exit.
    if (isnan(t)) {
      Serial.println("Failed to read temperature from DHT sensor!");
      delay(1000);
      //t = NULL;
      n ++;
    }
    else {
      //Serial.print(t);
      return t;
    }
  }
return -100;
}


float getHumidity() {
    // Read Humidity as Percentage (0-100%) or -1 if error

  float h;
  int n = 0;

  while (n < 10 ) {
    h = dht_val.readHumidity();
    // Check if any reads failed and exit.
    if (isnan(h)) {
      Serial.println("Failed to read humidity from DHT sensor!");
      delay(1000);
      //h = NULL;
      n ++;
    }
    else {
      //Serial.print(h);
      return h;
    }
  }
return -1;
}

void BootESP() {
  Serial.println("Booting in 3 seconds...");
  delay(3000);
  ESP.restart();
}

void FormatConfig() {     // WARNING!! To be used only as last resource!!!
    Serial.println(ESP.eraseConfig());
    delay(5000);
    ESP.reset();
}

void blink_LED(int slot) {    // slot range 1 to 10 =>> 3000/300
    now_millis = millis() % Pace_millis;
    if (now_millis > LED_millis*(slot-1) && now_millis < LED_millis*slot-LED_millis/2 ) {
        digitalWrite(LED_esp, boolean(config.LED));  // toggles LED status. will be restored by command above
        delay(LED_millis/2);
    }
}

void flash_LED(unsigned int n_flash) {    // number of flash 1 to 6 =>> 3000/500
    for (size_t i = 0; i < n_flash; i++) {
      digitalWrite(LED_esp, boolean(config.LED));      // Turn LED on
      delay(LED_millis/3);
      digitalWrite(LED_esp, boolean(!config.LED));     // Turn LED off
      delay(LED_millis/3);
    }
}

void Buzz(unsigned int n_beeps) {    // number of beeps 1 to 6 =>> 3000/500
    for (size_t i = 0; i < n_beeps; i++) {
      digitalWrite(BUZZER, HIGH);      // Turn Buzzer on
      delay(BUZZER_millis/6);
      digitalWrite(BUZZER, LOW);      // Turn Buzzer off
      delay(BUZZER_millis/6);
    }
}
