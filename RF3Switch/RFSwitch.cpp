/*
 * RFSwitch.cpp
 *
 *  Created on: 20.02.2017
 *      Author: Jan Trzciński
 */
#include <RCSwitch.h>
#include "RFSwitch.h"
#include "RF3Switch.h"

//attachInterrupt(digitalPinToInterrupt(PIN), FUNKCJA, REAKCJA_NA);

RCSwitch rcread;

RFSwitch::RFSwitch() {

	ncodOn = 0;
	ncodOff = 0;
	lengthLH =200;
	protocol = 1;
	RF = 0;
}

RFSwitch::~RFSwitch() {
	// TODO Auto-generated destructor stub
}

void RFSwitch::begin( uint8_t pTx){

	rcread.enableTransmit(pTx);

}
void RFSwitch::setCodOn(uint32_t cod){
	ncodOn = cod;
}

void RFSwitch::setCodOff(uint32_t cod){
	ncodOff = cod;
}

void RFSwitch::sendOff(void){
	//ustaw rodzaj protokołu i długość ilmulsu HIGH
	 rcread.setProtocol(protocol,lengthLH);
	 rcread.send(ncodOff,24);
	 RF=0;
}

void RFSwitch::sendOn(void){
	//ustaw rodzaj protokołu i długość ilmulsu HIGH
	 rcread.setProtocol(protocol,lengthLH);
	 rcread.send(ncodOn,24);
	 RF=1;
}

//odczytaj czy RF załaczony
uint8_t RFSwitch::readRF(){
	return RF;
}

//ustaw stan RF
void RFSwitch::writeRF(uint8_t rf){
	 RF=rf;
}

void RFSwitch::readCod(uint8_t pinRx){
	pinMode(RFONRX, OUTPUT);
	digitalWrite(RFONRX, HIGH); //załącz zasilanie odbiornika
	rcread.resetAvailable();
	rcread.enableReceive(digitalPinToInterrupt(pinRx)); // włacz program przerwania na pinRx
	uint32_t timeread = millis()+1000;
	  // czekaj max 1 sekundę na pobranie kodu
	  while (timeread > millis()){

		if (rcread.available()){
			ncodOn = (rcread.getReceivedValue());//ustaw pobrany kod załączenia
			ncodOff = ncodOn+((((ncodOn) & 3) > 1)? (9) : (3)); //oblicz kod wyłączenia
			//ustaw odczytany protokół i długość trwania impulsu HIGH
			lengthLH = rcread.getReceivedDelay();
			protocol = rcread.getReceivedProtocol();
			rcread.setProtocol(protocol,lengthLH);
			timeread = 0;

		}
	  }
	rcread.disableReceive(); // wyłącz program przerwania na pinRx
	digitalWrite(RFONRX, LOW); //wyłącz zasilanie odbiornika
	return;
}
uint32_t RFSwitch::getCodOn(void){
	return (ncodOn);

}
uint32_t RFSwitch::getCodOff(void){
	return (ncodOff);

}
