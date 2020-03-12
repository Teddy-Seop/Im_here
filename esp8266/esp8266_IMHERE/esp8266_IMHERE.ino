/*
 * WebSocketClient.ino
 *
 *  Created on: 24.05.2015
 *
 */

#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <TinyGPS++.h>
#include <SoftwareSerial.h> 
#include <WebSocketsClient.h>
#include <Arduino_JSON.h>

#include <Hash.h>

ESP8266WiFiMulti WiFiMulti;
WebSocketsClient webSocket;
JSONVar myObj;

#define USE_SERIAL Serial1

const char* ssid = "K_iptime";
const char* password = "12345678";
float lat, lot;
String lat_str, lot_str;

TinyGPSPlus gps;
SoftwareSerial ss(4, 5);
int speakerPin = 15;

int Slength = 51;                                                                                                // 노래의 총 길이 설정
char notes[] = "eeeeeeegcde fffffeeeeddedgeeeeeeegcde fffffeeeggfdc";             // 음계 설정
int beats[] = { 1, 1, 2, 1, 1, 2, 1, 1, 1, 1, 4,
                1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2,
                1, 1, 2, 1, 1, 2, 1, 1, 1, 1, 4,
                1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 4
              };                                                                                                           // 해당하는 음이 울리는 길이 설정

int tempo = 300;                                                                                                   // 캐럴이 연주되는 속도

void playTone(int tone, int duration)
{
  for (long i = 0; i < duration * 1000L; i += tone * 2)
  {
    digitalWrite(speakerPin, HIGH);
    delayMicroseconds(tone);
    digitalWrite(speakerPin, LOW);
    delayMicroseconds(tone);
  }
}

void playNote(char note, int duration) {
  char names[] = { 'c', 'd', 'e', 'f', 'g', 'a', 'b', 'C' };                                                         //음계 함수 설정
  int tones[] = { 1915, 1700, 1519, 1432, 1275, 1136, 1014, 956 };                                 // 음계 톤 설정
  for (int i = 0; i < 8; i++)
  {
    if (names[i] == note)
    {
      playTone(tones[i], duration);
    }
  }
}

String converter(uint8_t *str){
  return String((char *)str);
}

void Setgps() {
  while (ss.available() > 0) {
    if (gps.encode(ss.read())) {
      if (gps.location.isValid()) {
        lat = gps.location.lat();
        lat_str = String(lat, 6);
        lot = gps.location.lng();
        lot_str = String(lot, 6);
      }
    }
  }
}

void SpeakerOn() {
  for (int i = 0; i < Slength; i++)
  {
    if (notes[i] == ' ')
    {
      delay(beats[i] * tempo); // rest
    }
    else
    {
      playNote(notes[i], beats[i] * tempo);
    }
    delay(tempo / 10);
  }
  delay(500);
}


void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {

	switch(type) {
		case WStype_DISCONNECTED:
			Serial.printf("[WSc] Disconnected!\n");
			break;
		case WStype_CONNECTED:
			Serial.printf("[WSc] Connected to url: %s\n", payload);
			// send message to server when Connected
			webSocket.sendTXT("Connected");
		
			break;
		case WStype_TEXT: {
			Serial.printf("[WSc] get text: %s\n", payload);
      JSONVar Obj = JSON.parse(converter(payload));
      if(Obj.hasOwnProperty("message")) {
        if(strcmp(Obj["Device_num"], "1") || strcmp(Obj["message"], "Sound_on")) {
          SpeakerOn();
        }
      }
      Setgps();
			// send message to server
      JSONVar res;
      res["device_num"] = "1";
      res["option"] = "GPS";
      res["message"] = "GPS_data";
      res["latitude"] = lat_str;
      res["longitude"] = lot_str;
      String SendMsg = JSON.stringify(res);
			webSocket.sendTXT(SendMsg);
			break;
		}
		case WStype_BIN:
			Serial.printf("[WSc] get binary length: %u\n", length);
			hexdump(payload, length);

			// send data to server
			// webSocket.sendBIN(payload, length);
			break;
  }

}

void setup() {
	// USE_SERIAL.begin(921600);
	Serial.begin(115200);
  ss.begin(9600);
  pinMode(speakerPin, OUTPUT);
  
	Serial.setDebugOutput(true);
	Serial.setDebugOutput(true);

	Serial.println();
	Serial.println();
	Serial.println();

	for(uint8_t t = 4; t > 0; t--) {
		Serial.printf("[SETUP] BOOT WAIT %d...\n", t);
		Serial.flush();
		delay(1000);
	}

	WiFiMulti.addAP(ssid, password);

	//WiFi.disconnect();
	while(WiFiMulti.run() != WL_CONNECTED) {
		delay(100);
	}

	// server address, port and URL
	webSocket.begin("192.168.52.100", 6379, "/");

	// event handler
	webSocket.onEvent(webSocketEvent);

	// use HTTP Basic Authorization this is optional remove if not needed
	webSocket.setAuthorization("user", "Password");

	// try ever 5000 again if connection has failed
	webSocket.setReconnectInterval(5000);
  
  // start heartbeat (optional)
  // ping server every 15000 ms
  // expect pong from server within 3000 ms
  // consider connection disconnected if pong is not received 2 times
  webSocket.enableHeartbeat(15000, 3000, 2);
  // SpeakerOn();
}

void loop() {
	webSocket.loop();
// SpeakerOn();
// delay(5000);
// Setgps();
// Serial.print("lat : ");
// Serial.println(lat_str);
// Serial.print("lot : ");
// Serial.println(lot_str);
}
