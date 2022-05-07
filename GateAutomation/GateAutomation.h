/*
 * GateAutomation.h
 *
 *  Created on: 29 kwi 2022
 *      Author: jant
 */

#ifndef GATEAUTOMATION_H_
#define GATEAUTOMATION_H_
#define VERSION "0.1 beta"
#define IP_STATIC
//add manual button
#define BUTTON2
#define HOSTNAME "GateAutomation"

class GateAutomation {
	uint8_t pin_gate1A = 16; //GPIO16
	uint8_t pin_gate1B = 14; //GPIO14
	uint8_t pin_gate2A = 12; //GPIO12
	uint8_t pin_gate2B = 13; //GPIO13
	uint8_t pin_wicket = 15; //GPIO15
	uint8_t pin_button = 0; //GPIO0
	uint8_t pin_RF_Rx = 5; //GPIO5
	uint8_t pin_RF_Tx = 4; //GPIO4
	uint8_t pin_RxD = 3;	//RxD
	uint8_t pin_TxD = 1;	//TxD
	uint8_t pin_con = 2; //GPIO2
	bool isopen1 = false;
	bool openinggate1 = false;
	bool closinggate1 = false;
	bool isopen2 = false;
	bool openinggate2 = false;
	bool closinggate2 = false;
	bool isopenwicket = false;
	uint8_t curentstate1 = 0;
	//uint8_t curentstate1b = 0;
	uint8_t curentstate2 = 0;
	//uint8_t curentstate2b = 0;
	int time_gateA = 60 * 1000;
	int time_gateB = 60 * 1000;
	int time_delay_gateA = 0 * 1000;	//msekund
	int time_delay_gateB = 10 * 1000;	//msekund
	int time_delay_wicket = 0 * 1000;	//msekund
	const char statusget[] PROGMEM = {"GATE_CLOSE", "GATE_OPEN", "GATE_OPENING", "GATE_CLOSING"};
public:
	GateAutomation();
	virtual ~GateAutomation();
	void begin();
	void opengate();
	void closegate();
	uint8_t stategate();
	void opengate1();
	void closegate1();
	uint8_t stategate1();
	void opengate2();
	void closegate2();
	uint8_t stategate2();
	bool bellon();
	void stop();
	void pause();
	void start();
	bool statebutton();
	void gateloop();
	bool openwicket();


};
#endif /* GATEAUTOMATION_H_ */

