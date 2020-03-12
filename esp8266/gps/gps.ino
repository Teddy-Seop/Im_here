/*
 * Rui Santos 
 * Complete Project Details https://randomnerdtutorials.com
 */
 
#include <TinyGPS++.h>
#include <SoftwareSerial.h>

static const int RXPin = 4, TXPin = 5; // 12 = D6, 13 = D7
static const uint32_t GPSBaud = 9600;

// The TinyGPS++ object
TinyGPSPlus gps;

// The serial connection to the GPS device
SoftwareSerial ss(4, 5);

float latitude, longitude;
String lat_str, lot_str;

void setup(){
  Serial.begin(115200);
  while(!Serial) ;
  Serial.println("Serial on");
  ss.begin(9600);
  delay(1000);
  Serial.println(ss.read());
  ss.println("Connected");
  delay(5000);
}

void loop(){
  // This sketch displays information every time a new sentence is correctly encoded.
  Serial.println("HI");
  while (ss.available() >= 0){
    Serial.println("In while");
    Serial.println(ss.read());
    Serial.println("gps ok");
    latitude = gps.location.lat();
    lat_str = String(latitude, 6);
    longitude = gps.location.lng();
    lot_str = String(longitude, 6);
    Serial.println("in if");
    Serial.print("Latitude= "); 
    Serial.println(lat_str);
    Serial.print("Longitude= "); 
    Serial.println(lot_str);
    delay(100);
  }
}
