/*
 * GateAutomation.h
 *
 *  Created on: 29 kwi 2022
 *      Author: jant
 */

#ifndef GATEAUTOMATION_H_
#define GATEAUTOMATION_H_

class GateAutomation {
	int pin_gate1a = GPIO16;
	int pin_gate1b;
	int pin_gate2a;
	int pin_gate2b;
	int pin_wicket;
	int pin_button = 0;
	int pin_RF_Rx;
	int pin_RF_Tx;
	int pin_RxD;
	int pin_TxD;
	int pin_con;
public:
	GateAutomation();
	virtual ~GateAutomation();
	void begin();
	void opengate();
	void closegate();
	bool stategate();
	void opengateA();
	void closegateA();
	bool stategateA();
	void opengateB();
	void closegateB();
	bool stategateB();
	bool bellon();
	void stop();
	void pause();
	void start();
	bool statebutton();
	void gateloop();
	bool wicketopen();


};
#endif /* GATEAUTOMATION_H_ */

