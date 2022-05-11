/*
 * GateAuto.h
 *
 *  Created on: 11 maj 2022
 *      Author: jant
 */

#ifndef GATEAUTOMATION_GATEAUTO_H_
#define GATEAUTOMATION_GATEAUTO_H_

typedef unsigned char uint8_t;
typedef unsigned int uint32_t;
#define VERSION "0.3 beta"
#define IP_STATIC
#define DEBUG
//add manual button
#define BUTTON2
#define HOSTNAME "GateAutomation"
#define GATE_CLOSE        0b00
#define GATE_OPEN   0b01000000
#define GATE_OPENING      0b01
#define GATE_CLOSING     0b010
#define GATE_OPENING1  0b01001
#define GATE_CLOSING1  0b01010
#define GATE_OPENING2 0b010001
#define GATE_CLOSING2 0b010010
#define GATE_STOP   0b10000000

class datakeys {
	public:
	unsigned long code;
	uint8_t nkey;
};

class GateAuto {
	uint8_t pin_gate1A = 16; //GPIO16;
	uint8_t pin_gate1B = 14; //GPIO14
	uint8_t pin_gate2A = 12; //GPIO12
	uint8_t pin_gate2B = 13; //GPIO13
	uint8_t pin_wicket = 15; //GPIO15
//	uint8_t pin_button = 0; //GPIO0
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
	bool takes = false;

	int time_closing = 60; // sek
	int time_gateA = 60 * 1000;
	int time_gateB = 60 * 1000;
	int time_delay_gateA = 0 * 1000;	//msekund
	int time_delay_gateB = 10 * 1000;	//msekund
	int time_delay_wicket = 10 * 1000;	//msekund
	int time_gate = time_closing * 1000;
	long double time_current = 0;
	long double gate_duration = 0;
	int howmanykeys = 12;
	unsigned long codekeyA = 0b000000110001010100000011;
	unsigned long codekeyB = 0b000000110001010111000000;
	unsigned long codekeyC = 0b000000110001010100110000;
	unsigned long codekeyD = 0b000000110001010100001100;
	datakeys buffercoderc[12];


//	const char statusget[] = {"GATE_CLOSE", "GATE_OPEN", "GATE_OPENING", "GATE_CLOSING"};
public:
	//zmienne pamiętające status bramy
	uint8_t currentstate = 0b0;
	//status skrzydła pierwszego bramy
	uint8_t currentstate1 = 0b0;
	//int currentstate1b = 0;
	//status skrzydła drugiego bramy
	uint8_t currentstate2 = 0b0;
	volatile unsigned long codrc = 0;
	uint8_t pin_button = 0; //GPIO0
	GateAuto();
	virtual ~GateAuto();

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
void openwicket();
unsigned long readcoderc();
void sendcoderc(unsigned long code);
bool addcoderc();
bool addcodercA();
bool addcodercB();
bool addcodercC();
bool addcodercD();
bool clearcoderc();
bool clearcoderc(uint8_t ncode);
unsigned long viewcoderc(uint8_t ncode);
void setcodes();
void sendcodeA();
void sendcodeB();
void sendcodeC();
void sendcodeD();
uint8_t serchcodes(unsigned long code);
//void clickdev1();
};

#endif /* GATEAUTOMATION_GATEAUTO_H_ */
