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
#define VERSION "0.3.4 beta"
#define IP_STATIC
#define DEBUG
//add manual button
#define BUTTON2
#define HOSTNAME "GateAutomation"
#define GATE_CLOSE        0b00	//brama zamknięta
#define GATE_OPEN   0b01000000	//brama otwarta
#define GATE_OPEN2  0b01010000	//brama2 otwarta
#define GATE_OPENING      0b01	//brama jest otwierana
#define GATE_CLOSING     0b010	//brama jest zamykana
#define GATE_OPENING1  0b01001	//brama 1 jest otwierana
#define GATE_CLOSING1  0b01010	//brama 1 jest zamykana
#define GATE_OPENING2 0b010001	//brama 2 jest otwierana
#define GATE_CLOSING2 0b010010	//brama 2 jest zamykana
#define GATE_STOP   0b10000000	//brama ruch bramy jest zatrzymany
#define HOWMANYKEYS		12	// ile pilotów * ile klawiszy 3*4=12

/*typedef union {
  uint32_t data;
  struct {
	  uint32_t code:24;
	  uint32_t nkey:8;
  };
}DATAKEY;*/
//clasa potrzebna do utworzenia tablicy z kodami
class datakeys {
	public:
	uint32_t code; // kod klawisza liczba 24 bitowa
	uint8_t nkey;  // numer klawisza 1 = A, 2 = B, 3 = C,  4 = D
};

class GateAuto {
	friend class datakeys;
//	struct DATAKEY datakeys;
	uint8_t pin_gate1A = 16; //GPIO16 D0
	uint8_t pin_gate1B = 14; //GPIO14 D5
	uint8_t pin_gate2A = 12; //GPIO12 D6
	uint8_t pin_gate2B = 13; //GPIO13 D7
	uint8_t pin_wicket = 15; //GPIO15 D8
//	uint8_t pin_button = 0; //GPIO0 D3
	uint8_t pin_RF_Rx = 5; //GPIO5 D1
	uint8_t pin_RF_Tx = 4; //GPIO4 D2
	uint8_t pin_RxD = 3;	//RxD
	uint8_t pin_TxD = 1;	//TxD
	uint8_t pin_led = 2; //GPIO2 D4

	bool takes = false;	//ruch bramy trwa to true
	bool takes_autoclose = false;	// true działa automatyczzne zamykanie bramy

//	int time_closing = 15; // sek
	int duration_sek = 50;  // ile sekund trwa zamykanie lub otwieranie bramy
	int gate_duration = duration_sek * 1000;	//czas działania ruchu bramy w milisekundach
	int time_delay_gateA = 0 * 1000;	//msekund  opóźnienie rozpoczęcia ruchu bramy 1
	int time_delay_gateB = 2 * 1000;	//msekund  opóźnienie rozpoczęcia ruchu bramy 2
	int duration_on_wicket = 2 * 1000;	//msekund	czas działania elektromagnesu zamka furtki
//	int time_gate = gate_duration;

	int time_led = gate_duration + 3000;	//czas świecenia sygnalizacji LED
	int time_delay_read_nex_code = 500; //opuźnienie kolejnego odczytu codu
	long double time_read_code = 0;
	long double time_current = 0;	//pamięta czas liczony do końca ruchu




	uint32_t codrcx = 0;

	uint8_t nkeyx = 0;

public:
	bool service = false; //blokuje loop() podczas programowania
//	int howmanykeys = 12;	//ile pilotów * ile klawiszy -- 3 * 4
	datakeys buffercoderc[HOWMANYKEYS];	//ile pilotów * ile klawiszy -- 3 * 4
	//zmienne pamiętające status bramy
	uint8_t currentstate = 0b0;
	// czy brama zamykana automatycznie po określonym czasie otwarcia
	bool autoclose = false;
	//gdy sygnalizacja led ma w sobie pulsator
	bool led_pulse = false; //czy sygnalizacja led ma pulsować? tak = true, nie = false
	int time_autoclose = 180 ; //  sekund
	unsigned long curent_time_autoclose = 0;
	bool ledOn = false; //czy led ma być załaczony true czy nie false
	long double time_blink = 0; // zmienna do tworzenia migotania led
	int led_long_pulse = 1000; //długość impulsu led w milisekundach
	//status skrzydła pierwszego bramy
/*	uint8_t currentstate1 = 0b0;
	//status skrzydła drugiego bramy
	uint8_t currentstate2 = 0b0;*/
	volatile uint32_t codrc = 0;
	uint8_t nkey = 0;
	uint32_t ncodrc = 0;
	uint8_t pin_button = 0; //GPIO0 D3
	//kody pilota nieprogramowalnego
/*
	unsigned long codekeyA1;	// = 6115587; //0b10111010101000100000011
	unsigned long codekeyB1;	// = 6115776; //0b10111010101000111000000
	unsigned long codekeyC1;	// = 6115632; //0b10111010101000100110000
	unsigned long codekeyD1;	// = 6115596; //0b10111010101000100001100
//	kody pilota z przyciskami A i B
	//201987 - 0b000000110001010100000011;
	//202176 - 0b000000110001010111000000;
*/

	GateAuto();
	virtual ~GateAuto();

void begin();
void opengate();
void closegate();
uint8_t stategate();
void opengate1();
void closegate1();
//uint8_t stategate1();
void opengate2();
void closegate2();
//uint8_t stategate2();
bool bellon();
void stop();
//void pause();
//void start();
bool statebutton();
void gateloop();
void openwicket();
uint32_t readcoderc();
void readcodercx();
void sendcoderc(uint32_t code);
unsigned long addduration(int duration);
unsigned long adddurationsek(int duration);
bool addcoderc(uint32_t code, uint8_t nkey);
bool iscode(uint32_t cod);
void addcodesrc();
bool addcodercA();
bool addcodercB();
bool addcodercC();
bool addcodercD();
bool clearcodesrc();
bool clearcoderc(uint8_t ncode);
uint32_t viewcoderc(uint8_t ncode);
void setcodes();
void sendcodeA();
void sendcodeB();
void sendcodeC();
void sendcodeD();
uint8_t serchcodes(uint32_t code);
const char bin2str(uint32_t t);
const char byte2str(uint8_t t);
void led_blink(int tp);

};

#endif /* GATEAUTOMATION_GATEAUTO_H_ */
