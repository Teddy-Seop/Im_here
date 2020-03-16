/*
 * WebSocketClient.ino
 *
 *  Created on: 24.05.2015
 *
 */

#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <TinyGPS.h>
#include <SoftwareSerial.h> 
#include <WebSocketsClient.h>
#include <Arduino_JSON.h>

#include <Hash.h>

ESP8266WiFiMulti WiFiMulti;
WebSocketsClient webSocket;
JSONVar myObj;

#define USE_SERIAL Serial1

const char* ssid = "MICHELLE_2F_5G_";
const char* password = "michelle1100";
float lat, lot;
String lat_str, lot_str;

TinyGPS gps;
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

// getgps 함수는 우리가 원하는 값을 얻어서 출력
void getgps (TinyGPS & gps)
{
  // 모든 데이터를 코드에서 사용할 수있는 varialbes로 가져 오려면, 변수를 정의하고 객체를 핸들링 하면된다.
  // 데이터. 함수의 전체 목록을 보려면에서 keywords.txt 파일을 참조
  // TinyGPS와 NewSoftSerial 라이브러리.
  // 함수 호출
  gps.f_get_position (&lat, &lot);
  //경위도 출력 가능
  Serial.print("Lat/Long: ");
  Serial.print(lat,5);
  Serial.print(", ");
  Serial.println(lot,5);
  lat_str = String(lat, 5);
  lot_str = String(lot, 5);
  delay(10000);
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

void SendMessage() {
  // send message to server
  JSONVar res;
  res["device_num"] = "1";
  res["option"] = "GPS";
  res["message"] = "GPS_data";
  res["latitude"] = lat_str;
  res["longitude"] = lot_str;
  String SendMsg = JSON.stringify(res);
  webSocket.sendTXT(SendMsg);
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
      while (ss.available ()) // RX 핀에 데이터가있는 동안 ...
      {
          int c = ss.read (); // 데이터를 변수에로드 ...
          if (gps.encode (c)) // 새로운 유효한 문장이있는 경우 ...
          {
            getgps (gps); // 데이터를 가져온다.
          }
      }
      SendMessage();
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
  webSocket.begin("172.30.1.57", 6379, "/");

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
  SendMessage();
}

void loop() {
  webSocket.loop();
}
