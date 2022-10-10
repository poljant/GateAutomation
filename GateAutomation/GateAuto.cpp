/*
 * GateAuto.cpp
 *
 *  Created on: 11 maj 2022
 *      Author: jant
 */

#include "GateAuto.h"
#include <Arduino.h>
//#include<bits/stdc++.h>
//#ifdef BUTTON2
//#include <Button2.h>
//#endif
#include "Relay.h"
#include <RCSwitch.h>
#include "../GateAutomation/debug.h"

#include "../GateAutomation/GateAuto.h"
Relay rel1A;
Relay rel1B;
Relay rel2A;
Relay rel2B;
Relay relw;
Relay relled;
RCSwitch myrc = RCSwitch();
//Button2 buttondevice = Button2();
GateAuto::GateAuto() {
	// TODO Auto-generated constructor stub
	begin();
}

GateAuto::~GateAuto() {
	// TODO Auto-generated destructor stub
}
void GateAuto::begin() {
	rel1A.begin(pin_gate1A);
	rel1A.setOff();
	rel1B.begin(pin_gate1B);
	rel1B.setOff();
	rel2A.begin(pin_gate2A);
	rel2A.setOff();
	rel2B.begin(pin_gate2B);
	rel2B.setOff();
	relw.begin(pin_wicket);
	relled.begin(pin_led); //reverse in testing
	relled.setOff();
	myrc.enableReceive(pin_RF_Rx);
	myrc.enableTransmit(pin_RF_Tx);
	myrc.setPulseLength(400);
	//   buttondevice.begin(pin_button);
	//   buttondevice.setClickHandler(clickdev);
	currentstate = GATE_CLOSE;
	DEBUG_MSG_PROG("[GATE] Start begin()");
	addcodesrc();
}

void GateAuto::setcodes() {

}
void GateAuto::addcodesrc() {
	//ustal kody dla pierwszego pilota programowalnego
	//kody związane z ID chipa ESP
	unsigned long chipId = ESP.getChipId();
	chipId = chipId & 0xFFFF;
//	DEBUG_MSG_PROG("[GATE] ChipID = %ld \n\r", chipId);
	uint32_t chipIdx = chipId << 8;
	chipId = chipId & 0xFFFF00;
//	DEBUG_MSG_PROG("[GATE] ChipIDx = %d \n\r", chipIdx);

	uint32_t codekeyx = chipIdx + 0b00000011;
//	DEBUG_MSG_PROG("[GATE] codekeyx = %d \n\r", codekeyx);
	buffercoderc[0].code = codekeyx;
	buffercoderc[0].nkey = 1; //key A
//	DEBUG_MSG_PROG("[GATE] Code A = %d \n\r", buffercoderc[0].code);
	codekeyx = chipIdx + 0b11000000;
	buffercoderc[1].code = codekeyx;
	buffercoderc[1].nkey = 2; //key B
//	DEBUG_MSG_PROG("[GATE] Code B = %d \n\r", buffercoderc[1].code);
	codekeyx = chipIdx + 0b00110000;
	buffercoderc[2].code = codekeyx;
	buffercoderc[2].nkey = 3;  //key C
//	DEBUG_MSG_PROG("[GATE] Code C = %d \n\r", buffercoderc[2].code);
	codekeyx = chipIdx + 0b00001100;
	buffercoderc[3].code = codekeyx;
	buffercoderc[3].nkey = 4;  //key D
//	DEBUG_MSG_PROG("[GATE] Code D = %d \n\r", buffercoderc[3].code);
//dodaj kody odczytane z pilota nieprogramowalnego
	buffercoderc[4].code = 6115587; //0b10111010101000100000011
	buffercoderc[4].nkey = 1;	//key A
	buffercoderc[5].code = 6115776; //0b10111010101000111000000
	buffercoderc[5].nkey = 2;	//key B
	buffercoderc[6].code = 6115632; //0b10111010101000100110000
	buffercoderc[6].nkey = 3;	//key C
	buffercoderc[7].code = 6115596; //0b10111010101000100001100
	buffercoderc[7].nkey = 4;	//key D
	//	kody pilota z przyciskami A i B
	buffercoderc[8].code = 201987;	// - 0b000000110001010100000011;
	buffercoderc[8].nkey = 1;	//key A
	buffercoderc[9].code = 202176;	// - 0b000000110001010111000000;
	buffercoderc[9].nkey = 2;	//key B
	buffercoderc[10].code = 4149251;	//0b1111110101000000000011
	buffercoderc[10].nkey = 1;
	buffercoderc[11].code = 4149440;	//0b1111110101000011000000
	buffercoderc[11].nkey = 2;
//	DEBUG_MSG_PROG("[GATE] Start addcodesrc() \n\r");
}


