// **** Project code definition here ...
//#include <flash.h>
//#include <ambient.h>




// **** Project code functions here ...
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
      
}

void project_loop() {
  // Ambient handing
      //if (TIMER >0) if ((millis() - 3500) % (TIMER * 60000) < 5) ambient_data();      // TIMER bigger than zero on div or dog bites!!

}
