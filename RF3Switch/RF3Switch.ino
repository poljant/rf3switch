#include "Arduino.h"
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPUpdateServer.h>
#include "Relay.h"
#include <EEPROM.h>
#include "RCSwitch.h"
#include "RFSwitch.h"
#include "RF3Switch.h"

/* Program umożliwia sterowanie trzema zwykłymi przekaźnikami
 * podłączonymi do pinów D1, D2 i D3.
 * Albo trzema przekaźnikami sterowanymi pilotem radiowym na 433MHz.
 * np. kupionych w Biedronce lub innym markecie.
 * Program umożliwia sczytywanie kodów załączania z pilota. Kod wyłączania
 * jest tworzony z kodu załączania.
 * Aby wczytać kod z pilota, to na pilocie wcisnąć przycisk On przekaźnika 1, 2 lub 3,
 * a następnie na stronie www wcisnąć przycisk
 * z odpowiednim numerem kodu (kodOn1, kodOn2 lub kodOn3).
 * Po sekundzie w wybranym polu będzie widoczny nowy kod.
 * Aby zapamiętać kody i stany położenia przekaźników wcisnij pole Zapisz zmiany.
 * Wtedy wszystkie potrzebne dane zapisywane są do pamięci EEPROM.
 * Odbiornik jest zasilany, tylko na czas odczytu kodu, z pinu D0.
 * Brak połączenia z siecią WiFi sygnalizowane jest ciągłym świeceniem wbudowanej diody LED.
 * Aktualizacja programu poprzez WiFi
 * http://IP/update
 * Licencja na zasadzie open source.
 * Program przeznaczony jest tylko do testów.
 */
//#define IP_STATIC
#define DEBUG

extern RCSwitch rcread;
Relay r1;
Relay r2;
Relay r3;
RFSwitch RFrelay1;
RFSwitch RFrelay2;
RFSwitch RFrelay3;

unsigned int lhx = 200;
unsigned int prot =1;

const char* ssid = "SSID";   // SSID twojej sieci WiFi
const char* password = "pass";  // password do WiFi
const int port = 80;                 // port serwera www

#ifdef IP_STATIC
IPAddress IPadr(10,110,2,113); //stały IP
IPAddress netmask(255,255,0,0);
IPAddress gateway(10,110,0,1);
#endif
extern ESP8266WebServer server;
extern void setservers();

//zmienne pomocnicze do współpracy z EEPROM
uint8_t EcodOn0[4];
uint8_t EcodOn1[4];
uint8_t EcodOn2[4];
uint8_t EcodOn3[4];
uint8_t Eval1[2];

uint8_t int2char(uint16_t liczba, uint8_t bytex){
	if (bytex == 'L'){
		return  (liczba & 0xFF); //zwraca młodszy bajt
	}else if (bytex == 'H') {
		return (liczba >> 8); //zwraca starszy bajt
	}
 return 0;
}
 uint16_t char2int(uint8_t Lbyte, uint8_t Hbyte){
return ((Hbyte << 8) + Lbyte);
}
uint32_t char2long(uint8_t * src ){
	uint32_t a = 0;
	for (uint8_t i = 0; i<4; i++){
	a = (a << 8);
	a += src[i];
	}
	return a;
}
void long2char(uint32_t liczba ){
	for (uint8_t i = 0; i<4; i++){
		EcodOn0[i] = (uint8_t)(liczba & 0xFF);
		liczba = (uint32_t) (liczba >> 8);
		}
	return ;
}

uint32_t codOnTocodOff(uint32_t cod){
	return cod+((((cod) & 3) > 1)? (9) : (3)); //oblicz kod wyłączenia
}

