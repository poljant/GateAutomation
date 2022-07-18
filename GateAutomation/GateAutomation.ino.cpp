/*
 * main.cpp
 *
 *  Created on: 24 lis 2019
 *      Author: jant
 */
#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ESP8266WebServer.h>
#include "secrets.h"
#include "debug.h"
#include <Button2.h>
#include <LittleFS.h>
#include "../GateAutomation/GateAuto.h"
#include "../GateAutomation/WebPages.h"
#include "WiFiManager.h"

String version = VERSION;
bool eraseSetWiFi = false; //true kasuje dane WiFi
unsigned long apduration;	//czas trwania stanu AP
unsigned long apminutes = 10; //ile minut ma trwać stan AP
#define ID_DEV1 "BELL"
extern ESP8266WebServer httpserver;
#ifdef IP_STATIC
IPAddress IPadr(10, 110, 3, 33); // stały IP
IPAddress netmask(255, 255, 0, 0);
IPAddress gateway(10, 110, 0, 1);
//////////////////////////////
#endif
WiFiManager wm;
// Create AsyncWebServer object on port 80
//AsyncWebServer server(80);
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
/*// Replaces placeholder with LED state value
String processor(const String& var){
  Serial.println(var);
//  if(var == "GPIO_STATE"){
//    if(digitalRead(ledPin)){
//      ledState = "OFF";
 //   }
 //   else{
//      ledState = "ON";
 //   }
//    Serial.print(ledState);
//    return ledState;
//  }
  return String();
}*/
/*void WebPages() {
	  // Route for root / web page
	  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
	    request->send(LittleFS, "/index.html", String(), false, processor);
	  });

	  // Route to load style.css file
	  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request){
	    request->send(LittleFS, "/style.css", "text/css");
	  });
	  // Start server
	  server.begin();
}*/

//dane dla AP
const char *ap_ssid = hostname().c_str();   // SSID AP
const char *ap_pass = "12345678";  // password do AP
int ap_channel = 7; //numer kanału dla AP
void setwifi(bool erase) {
//#include <WiFiManager.h>
//	WiFiManager wm;
	if (erase) {
		wm.resetSettings();
	}
#ifdef IP_STATIC
	wm.setSTAStaticIPConfig(IPadr, gateway, netmask);
//	WiFi.config(IPadr, gateway, netmask);  // stały IP
#endif
	wm.autoConnect(hostname().c_str());
	  delay(100);
#ifdef IP_STATIC
//  WiFi.config(IPadr,gateway,netmask);  // stały IP
#endif
//  WiFi.mode(WIFI_STA); //tryb STATION
  WiFi.begin(myssid, mypass);
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
	//Initialize LittleFS;
	  if(!LittleFS.begin()){
		  DEBUG_MSG_PROG("[SETUP] Wystąpił błąd podczas montowania LittleFS");
	    return;
	  }
/*		WiFiManager wifiManager;
#ifdef IP_STATIC
	wifiManager.setSTAStaticIPConfig(IPadr, gateway, netmask);
	WiFi.config(IPadr, gateway, netmask);  // stały IP
#endif
		wifiManager.autoConnect(hostname().c_str());*/
	setwifi(false); //eraseSetWiFi);
	ga.begin();
	buttondevice.begin(ga.pin_button);
//	buttondevice1.setLongClickTime(4000);
	buttondevice.setClickHandler(clickdev);
//	buttondevice1.setLongClickHandler(longclickdev1);

	Serial.println();
	Serial.println(hostname());
	Serial.println(WiFi.localIP());
	Serial.println(WiFi.macAddress());
	apduration = fminutes(apminutes);
	setservers();
	MDNS.begin(hostname(), WiFi.localIP());
}

// the loop function runs over and over again forever
void loop() {
#ifdef BUTTON2
	buttondevice.loop();
#endif
	ga.gateloop();
	if (WiFi.status() != WL_CONNECTED) {
		WiFi.mode(WIFI_AP_STA); //tryb AP+STATION
		apduration = fminutes(apminutes);  // licz czas trwania AP po połaczeniu
	} else {
		if (apduration <= millis()) { // gdy minął czas trwania AP przełącz na STATION
			WiFi.mode(WIFI_STA); //tryb STATION
		}
	}
	httpserver.handleClient();
	MDNS.update();
}
