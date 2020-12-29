#include <TinyGPS++.h>
#include <SoftwareSerial.h>


static const uint32_t GPSBaud = 9600;
#define GPS_cycle 1100                               // Setting 1000 milisecond by default -- it Depends on what's configured on GPS module!
#define GPS_Update 10                                // Value (in Seconds) to update GPS Data variables
uint32_t GPS_Sat;
double GPS_Lat, GPS_Lng, GPS_Alt, GPS_Course, GPS_Speed; // GPS data: Satellites, Latitude, Longitude, Altitude, Course and Speed
bool GPS_Valid = false;

// The TinyGPS++ object
TinyGPSPlus gps;

// The serial connection to the GPS device
SoftwareSerial ss(GPS_RX, GPS_TX);




// This custom version of delay() ensures that the gps object
// is being "fed".
static void gps_update(unsigned long ms = GPS_cycle)
{
  unsigned long start = millis();
  do 
  {
    while (ss.available())
      gps.encode(ss.read());
  } while (millis() - start < ms);

  if(gps.location.isValid()) {
        GPS_Valid = true;
        GPS_Sat = gps.satellites.value();
        GPS_Lat = gps.location.lat();
        GPS_Lng = gps.location.lng();
        GPS_Alt = gps.altitude.meters();
        GPS_Course = gps.course.deg();
        GPS_Speed = gps.speed.kmph();
  }
  else {
        GPS_Valid = false;
  }
}


void gps_setup()
{
  Serial.print(F("TinyGPS++ library v. ")); Serial.println(TinyGPSPlus::libraryVersion());
  ss.begin(GPSBaud);
/*
  Serial.println();
  Serial.println(F("Sats HDOP  Latitude   Longitude   Fix  Date       Time     Date Alt    Course Speed Card  Distance Course Card  Chars Sentences Checksum"));
  Serial.println(F("           (deg)      (deg)       Age                      Age  (m)    --- from GPS ----  ---- to London  ----  RX    RX        Fail"));
  Serial.println(F("----------------------------------------------------------------------------------------------------------------------------------------"));
*/
}


void gps_loop()
{
  gps_update(GPS_cycle);

  if (millis() > 5000 && gps.charsProcessed() < 10)
    Serial.println(F("No GPS data received: check wiring"));

/*  
  static const double LONDON_LAT = 51.508131, LONDON_LON = -0.128002;

  printInt(gps.satellites.value(), gps.satellites.isValid(), 5);
  printFloat(gps.hdop.hdop(), gps.hdop.isValid(), 6, 1);
  printFloat(gps.location.lat(), gps.location.isValid(), 11, 6);
  printFloat(gps.location.lng(), gps.location.isValid(), 12, 6);
  printInt(gps.location.age(), gps.location.isValid(), 5);
  printDateTime(gps.date, gps.time);
  printFloat(gps.altitude.meters(), gps.altitude.isValid(), 7, 2);
  printFloat(gps.course.deg(), gps.course.isValid(), 7, 2);
  printFloat(gps.speed.kmph(), gps.speed.isValid(), 6, 2);
  printStr(gps.course.isValid() ? TinyGPSPlus::cardinal(gps.course.deg()) : "*** ", 6);

  unsigned long distanceKmToLondon = (unsigned long)TinyGPSPlus::distanceBetween(gps.location.lat(),gps.location.lng(),LONDON_LAT, LONDON_LON) / 1000;
  printInt(distanceKmToLondon, gps.location.isValid(), 9);

  double courseToLondon = TinyGPSPlus::courseTo(gps.location.lat(), gps.location.lng(), LONDON_LAT, LONDON_LON);
  printFloat(courseToLondon, gps.location.isValid(), 7, 2);

  const char *cardinalToLondon = TinyGPSPlus::cardinal(courseToLondon);
  printStr(gps.location.isValid() ? cardinalToLondon : "*** ", 6);
  printInt(gps.charsProcessed(), true, 6); printInt(gps.sentencesWithFix(), true, 10); printInt(gps.failedChecksum(), true, 9);
  Serial.println();
 */


}