void readEEProm(void){
	int offset = 1;
	//kody przekaźników
	EcodOn1[0] = EEPROM.read(offset++);
	EcodOn1[1] = EEPROM.read(offset++);
	EcodOn1[2] = EEPROM.read(offset++);
	EcodOn1[3] = EEPROM.read(offset++);
	RFrelay1.setCodOn(char2long(EcodOn1));
	EcodOn2[0] = EEPROM.read(offset++);
	EcodOn2[1] = EEPROM.read(offset++);
	EcodOn2[2] = EEPROM.read(offset++);
	EcodOn2[3] = EEPROM.read(offset++);
	RFrelay2.setCodOn(char2long(EcodOn2));
	EcodOn3[0] = EEPROM.read(offset++);
	EcodOn3[1] = EEPROM.read(offset++);
	EcodOn3[2] = EEPROM.read(offset++);
	EcodOn3[3] = EEPROM.read(offset++);
	RFrelay3.setCodOn(char2long(EcodOn3));
	//lengthLH przekaźników
	Eval1[0] = EEPROM.read(offset++);
	Eval1[1] = EEPROM.read(offset++);
	RFrelay1.lengthLH = char2int(Eval1[1],Eval1[0]);
	Eval1[0] = EEPROM.read(offset++);
	Eval1[1] = EEPROM.read(offset++);
	RFrelay2.lengthLH = char2int(Eval1[1],Eval1[0]);
	Eval1[0] = EEPROM.read(offset++);
	Eval1[1] = EEPROM.read(offset++);
	RFrelay3.lengthLH = char2int(Eval1[1],Eval1[0]);
	//protokoły
	RFrelay1.protocol = EEPROM.read(offset++);
	RFrelay2.protocol = EEPROM.read(offset++);
	RFrelay3.protocol = EEPROM.read(offset++);
	//stany przekaźników
	//załącz lub wyłącz przkaźniki
	(EEPROM.read(offset++)==1)? r1.setOn(): r1.setOff();
	(EEPROM.read(offset++)==1)? r2.setOn(): r2.setOff();
	(EEPROM.read(offset++)==1)? r3.setOn(): r3.setOff();
	RFrelay1.writeRF(EEPROM.read(offset++));
	RFrelay2.writeRF(EEPROM.read(offset++));
	RFrelay3.writeRF(EEPROM.read(offset++));
	//ustaw kody Off
	RFrelay1.setCodOff(codOnTocodOff(RFrelay1.getCodOn()));
	RFrelay2.setCodOff(codOnTocodOff(RFrelay2.getCodOn()));
	RFrelay3.setCodOff(codOnTocodOff(RFrelay3.getCodOn()));
	//załącz lub wyłącz przekaźniki RF
	(RFrelay1.RF==1)? RFrelay1.sendOn() : RFrelay1.sendOff();
	(RFrelay2.RF==1)? RFrelay2.sendOn() : RFrelay2.sendOff();
	(RFrelay3.RF==1)? RFrelay3.sendOn() : RFrelay3.sendOff();
	return;
}
void saveEEProm(void){
	int offset = 1;
	//kody przekaźników
	long2char(RFrelay1.getCodOn());
	EEPROM.write(offset++,EcodOn0[3]);
	EEPROM.write(offset++,EcodOn0[2]);
	EEPROM.write(offset++,EcodOn0[1]);
	EEPROM.write(offset++,EcodOn0[0]);
	long2char(RFrelay2.getCodOn());
	EEPROM.write(offset++,EcodOn0[3]);
	EEPROM.write(offset++,EcodOn0[2]);
	EEPROM.write(offset++,EcodOn0[1]);
	EEPROM.write(offset++,EcodOn0[0]);
	long2char(RFrelay3.getCodOn());
	EEPROM.write(offset++,EcodOn0[3]);
	EEPROM.write(offset++,EcodOn0[2]);
	EEPROM.write(offset++,EcodOn0[1]);
	EEPROM.write(offset++,EcodOn0[0]);
	//lengthLH przekaźników
	EEPROM.write(offset++,int2char(RFrelay1.lengthLH,'H'));
	EEPROM.write(offset++,int2char(RFrelay1.lengthLH,'L'));
	EEPROM.write(offset++,int2char(RFrelay2.lengthLH,'H'));
	EEPROM.write(offset++,int2char(RFrelay2.lengthLH,'L'));
	EEPROM.write(offset++,int2char(RFrelay3.lengthLH,'H'));
	EEPROM.write(offset++,int2char(RFrelay3.lengthLH,'L'));
	//protokoły przekaźników
	EEPROM.write(offset++,RFrelay1.protocol);
	EEPROM.write(offset++,RFrelay2.protocol);
	EEPROM.write(offset++,RFrelay3.protocol);
	//stany przkaźników
	EEPROM.write(offset++,r1.read());
	EEPROM.write(offset++,r2.read());
	EEPROM.write(offset++,r3.read());
	//stany przekaźników RF
	EEPROM.write(offset++,RFrelay1.readRF());
	EEPROM.write(offset++,RFrelay2.readRF());
	EEPROM.write(offset++,RFrelay3.readRF());

	EEPROM.commit(); //zapisz pamięć EEPROM
	return;
}

void setup()
{

#ifdef DEBUG
  Serial.begin(115200);
#endif

	//inicjalizacja przekaźników
	r1.setPin(pinrelay1);
	r2.setPin(pinrelay2);
	r3.setPin(pinrelay3);
	r1.begin();
	r2.begin();
	r3.begin();
	rcread.enableTransmit(RFPINTX);

	EEPROM.begin(512);
//	saveEEProm();
	readEEProm(); //odczytaj dane z pamięci
		//załacz LED
	 pinMode(pinled0, OUTPUT);
  	 digitalWrite(pinled0,LOW);
	  // konfiguracja WiFi
	#ifdef IP_STATIC
	  WiFi.config(IPadr,gateway,netmask);  // stały IP
	#endif
	  WiFi.mode(WIFI_STA); //tryb STATION
	  WiFi.begin(ssid, password);
	  while (WiFi.status() != WL_CONNECTED) {  //  czekaj na połączenie z WiFi
	    delay(500);
	#ifdef DEBUG
	    Serial.print(".");
	#endif
	  }
	#ifdef DEBUG
	  Serial.println("");
	  Serial.println("WiFi połączone");
	  Serial.println(WiFi.localIP());
	  Serial.println(WiFi.macAddress());
	#endif
	  digitalWrite(pinled0,HIGH);// wyłącz LED gdy jest połączenie z WiFi

setservers();

}

void loop()
{
	 server.handleClient();   // czekaj na wywołanie strony www
	 delay(100);

	 if (WiFi.status() != WL_CONNECTED){
		 digitalWrite(pinled0,LOW);// załączenie LED wbudowanej gdy brak połączenia z WiFi
	 }
	 else{ // wyłącz LED gdy jest połączenie z WiFi
		 digitalWrite(pinled0,HIGH);//
	 }
}