bool GateAuto::bellon() {

	return false;
}
uint8_t GateAuto::stategate() {
	return currentstate;
}
/*uint8_t GateAuto::stategate1() {

	return currentstate1;

}
uint8_t GateAuto::stategate2() {
	return currentstate2;
}*/
bool GateAuto::statebutton() {
	return false;
}
/*void GateAuto::start() {
	DEBUG_MSG_PROG("[GATE] Start() \n\r");
}
void GateAuto::pause() {
	DEBUG_MSG_PROG("[GATE] Pause() \n\r");
}*/


uint32_t GateAuto::readcoderc() {
	service = true;
	ncodrc = myrc.getReceivedValue();
	myrc.resetAvailable();
//	service = false;
	return ncodrc;
}
void GateAuto::readcodercx() {
	ncodrc = myrc.getReceivedValue();
	myrc.resetAvailable();
}
void GateAuto::sendcoderc(uint32_t code) {
	service = true;
	myrc.send(code, 24);
	service = false;
}
void GateAuto::sendcodeA() {
	service = true;
	sendcoderc(buffercoderc[0].code);
	service = false;
}
void GateAuto::sendcodeB() {
	service = true;
	sendcoderc(buffercoderc[1].code);
	service = false;
}
void GateAuto::sendcodeC() {
	service = true;
	sendcoderc(buffercoderc[2].code);
	service = false;
}
void GateAuto::sendcodeD() {
	service = true;
	sendcoderc(buffercoderc[3].code);
	service = false;
}
unsigned long GateAuto::addduration(int duration){
	return millis() + duration;
}
bool GateAuto::iscode(uint32_t cod){
	if (cod <=0) return false;
	uint8_t i;
	for (i = 4; i <= HOWMANYKEYS; i++) {
		if (buffercoderc[i].code == cod) {
			//DEBUG_MSG_PROG("[GATE] Is code number %d \n\r", cod);
			DEBUG_MSG_PROG("[GATE] Kod o tym numerze już jest %d \n\r", cod);
			return true;
		}
	}
	return false;
}
unsigned long GateAuto::adddurationsek(int duration){

	return millis() + duration*1000;
}
bool GateAuto::addcoderc(uint32_t code, uint8_t key) {
	ncodrc=0;
	if (iscode(code)){
		//DEBUG_MSG_PROG("[GATE] such a code number is already there. code = %d \n\r", code);
		DEBUG_MSG_PROG("[GATE] Taki kod jest już zapisany. code = %d \n\r", codrc);

		return false;
	}
	uint8_t i;
	for (i = 4; i <= HOWMANYKEYS; i++) {
		if (buffercoderc[i].code == 0) {
			buffercoderc[i].code = code;
			buffercoderc[i].nkey = key;
			//DEBUG_MSG_PROG("[GATE] adding code number %d = %d \n\r", i, code);
			DEBUG_MSG_PROG("[GATE] Dodano kod pod numerem %d = %d \n\r", i, code);
			return true;
		}
	}
	//DEBUG_MSG_PROG("[GATE] Error adding code number %d = %d\n\r", i, code);
	DEBUG_MSG_PROG("[GATE] Błąd dodania kodu numer %d = %d\n\r", i, code);
	return false;
}
bool GateAuto::addcodercA() {
	ncodrc=0;
	if (iscode(codrc)){
//		DEBUG_MSG_PROG("[GATE] such a code number is already there. code = %d \n\r", codrc);
		DEBUG_MSG_PROG("[GATE] Taki kod jest już zapisany. code = %d \n\r", codrc);
		return false;
	}
	uint8_t i;
	for (i = 4; i <= HOWMANYKEYS; i++) {
		if (buffercoderc[i].code == 0) {
			buffercoderc[i].code = codrc;
			buffercoderc[i].nkey = 1;
			codrc=0;
			return true;
		}
	}
	return false;
}
bool GateAuto::addcodercB() {
	ncodrc=0;
	if (iscode(codrc)){
//		DEBUG_MSG_PROG("[GATE] such a code number is already there. code = %d \n\r", codrc);
		DEBUG_MSG_PROG("[GATE] Taki kod jest już zapisany. code = %d \n\r", codrc);
		return false;
	}
	uint8_t i;
	for (i = 4; i <= HOWMANYKEYS; i++) {
		if (buffercoderc[i].code == 0) {
			buffercoderc[i].code = codrc;
			buffercoderc[i].nkey = 2;
			codrc=0;
			return true;
		}
	}
	return false;
}
bool GateAuto::addcodercC() {
	ncodrc=0;
	if (iscode(codrc)){
//		DEBUG_MSG_PROG("[GATE] such a code number is already there. code = %d \n\r", codrc);
		DEBUG_MSG_PROG("[GATE] Taki kod jest już zapisany. code = %d \n\r", codrc);
		return false;
	}
	uint8_t i;
	for (i = 4; i <= HOWMANYKEYS; i++) {
		if (buffercoderc[i].code == 0) {
			buffercoderc[i].code = codrc;
			buffercoderc[i].nkey = 3;
			codrc=0;
			return true;
		}
	}
	return false;
}
bool GateAuto::addcodercD() {
	ncodrc=0;
	if (iscode(codrc)){
//		DEBUG_MSG_PROG("[GATE] such a code number is already there. code = %d \n\r", codrc);
		DEBUG_MSG_PROG("[GATE] Taki kod jest już zapisany. code = %d \n\r", codrc);
		return false;
	}
	uint8_t i;
	for (i = 4; i <= HOWMANYKEYS; i++) {
		if (buffercoderc[i].code == 0) {
			buffercoderc[i].code = codrc;
			buffercoderc[i].nkey = 4;
			codrc=0;
			return true;
		}
	}
	return false;
}
bool GateAuto::clearcodesrc() {
	//kasuje wszystkie kody rc oprócz 4 pierwszych
	uint8_t i;
	for (i = 4; i < HOWMANYKEYS; i++) {
		buffercoderc[i].code = 0;
		buffercoderc[i].nkey = 0;
	}
	return true;
}
bool GateAuto::clearcoderc(uint8_t ncode) {
	//kasuje kod na podanym miejscu. Nie można skasować 4 pierwszych
	if (ncode < 4 and ncode > HOWMANYKEYS)
		return false;
	buffercoderc[ncode].code = 0;
	buffercoderc[ncode].nkey = 0;
	return true;
}
uint32_t GateAuto::viewcoderc(uint8_t ncode) {
	return buffercoderc[ncode].code;
}
void GateAuto::openwicket() {
	DEBUG_MSG_PROG("[GATE] Start openwicket() opening\n\r");
	relw.setOn();
	delay(duration_on_wicket);
	relw.setOff();
//	DEBUG_MSG_PROG("[GATE] Start openwicket() open\n\r");
}
void GateAuto::stop() {
	//wyłącz przekaźniki
	rel1A.setOff();
	rel1B.setOff();
	rel2A.setOff();
	rel2B.setOff();
	//ustaw bit stopu
	currentstate = currentstate | GATE_STOP;
	DEBUG_MSG_PROG("[GATE] Stop() currentstate = %d \n\r", currentstate);
}
void GateAuto::opengate1() {
// włącz LED
	ledOn = true;
	relled.setOn();
//otwórz pierwsze skrzydło bramy
	rel1A.setOn();
	rel1B.setOff();
//	currentstate = GATE_OPENING1;
//	DEBUG_MSG_PROG("[GATE] Start opengate1() currentstate = %d \n\r", currentstate);
}
void GateAuto::closegate1() {
// włącz LED
	ledOn = true;
	relled.setOn();
//zamknij pierwsze skrzydło bramy
	rel1A.setOff();
	rel1B.setOn();
//	currentstate = GATE_CLOSING1;
//	DEBUG_MSG_PROG("[GATE] Start closegate1() currentstate = %d \n\r", currentstate);
}
void GateAuto::opengate2() {
//	time_current = addduration(gate_duration); //	millis() + gate_duration;
	time_current = 0;
// włącz LED
	ledOn = true;
	relled.setOn();
//otwórz drugie skrzydło bramy
	rel2A.setOn();
	rel2B.setOff();
	if (nkey == 2){
	currentstate = GATE_OPENING2;

	}
	DEBUG_MSG_PROG("[GATE] Start opengate2() currentstate = %d \n\r", currentstate);
}
void GateAuto::closegate2() {
//	time_current = addduration(gate_duration); //	millis() + gate_duration;
// włącz LED
	ledOn = true;
	relled.setOn();
//zamknij drugie skrzydło bramy
	rel2A.setOff();
	rel2B.setOn();
	if (nkey == 2){
	currentstate = GATE_CLOSING2;

	}
	DEBUG_MSG_PROG("[GATE] Start closegate2() currentstate = %d \n\r", currentstate);
}
void GateAuto::opengate() {
//	time_current = addduration(gate_duration); //	millis() + gate_duration;
	time_current = 0;
// włącz LED
	ledOn = true;
	relled.setOn();
//otwórz oba skrzydła bramy
	currentstate = GATE_OPENING;
	opengate2();
	delay (time_delay_gateB);
	opengate1();

	DEBUG_MSG_PROG("[GATE] Start opengate() currentstate = %d \n\r", currentstate);
}
void GateAuto::closegate() {
//	time_current = addduration(gate_duration); //	millis() + gate_duration;
	time_current = 0;
// włącz LED
	ledOn = true;
	relled.setOn();
//zamknij oba skrzydła bramy
	currentstate = GATE_CLOSING;
	closegate1();
	delay(time_delay_gateB);
	closegate2();

	DEBUG_MSG_PROG("[GATE] Start closegate() currentstate = %d \n\r", currentstate);
}
void GateAuto::gateloop() {
	if (service) return;  // gdy programowanie lub inne prace serwisowe, to wyłącz pętle gateloop()
	//kontrola pulsowania leda sygnalizującego działanie bramy
	if (ledOn){
		led_blink(led_long_pulse);
	}else{ //jeśli led ma być wyłączony to sprawdź czy wyłączony
			// jeśli nie to wyłącz
		if(relled.read()) relled.setOff();
	}
	//czekaj aż minie czas od ostatniego naciśnięcia klawisza na pilocie
	if (time_read_code <= millis()) {
		readcodercx(); //czytaj kod
		if ((codrc != ncodrc) and (ncodrc != 0)){
		 codrc = ncodrc;
		 time_read_code = 0;
		 }else{
		 codrc = 0;
		 }
		//gdy kod odczytano
		if (codrc > 0) {
			nkeyx = serchcodes(codrc); //sprawdź czy kod dozwolony
			DEBUG_MSG_PROG("[GATE_LOOP] Coderc %d, nkey = %d  \n\r", codrc,
					nkeyx);
			//ustaw opuźnienie do czytania kolejnego kodu
			time_read_code =  millis() + time_delay_read_nex_code;
		}
	}

	//przetwarzaj klawisz wciśnięty
	switch (nkeyx) {
	case 1:		//gdy klawisz A
		nkeyx = 0;
		// gdy w czasie stop wcisnięto key A
		if (currentstate & GATE_STOP) {
			//skasuj bit stop
			currentstate = (currentstate ^ GATE_STOP); //& 0b01111111);
			//gdy stop był w czasie otwierania, to zamknij
			if (currentstate & GATE_OPENING) {
				closegate();
				break;
			} else{
				//gdy stop był w czasie zamykania, to otwórz
				if (currentstate & GATE_CLOSING) {
						opengate();
						break;
					}
					break;
			}
		}

		// gdy brama zamknięta i wciśnięto key A to otwórz
		if (currentstate == GATE_CLOSE) {
			opengate();
			break;
		}
		//gdy brama otwarta i wciśnięto key A to zamknij
		if (currentstate == GATE_OPEN) {
			closegate();
			break;
		}

		// gdy w czasie zamykania wcisnięto key A to wstrzymaj zamykanie
		if (currentstate == GATE_CLOSING) {
			stop();
			time_current += gate_duration;
			break;
		}
		// gdy w czasie otwierania wcisnięto key A to wstrzymaj otwieranie
		if (currentstate == GATE_OPENING) {
			stop();
			time_current += gate_duration;
			break;
		}
		break;

	case 2:		//gdy klawisz B
		nkeyx = 0;
		// gdy w czasie stop wcisnięto key B
		if (currentstate & GATE_STOP) {
			//skasuj bit stop
			currentstate = (currentstate ^ GATE_STOP);
			//gdy stop był w czasie zamykania, to otwórz
			if (currentstate == GATE_CLOSING2) {
				currentstate = GATE_OPENING2;
				opengate2();
				break;
			} else {
				//gdy stop był w czasie otwierania, to zamknij
				if (currentstate == GATE_OPENING2) {
					currentstate = GATE_CLOSING2;
					closegate2();
					break;
				}
				break;
			}
		}
		//DEBUG_MSG_PROG(	"[GATE_LOOP] Wciśnięto klawisz B. Currentstate = %d \n\r",
		//						currentstate);
		// gdy brama zamknięta i wciśnięto key B to otwórz
		if (currentstate == GATE_CLOSE) {
			currentstate = GATE_OPENING2;
			opengate2();
			break;
		}
		//gdy brama otwarta i wciśnięto key B to zamknij
		if (currentstate == GATE_OPEN2) {
			currentstate = GATE_CLOSING2;
			closegate2();
			break;
		}

		// gdy w czasie zamykania wcisnięto key B to wstrzymaj zamykanie
		if (currentstate == GATE_CLOSING2) {
			stop();
			time_current += gate_duration;  //przedłuż czas świecenia LED
			break;
		}
		// gdy w czasie otwierania wcisnięto key B to wstrzymaj otwieranie
		if (currentstate == GATE_OPENING2) {
			stop();
			time_current += gate_duration;	//przedłuż czas świecenia LED
			break;
		}
		break;
	case 3:		//gdy klawisz C
		//gdy wciśnięto klawisz C to otwórz zamek furtki
		openwicket();
		nkeyx = 0;
		break;

	case 4:		//gdy klawisz D
		//załącz lub wyłącz autoclose
		if (autoclose){
			autoclose= false;
			DEBUG_MSG_PROG(	"[GATE_LOOP] Autoclose = false\n\r");
		}else{
			autoclose= true;
			DEBUG_MSG_PROG(	"[GATE_LOOP] Autoclose = true\n\r");
		}

		nkeyx = 0;
		break;

	default: 	//gdy klawisz nieokreślony
		nkeyx = 0;
		break;

	};
	//gdy nie trwa zamykanie lub otwieranie bramy (bit GATE_TIMERS = 0) i zmienił się status bieżący bramy
	// ustawiony bit GATE_OPENING2 lub GATE_OPENING
	// to ustal czas trwania działania bramy (time_current + bieżący czas + gate_duration) i ustaw, że trwa działanie bramy (bit GATE_TIMER na 1)
	if(!(currentstate & GATE_TIMER) and (currentstate & (GATE_OPENING | GATE_CLOSING))){
		time_current = addduration(gate_duration); //millis() + gate_duration;
		//ustaw bit timera
		currentstate = currentstate | GATE_TIMER;
	};
	//gdy trwa działanie bramy, to sprawdź czy czas minął
	if ((currentstate & GATE_TIMER) and (time_current < millis())) {
		//jeśli czas minoł to wyłącz bit timera
		currentstate = currentstate ^ GATE_TIMER;
		//gdy trwa stan stop
		if (currentstate & GATE_STOP) {
			time_current = addduration(gate_duration); //millis() + gate_duration; //zwiększ czas trwania
		} else { //jeśli nie to wyłącz przekaźniki bramy
			rel1A.setOff();
			rel1B.setOff();
			rel2A.setOff();
			rel2B.setOff();
			//gdy brama 2 była otwierana, to zaznacz, że brama 2 została otwarta
			if (currentstate == GATE_OPENING2) {
				currentstate = GATE_OPEN2;
				DEBUG_MSG_PROG(
						"[GATE_LOOP] Bramam2 została otwarta. Currentstate = %d \n\r",
						currentstate);
			}
			//gdy brama 2 była zamykana, to zaznacz, że brama 2 została zamknięta
			if (currentstate == GATE_CLOSING2) {
				currentstate = GATE_CLOSE;
				DEBUG_MSG_PROG(
						"[GATE_LOOP] Bramam2 została zamknięta. Currentstate = %d \n\r",
						currentstate);
			}
			//gdy brama była otwierana, zaznacz, że brama została otwarta
			if (currentstate == GATE_OPENING) {
				currentstate = GATE_OPEN;
				DEBUG_MSG_PROG(
						"[GATE_LOOP] Bramam została otwarta. Currentstate = %d \n\r",
						currentstate);
			}
			//gdy brama była zamykana, to zaznacz, że brama została zamknięta
			if (currentstate == GATE_CLOSING) {
				currentstate = GATE_CLOSE;
				DEBUG_MSG_PROG(
						"[GATE_LOOP] Bramam została zamknięta. Currentstate = %d \n\r",
						currentstate);
			}
			// wyłącz LED
			ledOn = false;
			relled.setOff();
		}

	};
	//gdy ustawiono automatyczne zamykanie otwartej bramy i brama jest otwarta
	if (autoclose and (currentstate & GATE_OPEN )){

		//gdy nie trwa autozamykanie ustal czas trwania otwartej bramy
		//zaznacz, że zaczęto odliczać czas otwarcia
		if(curent_time_autoclose < millis() and  !takes_autoclose){
			curent_time_autoclose = adddurationsek(time_autoclose);
			takes_autoclose = true;
			DEBUG_MSG_PROG(	"[GATE_LOOP] Rozpoczął się czas liczenia do zamknięcia bramy\n\r");
		}
		//gdy zakończył się czas otwarcia bramy
		if(curent_time_autoclose < millis() and  takes_autoclose){
			//wyłącz trwanie czasu otwarcia bramy
			takes_autoclose = false;
			DEBUG_MSG_PROG(	"[GATE_LOOP] Zakończył się czas liczenia do zamknięcia bramy\n\r");
			//uruchom zamykanie bramy
			if (currentstate == GATE_OPEN2){
				currentstate = GATE_CLOSING2;
				closegate2();
			}
			if (currentstate == GATE_OPEN){
				currentstate = GATE_CLOSING;
				closegate();
						}
		}

	}

}
uint8_t GateAuto::serchcodes(uint32_t code) {
	//gdy brak kodu przerwij program
	if (code <= 0) {
		return 0;
	};
	//sprawdź dozwolone kody klawisza
	for (int i = 0; i < HOWMANYKEYS; i++) {
		codrcx = buffercoderc[i].code;
		if (codrcx == code) {
			uint8_t	nk = buffercoderc[i].nkey;
			nkey = nk;
		//	DEBUG_MSG_PROG("[SERCHCODES-2] Coderc %ld, nkey = %d \n\r", codrcx, nkeyx);
			return nk;
		}
	}
	// gdy nie znaleziono kodu
	return 0;
}
void GateAuto::led_blink(int tp){
	if (!led_pulse) return;
	if(time_blink < millis()){
		time_blink = millis()+tp;
		relled.read() ? relled.setOff() : relled.setOn();
	}
}
