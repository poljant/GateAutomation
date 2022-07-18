/*
 * WebPages.cpp
 *
 *  Created on: 5 lip 2022
 *      Author: jant
 */
#include <ESP8266WiFi.h>
#include "WebPages.h"
#include "GateAuto.h"
#include <ESP8266WebServer.h>
#include <ESP8266HTTPUpdateServer.h>
//#include "WiFiManager.h"
//#define WEBPAGEWIFISCAN
// login i hasło do sytemu
const char* www_login = "admin";
const char* www_pass = "esp8266";
const char* ssid;
const char* pass;
const char* xpass;
const char* xssid;
const char* modes[] = { "NULL", "STA", "AP", "STA+AP" };
const char* phymodes[] = { "", "B", "G", "N" };
const char* encrypType[] = { "OPEN","WEP", "WPA","WPA", "WPA2", "WEP", "WPA_WPA2","OPEN", "WPA/WPA2/PSK" }; //??????
extern GateAuto ga;
//extern WiFiManager wm;
const int port = 80;                 // port serwera www
ESP8266WebServer httpserver(port);
ESP8266HTTPUpdateServer httpUpdate;
const char HTML_TITLE[] PROGMEM = "Gate Automation";
const char HTML_HEADER[] PROGMEM =
		"<!DOCTYPE html>"
		"<html>"
		"<head>"
		"<title> %s </title>"
		"<meta charset=\"utf-8\">"
		"<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">"
//		"<meta http-equiv=\"refresh\" content=\"5\">"
		"<link rel=\"stylesheet\" href=\"https://maxcdn.bootstrapcdn.com/bootstrap/3.3.4/css/bootstrap.min.css\" >"
		"</head>"
		"<body style=\"text-align: center;color: white; background: black;font-size: 1.5em;\">"
		;

const char HTML_FOOTER[] PROGMEM =
		"<p><a href = \"/save\"><button class=\"btn btn-info\">Zapisz zmiany</button></a></p>"
		"<p><a href = \"/\"><button class=\"btn btn-info\">Odświerz stronę</button></a></p>"
		"<p>Jan Trzciński &copy; 2021 VII Ver. %s</p></td></tr></body></html>"
		"</body>"
		"</html>";
const char HTML_PAGE1[] PROGMEM =
		"<h1> %s </h1>"
		"<p><a href = \"/go\"><button class=\"btn btn-info\">Otwórz brame.</button></a></p>"
		"<p><a href = \"/gc\"><button class=\"btn btn-info\">Zamknij brame.</button></a></p>"
		"<p><a href = \"/glo\"><button class=\"btn btn-info\">Otwórz skrzydło bramy.</button></a></p>"
		"<p><a href = \"/glc\"><button class=\"btn btn-info\">Zamknij skrzydło bramy.</button></a></p>"
		"<p><a href = \"/wo\"><button class=\"btn btn-info\">Otwórz furtke.</button></a></p>"
		;
//const char HTMLSTATUS[] PROGMEM =
//		"<h1> %s </h1>"
		;
const char HTML_ADDING_KEY[] PROGMEM = "<h1> DODAWANIE PILOTA </h1> ";
const char HTML_PROGRAMING_KEY[] PROGMEM = "<h1> PROGRAMOWANIE PILOTA </h1> ";
const char HTML_PUSH_KEY[] PROGMEM = "<p>Wciśnij klawisz %c na pilocie</p>";
const char KEYS[] PROGMEM = "ABCD";


		// funkcja zamieniająca adres IP na string
//convert IP to String
String IPtoStr(IPAddress IP) {
	String result;
	for (int i = 0; i < 4; ++i) {
		result += String(IP[i]);
		if (i < 3)
			result += '.';
	}
	return result;
}
//covert IP to char
char* IPtoChar(int ip) {
	char *ip_str = new char[16];
	sprintf(ip_str, "%d.%d.%d.%d", (ip) & 0xFF, (ip >> 8) & 0xFF,
			(ip >> 16) & 0xFF, (ip >> 24) & 0xFF);
	return ip_str;
}

