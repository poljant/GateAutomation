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
const char HTML_TITLE2[] PROGMEM = "PROGRAMOWANIE PILOTA";
const char HTML_TITLE3[] PROGMEM = "DODAWANIE PILOTA";
const char HTML_HEADER[] PROGMEM =
		"<!DOCTYPE html>"
		"<html>"
		"<head>"
		"<meta charset=\"utf-8\">"
		"<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">"
//		"<meta http-equiv=\"refresh\" content=\"5\">"
		"<link rel=\"stylesheet\" href=\"https://maxcdn.bootstrapcdn.com/bootstrap/3.3.4/css/bootstrap.min.css\" >"
		"<title> %s </title>"
		"</head>"
		"<body style=\"text-align: center;color: white; background: black;font-size: 1.5em;\">"
		;

const char HTML_FOOTER[] PROGMEM =
//		"<p><a href = \"/save\"><button class=\"btn btn-info\">Zapisz zmiany</button></a></p>"
		"<p><a href = \"/\"><button class=\"btn btn-info\">Odswierz Strone</button></a></p>"
		"<p>Jan Trzciński &copy; 2022 Ver. %s</p></td></tr></body></html>"
		"</body>"
		"</html>";
const char HTML_FOOTER1[] PROGMEM =
//		"<p><a href = \"/save\"><button class=\"btn btn-info\">Zapisz zmiany</button></a></p>"
		"<p><a href = \"/\"><button class=\"btn btn-info\">Powrót</button></a></p>"
		"<p>Jan Trzciński &copy; 2022 Ver. %s</p></td></tr></body></html>"
		"</body>"
		"</html>";
const char HTML_PAGE1[] PROGMEM =
//		"<p><h2> AUTOMATYCZNE ZAMYKANIE JEST %sŁĄCZONE</h2></p>"
		"<p><h2> %s </h2></p>"
/*		"<p><a href = \"/go\"><button class=\"btn btn-info\">Otwórz brame.</button></a></p>"
		"<p><a href = \"/gc\"><button class=\"btn btn-info\">Zamknij brame.</button></a></p>"
		"<p><a href = \"/glo\"><button class=\"btn btn-info\">Otwórz skrzydło bramy.</button></a></p>"
		"<p><a href = \"/glc\"><button class=\"btn btn-info\">Zamknij skrzydło bramy.</button></a></p>"
		"<p><a href = \"/wo\"><button class=\"btn btn-info\">Otwórz furtke.</button></a></p>"*/
		;

const char HTML_PAGE1A[] PROGMEM =
		"<p><a href = \"/autoc0\"><button class=\"btn btn-info\">Wyłącz automatyczne zamykania</button></a></p>";
const char HTML_PAGE1B[] PROGMEM =
		"<p><a href = \"/autoc1\"><button class=\"btn btn-info\">Załącz automatyczne zamykania</button></a></p>";
