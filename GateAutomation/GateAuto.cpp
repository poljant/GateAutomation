/*
 * GateAuto.cpp
 *
 *  Created on: 11 maj 2022
 *      Author: jant
 */

#include "GateAuto.h"
#include <Arduino.h>
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
	rel1B.begin(pin_gate1B);
	rel2A.begin(pin_gate2A);
	rel2B.begin(pin_gate2B);
	relw.begin(pin_wicket);
	relled.begin(pin_led, true); //reverse in testing
	myrc.enableReceive(pin_RF_Rx);
	myrc.enableTransmit(pin_RF_Tx);
	myrc.setPulseLength(400);
	//   buttondevice.begin(pin_button);
	//   buttondevice.setClickHandler(clickdev);

//	DEBUG_MSG_PROG("[GATE] Start begin()");
	addcodesrc();
}

void GateAuto::setcodes() {

}
void GateAuto::addcodesrc() {
	//ustal kody dla pierwszego pilota programowalnego
	//kody związane z ID chipa ESP
	unsigned long chipId = ESP.getChipId();
	unsigned long chipIdx = chipId << 8;
	unsigned long codekeyx = chipIdx & 0b00000011;
	buffercoderc[0].code = codekeyx;
	buffercoderc[0].nkey = 1; //key A

	codekeyx = chipIdx & 0b11000000;
	buffercoderc[1].code = codekeyx;
	buffercoderc[1].nkey = 2; //key B

	codekeyx = chipIdx & 0b00110000;
	buffercoderc[2].code = codekeyx;
	buffercoderc[2].nkey = 3;  //key C

	codekeyx = chipIdx & 0b00001100;
	buffercoderc[3].code = codekeyx;
	buffercoderc[3].nkey = 4;  //key D

//dodaj kody szczytane z pilota nieprogramowalnego
	buffercoderc[4].code = codekeyA1;
	buffercoderc[4].nkey = 1;
	buffercoderc[5].code = codekeyB1;
	buffercoderc[5].nkey = 2;
	buffercoderc[6].code = codekeyC1;
	buffercoderc[6].nkey = 3;
	buffercoderc[7].code = codekeyD1;
	buffercoderc[7].nkey = 4;

/*	buffercoderc[8].code = 0;
	buffercoderc[8].nkey = 1;
	buffercoderc[9].code = 0;
	buffercoderc[9].nkey = 2;
	buffercoderc[10].code = 0;
	buffercoderc[10].nkey = 3;
	buffercoderc[11].code = 0;
	buffercoderc[11].nkey = 4;*/
//	DEBUG_MSG_PROG("[GATE] Start addcodesrc() \n\r");
}
void GateAuto::opengate() {
// włącz LED
	relled.setOn();
//otwórz oba skrzydła bramy
	opengate2();
//	delay (time_gateB*1000);
	opengate1();
	currentstate = GATE_OPENING;
	DEBUG_MSG_PROG("[GATE] Start opengate() currentstate = %d \n\r", currentstate);
}
void GateAuto::closegate() {
// włącz LED
	relled.setOn();
//zamknij oba skrzydła bramy
	closegate1();
	delay(time_delay_gateB);
	closegate2();
	currentstate = GATE_CLOSING;
	DEBUG_MSG_PROG("[GATE] Start closegate() currentstate = %d \n\r", currentstate);
}
void GateAuto::opengate1() {
// włącz LED
	relled.setOn();
//otwórz pierwsze skrzydło bramy
	rel1A.setOn();
	rel1B.setOff();
//	currentstate = GATE_OPENING1;
//	DEBUG_MSG_PROG("[GATE] Start opengate1() currentstate = %d \n\r", currentstate);
}
void GateAuto::closegate1() {
// włącz LED
	relled.setOn();
//zamknij pierwsze skrzydło bramy
	rel1A.setOff();
	rel1B.setOn();
//	currentstate = GATE_CLOSING1;
//	DEBUG_MSG_PROG("[GATE] Start closegate1() currentstate = %d \n\r", currentstate);
}
void GateAuto::opengate2() {
// włącz LED
	relled.setOn();
//otwórz drugie skrzydło bramy
	rel2A.setOn();
	rel2B.setOff();
	if (nkey == 2){
	currentstate = GATE_OPENING2;
	DEBUG_MSG_PROG("[GATE] Start opengate2() currentstate = %d \n\r", currentstate);
	}
}
void GateAuto::closegate2() {
// włącz LED
	relled.setOn();
//zamknij drugie skrzydło bramy
	rel2A.setOff();
	rel2B.setOn();
	if (nkey == 2){
	currentstate = GATE_CLOSING2;
	DEBUG_MSG_PROG("[GATE] Start closegate2()currentstate = %d \n\r", currentstate);
}
}
bool GateAuto::bellon() {

	return false;
}
uint8_t GateAuto::stategate() {
	return currentstate;
}
uint8_t GateAuto::stategate1() {

	return currentstate1;

}
uint8_t GateAuto::stategate2() {
	return currentstate2;
}
bool GateAuto::statebutton() {
	return false;
}
void GateAuto::start() {
	DEBUG_MSG_PROG("[GATE] Start() \n\r");
}
void GateAuto::pause() {
	DEBUG_MSG_PROG("[GATE] Pause() \n\r");
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

unsigned long GateAuto::readcoderc() {
	ncodrc = myrc.getReceivedValue();
	myrc.resetAvailable();
	return ncodrc;
}
void GateAuto::readcodercx() {
	ncodrc = myrc.getReceivedValue();
	myrc.resetAvailable();
}
void GateAuto::sendcoderc(unsigned long code) {
	myrc.send(code, 24);
}
void GateAuto::sendcodeA() {
	sendcoderc(buffercoderc[0].code);
}
void GateAuto::sendcodeB() {
	sendcoderc(buffercoderc[1].code);
}
void GateAuto::sendcodeC() {
	sendcoderc(buffercoderc[2].code);
}
void GateAuto::sendcodeD() {
	sendcoderc(buffercoderc[3].code);;
}
bool GateAuto::addcoderc(unsigned long code, uint8_t key) {
	uint8_t i;
	for (i = 4; i <= howmanykeys; i++) {
		if (buffercoderc[i].code == 0) {
			buffercoderc[i].code = code;
			buffercoderc[i].nkey = key;
			return true;
		}
	}
	return false;
}
bool GateAuto::addcodercA() {
	uint8_t i;
	for (i = 4; i <= howmanykeys; i++) {
		if (buffercoderc[i].code == 0) {
			buffercoderc[i].code = codrc;
			buffercoderc[i].nkey = 1;
			return true;
		}
	}
	return false;
}
bool GateAuto::addcodercB() {
	uint8_t i;
	for (i = 4; i <= howmanykeys; i++) {
		if (buffercoderc[i].code == 0) {
			buffercoderc[i].code = codrc;
			buffercoderc[i].nkey = 2;
			return true;
		}
	}
	return false;
}
bool GateAuto::addcodercC() {
	uint8_t i;
	for (i = 4; i <= howmanykeys; i++) {
		if (buffercoderc[i].code == 0) {
			buffercoderc[i].code = codrc;
			buffercoderc[i].nkey = 3;
			return true;
		}
	}
	return false;
}
bool GateAuto::addcodercD() {
	uint8_t i;
	for (i = 4; i <= howmanykeys; i++) {
		if (buffercoderc[i].code == 0) {
			buffercoderc[i].code = codrc;
			buffercoderc[i].nkey = 4;
			return true;
		}
	}
	return false;
}
bool GateAuto::clearcodesrc() {
	//kasuje wszystkie kody rc oprócz 4 pierwszych
	uint8_t i;
	for (i = 4; i < howmanykeys; i++) {
		buffercoderc[i].code = 0;
		buffercoderc[i].nkey = 0;
	}
	return true;
}
bool GateAuto::clearcoderc(uint8_t ncode) {
	//kasuje kod na podanym miejscu. Nie można skasować 4 pierwszych
	if (ncode < 4 and ncode > howmanykeys)
		return false;
	buffercoderc[ncode].code = 0;
	buffercoderc[ncode].nkey = 0;
	return true;
}
unsigned long GateAuto::viewcoderc(uint8_t ncode) {
	return buffercoderc[ncode].code;
}
void GateAuto::openwicket() {
	DEBUG_MSG_PROG("[GATE] Start openwicket() opening\n\r");
	relw.setOn();
	delay(time_delay_wicket);
	relw.setOff();
	DEBUG_MSG_PROG("[GATE] Start openwicket() open\n\r");
}
void GateAuto::gateloop() {
	//czekaj aż minie czas od ostatniego naciśnięcia klawisza na pilocie
	if (time_read_code <= millis()) {
		readcodercx();
		if ((codrc != ncodrc) and (ncodrc != 0)){
		 codrc = ncodrc;
		 time_read_code = 0;
		 }else{
		 codrc = 0;
		 }

		if (codrc > 0) {
			nkeyx = serchcodes(codrc);
			DEBUG_MSG_PROG("[GATE_LOOP] Coderc %ld, nkey = %d \n\r", codrc,
					nkeyx);

//			codrc = 0;
			time_read_code = millis() + time_delay_read_code;
		}
	}

	switch (nkeyx) {
	case 1:
		nkeyx = 0;
		// gdy w czasie stop wcisnięto key A
		if (currentstate & GATE_STOP) {
			//skasuj bit stop
			currentstate = (currentstate and 0b01111111);
			//gdy stop był w czasie zamykania, to otwórz
			if (currentstate & GATE_CLOSING) {
				opengate();
				break;
			}
			//gdy stop był w czasie otwierania, to zamknij
			if (currentstate & GATE_OPENING) {
				closegate();
				break;
			}
			break;
		}
		// gdy brama zamknięta i wciśnięto key A to otwórz
		if (currentstate == GATE_CLOSE) {
			opengate();
			break;
		}
		//gdy brama otwarta i wciśnięto key A to zamknij
		if (currentstate & GATE_OPEN) {
			closegate();
			break;
		}

		// gdy w czasie zamykania wcisnięto key A
		if (currentstate & GATE_CLOSING) {
			stop();
			break;
		}
		// gdy w czasie otwierania wcisnięto key A
		if (currentstate & GATE_OPENING) {
			stop();
			break;
		}
		break;

	case 2:
		nkeyx = 0;
		// gdy w czasie stop wcisnięto key B
		if (currentstate & GATE_STOP) {
			//skasuj bit stop
			currentstate = (currentstate and 0b01111111);
			//				currentstate = currentstate2;
			//gdy stop był w czasie zamykania, to otwórz
			if (currentstate & GATE_CLOSING2) {
				opengate2();
				break;
			}
			//gdy stop był w czasie otwierania, to zamknij
			if (currentstate & GATE_OPENING2) {
				closegate2();
				break;
			}
			break;
		}
		// gdy brama zamknięta i wciśnięto key B to otwórz
		if (currentstate == GATE_CLOSE) {
			opengate2();
			break;
		}
		//gdy brama otwarta i wciśnięto key B to zamknij
		if (currentstate & GATE_OPEN) {
			closegate2();
			break;
		}

		// gdy w czasie zamykania wcisnięto key B
		if (currentstate & GATE_CLOSING2) {
			stop();
			break;
		}
		// gdy w czasie otwierania wcisnięto key B
		if (currentstate & GATE_OPENING2) {
			stop();
			break;
		}
		break;
	case 3:
		openwicket();
		nkeyx = 0;
		break;

	case 4:
		nkeyx = 0;
		break;

	default:
		nkeyx = 0;
		break;

	};
//	};
	if (!takes and (currentstate > 0) and (time_current < millis())) {
		time_current = millis() + time_gate;
//		gate_duration = time_current + time_led;
//		relled.setOn();
		takes = true;
	};
/*	if (gate_duration < millis()) {
		relled.setOff();
	};*/
	if (takes and (time_current < millis())) {
		takes = false;
		if (currentstate & GATE_STOP) {
			time_current = millis() + time_gate;
			//	currentstate =  GATE_OPEN;
		} else {
			rel1A.setOff();
			rel1B.setOff();
			rel2A.setOff();
			rel2B.setOff();

			if (currentstate == GATE_OPENING2) {
				//			currentstate = GATE_OPEN;
				//			currentstate2 = GATE_OPEN;
				DEBUG_MSG_PROG(
						"[GATE_LOOP] Bramam2 została otwarta.Currentstate = %d \n\r",
						currentstate);
				currentstate = GATE_OPEN;
				// wyłącz LED
					relled.setOff();
			}
			if (currentstate == GATE_CLOSING2) {
				//			currentstate = GATE_CLOSE;
				//		currentstate2 = GATE_CLOSE;
				DEBUG_MSG_PROG(
						"[GATE_LOOP] Bramam2 została zamknięta.Currentstate = %d \n\r",
						currentstate);
				currentstate = GATE_CLOSE;
				// wyłącz LED
					relled.setOff();
			}
			if (currentstate == GATE_OPENING) {
				//			currentstate = GATE_OPEN;
				DEBUG_MSG_PROG(
						"[GATE_LOOP] Bramam została otwarta.Currentstate = %d \n\r",
						currentstate);
				currentstate = GATE_OPEN;
				// wyłącz LED
					relled.setOff();
			}
			if (currentstate == GATE_CLOSING) {
				//			currentstate = GATE_CLOSE;
				DEBUG_MSG_PROG(
						"[GATE_LOOP] Bramam została zamknięta.Currentstate = %d \n\r",
						currentstate);
				currentstate = GATE_CLOSE;
				// wyłącz LED
					relled.setOff();
			}
			//		DEBUG_MSG_PROG("[GATE_LOOP] currentstate = %d \n\r", currentstate);
		}

	};

}
uint8_t GateAuto::serchcodes(unsigned long code) {
	//gdy brak kodu przerwij program
	if (code <= 0) {
		return 0;
	};
	//sprawdź dozwolone kody klawisza
	for (int i = 0; i < howmanykeys; i++) {
		codrcx = buffercoderc[i].code;
//	uint8_t	nk = buffercoderc[i].nkey;
//		DEBUG_MSG_PROG("[SERCHCODES-2] Coderc %ld, nkey = %d \n\r",codrcx, nkeyx);
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
const char GateAuto::byte2str(uint8_t b){
 char* byte[]="00000000";
 char bits = 0b10000000;
	if (b == 0xFF){
		for (uint8_t i = 0; i < 8; i++){
			bits = bits >> 1;
			byte[i] = (b and bits) & 0b110000; //chr(48)
		}
		return byte;
	} else {

	return byte;
	}
}
const char GateAuto::bin2str(unsigned long b){
	char* bin[] = "000000000000000000000000";
	bin = "";
	if (b == 0){
		return (bin);
	}else{
		if(b & 0xFF0000){
			bin += byte2str(b>>16);
		}
		if (b & 0x00FF00){
			bin += byte2str(b>>8);
		}
		if (b & 0x0000FF){
			bin += byte2str(b);
		}
	}
	return bin;
}

