#include <SoftwareSerial.h>
#include <TinyGPS.h>
 
// Arduino에서 사용할 핀을 정의하여 선언
#define RXPIN 4
#define TXPIN 5
//이 값을 GPS의 보오율과 동일하게 설정(9600)
#define GPSBAUD 9600
 
// TinyGPS 객체의 인스턴스를 생성 -> TinyGPS 라이브러리 패키지를 다운로드 받아야 함!
TinyGPS gps;
// 위에서 정의한 핀으로 NewSoftSerial 라이브러리를 초기화한다.
SoftwareSerial uart_gps (RXPIN, TXPIN);
 
// 여기서 함수의 프로토 타입을 선언
// TinyGPS 라이브러리를 사용
void getgps (TinyGPS & gps);
 
// setup 함수에서 두 개의 직렬 포트를 초기화
// 표준 하드웨어 직렬 포트 (Serial ())를 사용하여 수신.
void setup ()
{
  Serial.begin (9600);
  uart_gps.begin (GPSBAUD);
  
  Serial.println ( "");
  Serial.println ( "... 수신 대기 중 ...");
  Serial.println ( "");
}
 
// 코드의 메인 루프, 단지 데이터가 유효한지 확인하는 것뿐!
// ardiuno의 RX 핀은 데이터가 유효한 NMEA 문장인지 확인
// getgps () 함수로 실제 데이터 출력
void loop ()
{
  while (uart_gps.available ()) // RX 핀에 데이터가있는 동안 ...
  {
      int c = uart_gps.read (); // 데이터를 변수에로드 ...
      if (gps.encode (c)) // 새로운 유효한 문장이있는 경우 ...
      {
        getgps (gps); // 데이터를 가져온다.
      }
  }
}
 
// getgps 함수는 우리가 원하는 값을 얻어서 출력
void getgps (TinyGPS & gps)
{
  // 모든 데이터를 코드에서 사용할 수있는 varialbes로 가져 오려면, 변수를 정의하고 객체를 핸들링 하면된다.
  // 데이터. 함수의 전체 목록을 보려면에서 keywords.txt 파일을 참조
  // TinyGPS와 NewSoftSerial 라이브러리.
  // 경, 위도 변수를 정의
    float latitude, longitude;
  // 함수 호출
  gps.f_get_position (&latitude, &longitude);
  //경위도 출력 가능
  Serial.print("Lat/Long: ");
  Serial.print(latitude,6);
  Serial.print(", ");
  Serial.println(longitude,6);
  
 delay (10000);
} //완성본