const char HTML_ADDING_KEY[] PROGMEM = "<h2> DODAWANIE PILOTA </h2> ";
const char HTML_PROGRAMING_KEY[] PROGMEM = "<h2> PROGRAMOWANIE PILOTA </h2> ";
const char HTML_PUSH_KEY[] PROGMEM = "<p><h2>Wciśnij klawisz %c na pilocie.</h2></p>";
char KEYS[] = "ABCD";


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
//	Serial.println("buff");
//	Serial.println(buff);
	return buff;
}
String HTMLFooter() {             //  stopka strony www  //foot page
	char *buff = new char[sizeof(HTML_FOOTER) + 16];
	sprintf_P(buff, HTML_FOOTER, VERSION);
	return buff;
}
String HTMLFooter1() {             //  stopka strony www  //foot page
	char *buff = new char[sizeof(HTML_FOOTER) + 16];
	sprintf_P(buff, HTML_FOOTER1, VERSION);
	return buff;
}
String HTMLPage1() {             //   page
///	char *text[32];
	char *buff = new char[sizeof(HTML_PAGE1) + 35];
	switch (ga.currentstate ){
	case GATE_OPEN:
		sprintf_P(buff, HTML_PAGE1, PSTR("BRAMA OTWARTA"));
		break;
	case GATE_OPEN2:
		sprintf_P(buff, HTML_PAGE1,   PSTR("SKRZYDŁO BRAMY OTWARTE"));
		break;
	case GATE_CLOSE:
		sprintf_P(buff, HTML_PAGE1, PSTR("BRAMA ZAMKNIĘTA"));
		break;
	case GATE_OPENING:
		sprintf_P(buff, HTML_PAGE1, PSTR("BRAMA JEST OTWIERANA"));
		break;
	case GATE_CLOSING:
		sprintf_P(buff, HTML_PAGE1, PSTR("BRAMA JEST ZAMYKANA"));
		break;
	case GATE_OPENING2:
		sprintf_P(buff, HTML_PAGE1,  PSTR("SKRZYDŁO BRAMY JEST OTWIERANE"));
		break;
	case GATE_CLOSING2:
		sprintf_P(buff, HTML_PAGE1,  PSTR("SKRZYDŁO BRAMY JEST ZAMYKANE"));
		break;
	case GATE_STOP:
		sprintf_P(buff, HTML_PAGE1, PSTR("BRAMA JEST ZATRZYMANA"));
		break;
	default:
		sprintf_P(buff, HTML_PAGE1, PSTR("BRAMA XXX "));
	};
	return buff;
}
String HTML_WebPage1b(){
String	buff="";
	if(ga.currentstate==GATE_CLOSE or ga.currentstate==GATE_CLOSING2){
		buff = PSTR("<p><a href = \"/go\"><button class=\"btn btn-info\">Otwórz brame.</button></a></p>");
	}else{
		buff = PSTR("<p><a href = \"/gc\"><button class=\"btn btn-info\">Zamknij brame.</button></a></p>");}
	if(ga.currentstate==GATE_OPEN2 or ga.currentstate==GATE_OPENING2){
		buff+=PSTR("<p><a href = \"/glc\"><button class=\"btn btn-info\">Zamknij skrzydło bramy.</button></a></p>");
	}else{
		buff+=PSTR("<p><a href = \"/glo\"><button class=\"btn btn-info\">Otwórz skrzydło bramy.</button></a></p>");}
	buff+=PSTR("<p><a href = \"/wo\"><button class=\"btn btn-info\">Otwórz furtke.</button></a></p>");
	buff+=PSTR("<p><a href = \"/scx\"><button class=\"btn btn-info\">Programowanie pilota.</button></a></p>");
	buff+=PSTR("<p><a href = \"/acx\"><button class=\"btn btn-info\">Dodawanie pilota.</button></a></p>");
	return buff;
}
String HTML_WebPage1c(){
	return (ga.autoclose?HTML_PAGE1A:HTML_PAGE1B);
}


String WebPage() {       // połącz wszystkie części strony www
 return (HTMLHeader()+HTMLPage1()+HTML_WebPage1b()+HTML_WebPage1c()+HTMLFooter());
}

String HTTP_Header2(){
	char *buff = new char[sizeof(HTML_HEADER) + 33];
	sprintf_P(buff, HTML_HEADER, HTML_TITLE2);
	return buff;
}
String HTTP_WebPage2(){
	String buff;
	buff = HTML_PROGRAMING_KEY;
//	buff += PSTR("<p><h2>PROGRAMOWANIE PILOTA</h2></p>");
	buff += PSTR("<p><a href = \"/scx0\"><button class=\"btn btn-info\">Programowanie klawisza A.</button></a></p>");
	buff += PSTR("<p><a href = \"/scx1\"><button class=\"btn btn-info\">Programowanie klawisza B.</button></a></p>");
	buff += PSTR("<p><a href = \"/scx2\"><button class=\"btn btn-info\">Programowanie klawisza C.</button></a></p>");
	buff += PSTR("<p><a href = \"/scx3\"><button class=\"btn btn-info\">Programowanie klawisza D.</button></a></p>");
//	Serial.println(buff);
	return buff;
}

