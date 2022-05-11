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
Relay relcon;
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
    relcon.begin(pin_con);
    myrc.enableReceive(pin_RF_Rx);
    myrc.enableTransmit(pin_RF_Tx);
    myrc.setPulseLength(400);
 //   buttondevice.begin(pin_button);
 //   buttondevice.setClickHandler(clickdev);
    //ustal kody dla pierwszego pilota programowalnego
    unsigned long chipId = ESP.getChipId();
    unsigned long chipIdx = chipId << 8;
    unsigned long codekeyA = chipIdx & 0b00000011;
    buffercoderc[0].code = codekeyA;
    buffercoderc[0].nkey = 1;

    unsigned long codekeyB = chipIdx & 0b11000000;
    buffercoderc[1].code = codekeyB;
    buffercoderc[1].nkey = 2;

    unsigned long codekeyC = chipIdx & 0b00110000;
    buffercoderc[2].code = codekeyC;
    buffercoderc[2].nkey = 3;

    unsigned long codekeyD = chipIdx & 0b00001100;
    buffercoderc[3].code = codekeyD;
    buffercoderc[3].nkey = 4;


}

void GateAuto::setcodes(){

}
void GateAuto::opengate(){
//otwórz oba skrzydła bramy
	opengate2();
//	delay (time_gateB*1000);
	opengate1();
	currentstate = GATE_OPENING1 | GATE_OPENING;
}
void GateAuto::closegate(){
//zamknij oba skrzydła bramy
	closegate1();
	delay (time_delay_gateB);
	closegate2();
	currentstate = GATE_CLOSING1 | GATE_CLOSING;
}
void GateAuto::opengate1(){
//otwórz pierwsze skrzydło bramy
	rel1A.setOn();
	rel1B.setOff();
 //   openinggate1 = true;
    currentstate1 = GATE_OPENING1;
}
void GateAuto::closegate1(){
//zamknij pierwsze skrzydło bramy
	rel1A.setOff();
	rel1B.setOn();
 //   closinggate1 = true;
    currentstate1 = GATE_CLOSING1;
}
void GateAuto::opengate2(){
//otwórz drugie skrzydło bramy
	rel2A.setOn();
	rel2B.setOff();
//    openinggate2 = true;
    currentstate2 = GATE_OPENING2;
}
void GateAuto::closegate2(){
//zamknij drugie skrzydło bramy
	rel2A.setOff();
	rel2B.setOn();
//    closinggate2 = true;
    currentstate2 = GATE_CLOSING2;
}
bool GateAuto::bellon(){
	return false;
}
uint8_t GateAuto::stategate(){
	return currentstate;
}
uint8_t GateAuto::stategate1(){

	return currentstate1;

}
uint8_t GateAuto::stategate2(){
	return currentstate2;
}
bool GateAuto::statebutton(){
	return false;
}
void GateAuto::start(){

}
void GateAuto::pause(){

}
void GateAuto::stop(){

}
unsigned long  GateAuto::readcoderc(){
	unsigned long ncodrc = myrc.getReceivedValue();
	myrc.resetAvailable();
	return ncodrc;
}
void GateAuto::sendcoderc(unsigned long code){
	myrc.send(code, 24);
}
void GateAuto::sendcodeA(){
	sendcoderc(codekeyA);
}
void GateAuto::sendcodeB(){
	sendcoderc(codekeyB);
}
void GateAuto::sendcodeC(){
	sendcoderc(codekeyC);
}
void GateAuto::sendcodeD(){
	sendcoderc(codekeyD);
}
/*bool GateAutomation::addcoderc(){
	uint8_t i;
	for (i=1 ; i<= howmanykeys;i++){
		if (buffercoderc[i]==0) {
			buffercoderc[i].code = codrc;
			return true;
		}
	}
	return false;
}*/
bool GateAuto::addcodercA(){
	uint8_t i;
	for (i=1 ; i<= howmanykeys;i++){
		if (buffercoderc[i].code==0) {
			buffercoderc[i].code = codrc;
			buffercoderc[i].nkey = 1;
			return true;
		}
	}
	return false;
}
bool GateAuto::addcodercB(){
	uint8_t i;
	for (i=1 ; i<= howmanykeys;i++){
		if (buffercoderc[i].code==0) {
					buffercoderc[i].code = codrc;
					buffercoderc[i].nkey = 2;
			return true;
		}
	}
	return false;
}
bool GateAuto::addcodercC(){
	uint8_t i;
	for (i=1 ; i<= howmanykeys;i++){
		if (buffercoderc[i].code==0) {
					buffercoderc[i].code = codrc;
					buffercoderc[i].nkey = 3;
			return true;
		}
	}
	return false;
}
bool GateAuto::addcodercD(){
	uint8_t i;
	for (i=1 ; i<= howmanykeys;i++){
		if (buffercoderc[i].code==0) {
					buffercoderc[i].code = codrc;
					buffercoderc[i].nkey = 4;
			return true;
		}
	}
	return false;
}
bool GateAuto::clearcoderc(){
	uint8_t i;
	for (i=1 ; i<howmanykeys ; i++) {
		buffercoderc[i].code = 0;
		buffercoderc[i].nkey = 0;
	}
	return true;
}
bool GateAuto::clearcoderc(uint8_t ncode){
	if (ncode <= 0 and ncode > howmanykeys) return false;
		buffercoderc[ncode].code = 0;
		buffercoderc[ncode].nkey = 0;
	return true;
}
unsigned long GateAuto::viewcoderc(uint8_t ncode){
	return buffercoderc[ncode].code;
}
void GateAuto::openwicket(){
	relw.setOn();
	delay(time_delay_wicket);
	relw.setOff();
}
void GateAuto::gateloop(){
	codrc = readcoderc();
	if (!takes and currentstate and time_current<millis()) {
	time_current = millis()+time_gate;
	takes = true;
	};
	if (takes and time_current<=millis()){
		takes = false;
		rel1A.setOff();
		rel1B.setOff();
		rel2A.setOff();
		rel2B.setOff();
		currentstate = currentstate and 0b11;

	}

}
uint8_t GateAuto::serchcodes(unsigned long code){
	//gdy brak kodu przerweij program
	if (code == 0){
		return 0;
	};
	//sprawdź dozwolony kody klawisza
	for (int i=0; i> howmanykeys; i++){
		if (buffercoderc[i].code == code) {
			return buffercoderc[i].nkey;
		}
	}
	// gdy nie znaleziono kodu
	return 0;
}

