/*
 * GateAutomation.cpp
 *
 *  Created on: 29 kwi 2022
 *      Author: jant
 */
#include <Arduino.h>
#include "../GateAutomation/GateAutomation.h"
#ifdef BUTTON2
#include <Button2.h>
#endif
#include "Relay.h"
Relay rel1A;
Relay rel1B;
Relay rel2A;
Relay rel2B;
Relay relw;
Relay relcon;
GateAutomation::GateAutomation() {
	// TODO Auto-generated constructor stub

}

GateAutomation::~GateAutomation() {
	// TODO Auto-generated destructor stub
}

void GateAutomation::begin() {
	rel1A.begin(pin_gate1A);
    rel1B.begin(pin_gate1B);
	rel2A.begin(pin_gate2A);
    rel2B.begin(pin_gate2B);
    relw.begin(pin_wicket);
    relcon.begin(pin_con);

}
void GateAutomation::opengate(){
	opengate2();
//	delay (time_gateB*1000);
	opengate1();
}
void GateAutomation::closegate(){
	closegate1();
	delay (time_delay_gateB);
	closegate2();

}
void GateAutomation::opengate1(){
	rel1A.setOn();
	rel1B.setOff();
 //   openinggate1 = true;
    curentstate1 = 2;
}
void GateAutomation::closegate1(){
	rel1A.setOff();
	rel1B.setOn();
 //   closinggate1 = true;
    curentstate1 = 3;
}
void GateAutomation::opengate2(){
	rel2A.setOn();
	rel2B.setOff();
//    openinggate2 = true;
    curentstate2 = 2;
}
void GateAutomation::closegate2(){
	rel2A.setOff();
	rel2B.setOn();
//    closinggate2 = true;
    curentstate2 = 3;
}
bool GateAutomation::bellon(){
	return false;
}
uint8_t GateAutomation::stategate(){
	return 0;
}
uint8_t GateAutomation::stategate1(){
	uint8_t i;
//	if (openinggate1) {
//		i = 2;
//	}else(closinggate1) {
//		i = 3;
//	}
	return (rel1A.read()) ? ((openinggate1) ? 2: 1) : 0;
	//return i;
}
uint8_t GateAutomation::stategate2(){
	return false;
}
bool GateAutomation::statebutton(){
	return false;
}
void GateAutomation::start(){

}
void GateAutomation::pause(){

}
void GateAutomation::stop(){

}
bool GateAutomation::openwicket(){
	return false;
}
void GateAutomation::gateloop(){

}
