/*
 * main.cpp
 *
 *  Created on: 24 lis 2019
 *      Author: jant
 */
#include <Arduino.h>

#include <ESP8266WiFi.h>
//#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include "secrets.h"
#include "debug.h"
#include <Button2.h>
#include "WiFiManager.h"
#include "../GateAutomation/GateAuto.h"

String version = VERSION;
bool eraseSetWiFi = false;
unsigned long aptime;
unsigned long apminutes = 10;
#define ID_DEV1 "BELL"

#ifdef IP_STATIC
IPAddress IPadr(10, 110, 1, 115); // stały IP
IPAddress netmask(255, 255, 0, 0);
IPAddress gateway(10, 110, 0, 1);
//////////////////////////////
#endif
//Button2 buttondevice;
GateAuto ga;
//unsigned int ilM=10;
unsigned long fminutes(unsigned int ile) {
	return (millis() + (1000 * 60 * ile));
}
///////////////////////////////////////////
String hostname(void) {
	uint32_t chipId = ESP.getChipId();
	char uid[20];
	sprintf_P(uid, PSTR("%s_%02x%02x%02x"), HOSTNAME,
			(uint16_t) ((chipId >> 16) & 0xff),
			(uint16_t) ((chipId >> 8) & 0xff),
			((uint16_t) chipId & 0xff) );
	//	Serial.println(uid);
	return String(uid);
}

//dane dla AP
const char *ap_ssid = hostname().c_str();   // SSID AP
const char *ap_pass = "12345678";  // password do AP
int ap_channel = 7; //numer kanału dla AP
void setwifi(bool erase) {
#include <WiFiManager.h>
	WiFiManager wifiManager;
	if (erase) {
		wifiManager.resetSettings();

	}
#ifdef IP_STATIC
	wifiManager.setSTAStaticIPConfig(IPadr, gateway, netmask);
	WiFi.config(IPadr, gateway, netmask);  // stały IP
#endif
	wifiManager.autoConnect(hostname().c_str());
	delay(100);
}
#ifdef BUTTON2
Button2 buttondevice;

void clickdev(Button2 &btn) {
//	if (!device1.read()) {
		DEBUG_MSG_PROG("[CLICK] %s On\n\r", ID_DEV1);
// załącz dzwonek
//	} else {
//		DEBUG_MSG_PROG("[CLICK] %s Off\n\r", ID_DEV1);
//	}
}
#endif
void setup() {
	WiFi.mode(WIFI_STA);
	Serial.begin(115200);
	setwifi(eraseSetWiFi);
	ga.begin();
	buttondevice.begin(ga.pin_button);
//	buttondevice1.setLongClickTime(4000);
	buttondevice.setClickHandler(clickdev);
//	buttondevice1.setLongClickHandler(longclickdev1);
	MDNS.begin(hostname(), WiFi.localIP());
	Serial.println();
	Serial.println(hostname());
	Serial.println(WiFi.localIP());
	Serial.println(WiFi.macAddress());
	aptime = fminutes(apminutes);
}

// the loop function runs over and over again forever
void loop() {
#ifdef BUTTON2
	buttondevice.loop();
#endif
	ga.gateloop();
	if (WiFi.status() != WL_CONNECTED) {
		WiFi.mode(WIFI_AP_STA); //tryb AP+STATION
		aptime = fminutes(apminutes);  // licz czas trwania AP po połaczeniu
	} else {
		if (aptime <= millis()) { // gdy minął czas trwania AP przełącz na STATION
			WiFi.mode(WIFI_STA); //tryb STATION
		}
	}
	MDNS.update();
}