String WebPage2() {       // połącz wszystkie części strony www
//	DEBUG_MSG_PROG("[GATE WEB] WebPage2 \n\r");
 return (HTTP_Header2()+HTTP_WebPage2()+HTMLFooter());
}
String WebPage3x(int a){
	String buff;
	char *buf = new char[120];
	buff=F("<p><h2>Zbliż programowany pilot do anteny nadawczej.</h2></p>");
	buff+=HTML_Push_Key(a);
	buff+=F("<p><h2>Na ekranie wciśnij przycisk PROGRAMOWANIE.</h2></p>");
	sprintf_P(buf,PSTR("<p><a href = \"/sc%c\"><button class=\"btn btn-info\">PROGRAMOWANIE</button></a></p>"), KEYS[a]);
	buff+=buf;
//	Serial.println(buff);
	return buff;
}
String WebPage3(int a){
//	DEBUG_MSG_PROG("[GATE WEB] WebPage3 buff =\n\r");
//Serial.println(WebPage3x(a));
	return (HTTP_Header2()+WebPage3x(a)+HTMLFooter1());
}
String HTTP_Header4(){
	char *buff = new char[sizeof(HTML_HEADER) + 33];
	sprintf_P(buff, HTML_HEADER, HTML_TITLE3);
	return buff;
}
String WebPage4x(int a){
	String buff;
	char *buf = new char[120];
	buff=F("<p><h2>Weź pilot, którego kody chcesz dodać. </h2></p>");
	buff+=HTML_Push_Key(a);
	buff+=F("<p><h2>Na ekranie wciśnij przycisk DODAWANIE.</h2></p>");
	sprintf_P(buf,PSTR("<p><a href = \"/ac%c\"><button class=\"btn btn-info\">DODAWANIE</button></a></p>"), KEYS[a]);
	buff+=buf;
//	Serial.println(buff);
	return buff;
}
String WebPage4a(int a){
//	DEBUG_MSG_PROG("[GATE WEB] WebPage3 buff =\n\r");
//Serial.println(WebPage4x(a));
	return (HTTP_Header4()+WebPage4x(a)+HTMLFooter1());
}
String HTTP_WebPage4(){
	String buff;
	buff = PSTR("<p><h2>DODAWANIE PILOTA</h2></p>");
	buff += PSTR("<p><a href = \"/acx0\"><button class=\"btn btn-info\">Dodaj kod klawisza A.</button></a></p>");
	buff += PSTR("<p><a href = \"/acx1\"><button class=\"btn btn-info\">Dodaj kod klawisza B.</button></a></p>");
	buff += PSTR("<p><a href = \"/acx2\"><button class=\"btn btn-info\">Dodaj kod klawisza C.</button></a></p>");
	buff += PSTR("<p><a href = \"/acx3\"><button class=\"btn btn-info\">Dodaj kod klawisza D.</button></a></p>");
//	Serial.println(buff);
	return buff;
}

