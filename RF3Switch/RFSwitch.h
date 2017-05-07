/*
 * RFSwitch.h
 *
 *  Created on: 20.02.2017
 *      Author: Jan Trzciński
 *
 *      Klasa RFSwitch umożliwia nadawanie kodów załączenia i wyłączenia przekaźników
 *      sterowanych radiem na 433MHz lub 866MHz.
 *      Układ ten stosowany jest w tanich i popularnych wyłacznikach 220V.
 *      Do uruchomienia układu potrzebny jest nadajnik RF na 433MHz lub 866MHz
 */

#ifndef RFSWITCH_H_
#define RFSWITCH_H_
#include <RCSwitch.h>

class RFSwitch  {
	uint32_t ncodOn;
	uint32_t ncodOff;
public:
	uint16_t lengthLH;
	uint8_t protocol;
	uint8_t RF;

public:
	RFSwitch();
	virtual ~RFSwitch();

	void begin(uint8_t); //rozpocznij i ustaw pint Tx
	void sendOn(void); // wyslij kod załączenia
	void sendOff(void); // wyslij kod wyłączenia
	uint8_t readRF(void); // czytaj stan wyłącznika RF
	void writeRF(uint8_t); // zapisz stan RF
	void setCodOn(uint32_t); //ustaw kod załączenia
	void setCodOff(uint32_t); // ustaw kod wyłączenia
	void setRF(uint8_t, uint8_t, uint8_t); //ustaw parametry wyłącznika RF
	void readCod(uint8_t); //czyta kod z odbiornika podłączonego do podanego pinu
	void readCod(void); //czyta kod z odbiornika podłaczonego do pinu RFpinRx
	uint32_t getCodOn(void); //czytaj kod On przekaźnika RF
	uint32_t getCodOff(void); //czytaj kod Off przekaźnika RF
};


#endif /* RFSWITCH_H_ */
