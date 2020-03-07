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
  digitalWrite(speakerPin, HIGH);
  delay(5000);
  digitalWrite(speakerPin, LOW);
}


void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {

	switch(type) {
		case WStype_DISCONNECTED:
			USE_SERIAL.printf("[WSc] Disconnected!\n");
			break;
		case WStype_CONNECTED:
			USE_SERIAL.printf("[WSc] Connected to url: %s\n", payload);
			// send message to server when Connected
			webSocket.sendTXT("Connected");
		
			break;
		case WStype_TEXT: {
			USE_SERIAL.printf("[WSc] get text: %s\n", payload);
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
      res["type"] = "GPS";
      res["message"] = "GPS_data";
      res["latitude"] = lat_str;
      res["longitude"] = lot_str;
      String SendMsg = JSON.stringify(res);
			webSocket.sendTXT(SendMsg);
			break;
		}
		case WStype_BIN:
			USE_SERIAL.printf("[WSc] get binary length: %u\n", length);
			hexdump(payload, length);

			// send data to server
			// webSocket.sendBIN(payload, length);
			break;
  }

}

void setup() {
	// USE_SERIAL.begin(921600);
	USE_SERIAL.begin(115200);
  ss.begin(9600);

	//Serial.setDebugOutput(true);
	USE_SERIAL.setDebugOutput(true);

	USE_SERIAL.println();
	USE_SERIAL.println();
	USE_SERIAL.println();

	for(uint8_t t = 4; t > 0; t--) {
		USE_SERIAL.printf("[SETUP] BOOT WAIT %d...\n", t);
		USE_SERIAL.flush();
		delay(1000);
	}

	WiFiMulti.addAP(ssid, password);

	//WiFi.disconnect();
	while(WiFiMulti.run() != WL_CONNECTED) {
		delay(100);
	}

	// server address, port and URL
	webSocket.begin("192.168.0.9", 81, "/");

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
  SpeakerOn();
}

void loop() {
	//webSocket.loop();
 SpeakerOn();
 delay(5000);
 Setgps();
 Serial.print("lat : ");
 Serial.println(lat_str);
 Serial.print("lot : ");
 Serial.println(lot_str);
}