String HTMLHeader() {           //  nagłówek strony //Header page

	char *buff = new char[sizeof(HTML_HEADER) + 33];
	sprintf_P(buff, HTML_HEADER, HTML_TITLE);
	return buff;
}
String HTML_Push_Key(int nk) {
	if(nk<0 or nk>3) return "";
	char *buff = new char[sizeof(HTML_PUSH_KEY) + 3];
	sprintf_P(buff, HTML_PUSH_KEY, KEYS[nk]);
	return buff;
}
String HTMLFooter() {             //  stopka strony www  //foot page
	char *buff = new char[sizeof(HTML_FOOTER) + 16];
	sprintf_P(buff, HTML_FOOTER, VERSION);
	return buff;
}
String HTMLPage1() {             //  stopka strony www  //foot page
///	char *text[32];
	char *buff = new char[sizeof(HTML_PAGE1) + 32];
	switch (ga.currentstate ){
	case GATE_OPEN:
		if (ga.nkey == 2) {
		sprintf_P(buff, HTML_PAGE1, PSTR("SKRZYDŁO BRAMY OTWARTE"));
		}else{
			sprintf_P(buff, HTML_PAGE1, PSTR("BRAMA OTWARTA"));
		}
		break;
	case GATE_CLOSE:
		sprintf_P(buff, HTML_PAGE1, PSTR("BRAMA ZAMKNIĘTA"));
		break;
	case GATE_OPENING:
		sprintf_P(buff, HTML_PAGE1, PSTR("BRAMA JEST OTWIERANA"));
		break;
	case GATE_CLOSING:
		sprintf_P(buff, HTML_PAGE1, PSTR("BRAMA JEST ZAMYKANE"));
		break;
	case GATE_OPENING2:
		sprintf_P(buff, HTML_PAGE1, PSTR("SKRZYDŁO BRAMY JEST OTWIERANE"));
		break;
	case GATE_CLOSING2:
		sprintf_P(buff, HTML_PAGE1, PSTR("SKRZYDŁO BRAMY JEST ZAMYKANE"));
		break;
	case GATE_STOP:
		sprintf_P(buff, HTML_PAGE1, PSTR("BRAMA JEST ZATRZYMANA"));
		break;
	default:
		sprintf_P(buff, HTML_PAGE1,PSTR("BRAMA XXX "));
};
	return buff;
}
String WebPage() {       // połącz wszystkie części strony www
 return (HTMLHeader()+HTMLPage1()+HTMLFooter());
}
// funkcja ustawia wszystkie strony www
void setservers() {
	httpUpdate.setup(&httpserver, "/update", www_login, www_pass); // umożliwia aktualizację poprzez WiFi

	httpserver.on("/", []() {
		httpserver.send(200, "text/html", WebPage());
	});

	httpserver.on("/gc", []()     //  zamknij brame
			{
				ga.nkey = 1;
				ga.closegate();
				ga.nkey = 0;
				httpserver.send(200, "text/html", WebPage());
			});

	httpserver.on("/go", []()      // otwórz brame
			{
				ga.nkey = 1;
				ga.opengate();
				ga.nkey = 0;
				httpserver.send(200, "text/html", WebPage());
			});

	httpserver.on("/glc", []()     // zamknij skrzydło bramy
			{
				ga.nkey = 2;
				ga.closegate2();
	//			ga.nkey = 0;
				httpserver.send(200, "text/html", WebPage());
			});

	httpserver.on("/glo", []()      // otwórz skrzydło bramy
			{
				ga.nkey = 2;
				ga.opengate2();
	//			ga.nkey = 0;
				httpserver.send(200, "text/html", WebPage());
			});
	httpserver.on("/wo", []()     // otwórz furtke
			{
				ga.nkey = 3;
				ga.openwicket();
				ga.nkey = 0;
				httpserver.send(200, "text/html", WebPage());
			});

	httpserver.on("/sca", []()      // wyślij kod klawisza A
			{
		httpserver.send(200, "text/html", HTML_PROGRAMING_KEY+HTML_Push_Key(0));
				ga.sendcodeA();
				httpserver.send(200, "text/html", WebPage());
			});

	httpserver.on("/scb", []()      // wyślij kod klawisza B
			{
				httpserver.send(200, "text/html", HTML_PROGRAMING_KEY+HTML_Push_Key(1));
				ga.sendcodeB();
				httpserver.send(200, "text/html", WebPage());
			});

	httpserver.on("/scc", []()      // wyślij kod klawisza C
			{
				httpserver.send(200, "text/html", HTML_PROGRAMING_KEY+HTML_Push_Key(2));
				ga.sendcodeC();
				httpserver.send(200, "text/html", WebPage());
			});

	httpserver.on("/scd", []()      //wyślij kod klawisza D
			{
				httpserver.send(200, "text/html", HTML_PROGRAMING_KEY+HTML_Push_Key(3));
				ga.sendcodeD();
				httpserver.send(200, "text/html", WebPage());
			});

	httpserver.on("/aca", []()      //dodaj kod klawisza A
			{
				httpserver.send(200, "text/html", HTML_ADDING_KEY+HTML_Push_Key(0));
				ga.addcoderc(ga.readcoderc(),0);
				httpserver.send(200, "text/html", WebPage());
			});

	httpserver.on("/acb", []()      //dodaj kod klawisza B
			{
				httpserver.send(200, "text/html", HTML_ADDING_KEY+HTML_Push_Key(1));
				ga.addcoderc(ga.readcoderc(),1);
				httpserver.send(200, "text/html", WebPage());
			});

	httpserver.on("/acc", []()      //dodaj kod klawisza C
			{
				httpserver.send(200, "text/html", HTML_ADDING_KEY+HTML_Push_Key(2));
				ga.addcoderc(ga.readcoderc(),2);
				httpserver.send(200, "text/html", WebPage());
			});

	httpserver.on("/acd", []()      //dodaj kod klawisza d
			{
				httpserver.send(200, "text/html", HTML_ADDING_KEY+HTML_Push_Key(3));
				ga.addcoderc(ga.readcoderc(),3);
				httpserver.send(200, "text/html", WebPage());
			});

	httpserver.on("/save", []()      // zapisz zmiany ustawień
			{
				//	   saveEEProm();
				httpserver.send(200, "text/html", WebPage());
			});

	httpserver.begin();                // Start serwera www
#ifdef DEBUG
	Serial.println("Server started");
#endif
}