String WebPage4() {       // połącz wszystkie części strony www
//	DEBUG_MSG_PROG("[GATE WEB] WebPage4 \n\r");
 return (HTTP_Header4()+HTTP_WebPage4()+HTMLFooter1());
}
// funkcja ustawia wszystkie strony www
void setservers() {
	httpUpdate.setup(&httpserver, "/update", www_login, www_pass); // umożliwia aktualizację poprzez WiFi

	httpserver.on("/", []()
		{
		ga.service = false;
		httpserver.send(200, "text/html", WebPage());
	});

	httpserver.on("/gc", []()     //  zamknij brame
			{
		//gdy brama jest otwarta lub otwierana
		if(GATE_OPEN == ga.currentstate or GATE_OPENING == ga.currentstate){
				ga.nkey = 1;
				ga.closegate();
				ga.nkey = 0;}
				httpserver.send(200, "text/html", WebPage());
			});

	httpserver.on("/go", []()      // otwórz brame
			{
		//gdy brama jest zamknięta lub zamykana
		if(GATE_CLOSE == ga.currentstate or GATE_CLOSING == ga.currentstate){
				ga.nkey = 1;
				ga.opengate();
				ga.nkey = 0;}
				httpserver.send(200, "text/html", WebPage());
			});

	httpserver.on("/glc", []()     // zamknij skrzydło bramy
			{
		if(GATE_OPEN2 == ga.currentstate or GATE_OPENING2 == ga.currentstate){
				ga.nkey = 2;
				ga.closegate2();
	//			ga.nkey = 0;
		}
				httpserver.send(200, "text/html", WebPage());
			});

	httpserver.on("/glo", []()      // otwórz skrzydło bramy
			{
		if(GATE_CLOSE == ga.currentstate or GATE_CLOSING2 == ga.currentstate){
				ga.nkey = 2;
				ga.opengate2();
	//			ga.nkey = 0;
		}
				httpserver.send(200, "text/html", WebPage());
			});
	httpserver.on("/wo", []()     // otwórz furtke
			{
				ga.nkey = 3;
				ga.openwicket();
				ga.nkey = 0;
				httpserver.send(200, "text/html", WebPage());
			});

	httpserver.on("/scx", []()      // Programowanie pilota
			{
				httpserver.send(200, "text/html", WebPage2());
			});
	httpserver.on("/scx0", []()      // Programowanie klawisza A
				{
					httpserver.send(200, "text/html", WebPage3(0));
				});
	httpserver.on("/scx1", []()      // Programowanie klawisza B
					{
						httpserver.send(200, "text/html", WebPage3(1));
					});
	httpserver.on("/scx2", []()      // Programowanie klawisza B
					{
						httpserver.send(200, "text/html", WebPage3(2));
					});
	httpserver.on("/scx3", []()      // Programowanie klawisza B
					{
						httpserver.send(200, "text/html", WebPage3(3));
					});
	httpserver.on("/scA", []()      // wyślij kod klawisza A
			{
				ga.sendcodeA();
				DEBUG_MSG_PROG("[GATE WEB] Zaprogramowano klawisz A pilota\n\r");
				httpserver.send(200, "text/html", WebPage3(0));
			});

	httpserver.on("/scB", []()      // wyślij kod klawisza B
			{
				ga.sendcodeB();
				DEBUG_MSG_PROG("[GATE WEB] Zaprogramowano klawisz B pilota\n\r");
				httpserver.send(200, "text/html", WebPage3(1));
			});

	httpserver.on("/scC", []()      // wyślij kod klawisza C
			{
				ga.sendcodeC();
				DEBUG_MSG_PROG("[GATE WEB] Zaprogramowano klawisz C pilota\n\r");
				httpserver.send(200, "text/html", WebPage3(2));
			});

	httpserver.on("/scD", []()      //wyślij kod klawisza D
			{
				ga.sendcodeD();
				DEBUG_MSG_PROG("[GATE WEB] Zaprogramowano klawisz D pilota\n\r");
				httpserver.send(200, "text/html", WebPage3(3));
			});
	httpserver.on("/acx", []()      // Dodawanie pilota
				{
					httpserver.send(200, "text/html", WebPage4());
				});
		httpserver.on("/acx0", []()      // Dodawanie klawisza A
					{
						httpserver.send(200, "text/html", WebPage4a(0));
					});
		httpserver.on("/acx1", []()      // Dodawanie klawisza B
						{
							httpserver.send(200, "text/html", WebPage4a(1));
						});
		httpserver.on("/acx2", []()      // Dodawanie klawisza C
						{
							httpserver.send(200, "text/html", WebPage4a(2));
						});
		httpserver.on("/acx3", []()      // Dodawanie klawisza D
						{
							httpserver.send(200, "text/html", WebPage4a(3));
						});
	httpserver.on("/acA", []()      //dodaj kod klawisza A
			{
				ga.service = true;
				ga.addcoderc(ga.readcoderc(),0);
				ga.ncodrc=0;
				httpserver.send(200, "text/html", WebPage4a(0));
			});

	httpserver.on("/acB", []()      //dodaj kod klawisza B
			{
				ga.service = true;
				ga.addcoderc(ga.readcoderc(),1);
				ga.ncodrc=0;
				httpserver.send(200, "text/html", WebPage4a(1));
			});

	httpserver.on("/acC", []()      //dodaj kod klawisza C
			{
				ga.service = true;
				ga.addcoderc(ga.readcoderc(),2);
				ga.ncodrc=0;
				httpserver.send(200, "text/html", WebPage4a(2));
			});

	httpserver.on("/acD", []()      //dodaj kod klawisza D
			{
				ga.service = true;
				ga.addcoderc(ga.readcoderc(),3);
				ga.ncodrc=0;
				httpserver.send(200, "text/html", WebPage4a(3));
			});

	httpserver.on("/autoc1", []()      // ustaw autoclose
			{
				ga.autoclose= true;
				//	   saveEEProm();
				httpserver.send(200, "text/html", WebPage());
			});
	httpserver.on("/autoc0", []()      // wstrzymaj autoclose
			{
				ga.autoclose= false;
				//	   saveEEProm();
				httpserver.send(200, "text/html", WebPage());
			});
	httpserver.on("/lp", []()      // zmień led_pulse
			{
				ga.led_pulse = !ga.led_pulse;
				//	   saveEEProm();
				httpserver.send(200, "text/html", WebPage());
			});
	httpserver.on("/save", []()      // zapisz zmiany ustawień
			{
				//	   saveEEProm();
				httpserver.send(200, "text/html", WebPage());
			});
	httpserver.on("/reboot", []()      // uruchom ponownie
				{
		//ESP.reset();
		ESP.restart();
					//	   saveEEProm();
					httpserver.send(200, "text/html", WebPage());
				});

	httpserver.begin();                // Start serwera www
#ifdef DEBUG
	Serial.println("Server started");
#endif
}


