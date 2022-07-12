/*
 * WebPages.cpp
 *
 *  Created on: 5 lip 2022
 *      Author: jant
 */
#include "WebPages.h"
#include "GateAuto.h"
// login i hasło do sytemu
const char* www_login = "admin";
const char* www_pass = "esp8266";
extern GateAuto ga;
const int port = 80;                 // port serwera www
ESP8266WebServer server(port);
ESP8266HTTPUpdateServer httpUpdate;
const char HTML_TITLE[] PROGMEM = "Gate Automation";
const char HTML_HEADER[] PROGMEM =
		"<!DOCTYPE html>"
		"<html>"
		"<head>"
		"<title> %s </title>"
		"<meta charset=\"utf-8\">"
		"<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">"
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
const char HTML_ADDING_KEY[] PROGMEM = "<h1> DODAWANIE PILOTA </h1> ";
const char HTML_PROGRAMING_KEY[] PROGMEM = "<h1> PROGRAMOWANIE PILOTA </h1> ";
const char HTML_PUSH_KEY[] PROGMEM = "<p>Wciśnij klawisz %c na pilocie</p>";
const char KEYS[] PROGMEM = "ABCD";
/*
const char HTML_WIFISCAN[] PROGMEM  =
		"<table  align=\"center\" border=\"2\" >"
		"<thead ><tr><th> </th><th style = \"text-align: center;\">SSID</th>"
		"<th>kanał</th><th style = \"text-align: center;\">MAC</th>"
#ifdef POLISH

		"<th style = \"text-align: center;\">RSSI</th><th>zabezp</th><th>ukryta</th><tr></thead><tbody>";
#else

		"<th style = \"text-align: center;\">RSSI</th><th>encryption</th><th>hidden</th><tr></thead><tbody>";
#endif

const char HTML_WIFISCAN_TAB[] PROGMEM  = "<tr><td><form action=\"/wifiset\" metod=\"post\">"
		 "<label><input id=\"SSID\" type=\"radio\" name=\"SSID\" value=\"%s\"></label>"
		 "</td><td>%s</td><td>%i</td><td>%s</td><td>%i dBm</td><td>%s</td><td>%s</td></tr>";

const char HTML_WIFISCAN_TAB_END[] PROGMEM  = "</tbody></table><div><p></p> </div>";

const char HTML_WIFISCAN1[] PROGMEM  =
#ifdef POLISH
		"<p>Jesteś połączony z siecią </p>"
#else
		"<p>Connected with WiFi </p>"
#endif
		"<table align=\"center\" border=\"2\" ><tbody>"
		"<thead ><tr><th style = \"text-align: center;\">SSID</th>"
#ifdef POLISH
		"<th>kanał</th><th style = \"text-align: center;\">MAC</th>"
#else
		"<th>channel</th><th style = \"text-align: center;\">MAC</th>"
#endif
		"<th style = \"text-align: center;\">RSSI</th></thead><tbody>"
		"<td>%s</td><td>%i</td><td>%s</td><td>%i dBm</td></tbody></table>"
		"<label>IP: %s</label>"
#ifdef POLISH
		"<p>Aby zmienić sieć WiFi.</p>";
#else
		"<p>To change the WiFi network.</p>";
#endif

const char HTML_WIFISCAN1A[] PROGMEM  =
#ifdef POLISH
		 "<p>Brak połączenia z siecią WiFi.</p>"
#else
		 "<p>No connection with WiFi.</p>"
#endif
		 "<label>IP AP: %s</label>";


const char HTML_WIFISCAN1B[] PROGMEM  =
#ifdef POLISH
		"<p>Wybierz sieć powyżej i podaj hasło</p>"
		"<label for=\"password\">Hasło do WiFi:</label>"
#else
		"<p>Select the WiFi network above and enter the password</p>"
		"<label for=\"password\">WiFi password:</label>"
#endif
		"<input style=\"color: black;\" type=\"password\" name=\"password\" />"
#ifdef POLISH
		"<input style=\"text-align: center;color: black;\" type=\"submit\" value=\"Połącz z WiFi.\"/></form>"
		"<p><a href = \"/wifiscan\"><button class=\"btn btn-info\">Skanuj ponownie</button></a></p>"
		"<p><a href = \"/\"><button class=\"btn btn-info\">Strona główna</button></a></p>";
#else
		"<input style=\"text-align: center;color: black;\" type=\"submit\" value=\"Connect.\"/></form>"
		"<p><a href = \"/wifiscan\"><button class=\"btn btn-info\">Scan again</button></a></p>"
		"<p><a href = \"/\"><button class=\"btn btn-info\">Home</button></a></p>";
#endif
*/

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
String WebPage() {       // połącz wszystkie części strony www
 return (HTMLHeader()+HTMLPage1()+ HTMLPage2()+HTMLFooter());
}
// funkcja ustawia wszystkie strony www
void setservers() {
	httpUpdate.setup(&server, "/update", www_login, www_pass); // umożliwia aktualizację poprzez WiFi

	server.on("/", []() {
		server.send(200, "text/html", WebPage());
	});

	server.on("/gc", []()     //  zamknij brame
			{

				ga.closegate();
				server.send(200, "text/html", WebPage());
			});

	server.on("/go", []()      // otwórz brame
			{

				ga.opengate();
				server.send(200, "text/html", WebPage());
			});

	server.on("/gcb", []()     // zamknij skrzydło bramy
			{

				ga.closegate2();
				server.send(200, "text/html", WebPage());
			});

	server.on("/gob", []()      // otwórz skrzydło bramy
			{
				ga.opengate2();
				server.send(200, "text/html", WebPage());
			});
	server.on("/wo", []()     // otwórz furtke
			{
				ga.openwicket();
				server.send(200, "text/html", WebPage());
			});

	server.on("/sca", []()      // wyślij kod klawisza A
			{
				server.send(200, "text/html", HTML_PROGRAMING_KEY+HTML_Push_Key(0));
				ga.sendcodeA();
				server.send(200, "text/html", WebPage());
			});

	server.on("/scb", []()      // wyślij kod klawisza B
			{
				server.send(200, "text/html", HTML_PROGRAMING_KEY+HTML_Push_Key(1));
				ga.sendcodeB();
				server.send(200, "text/html", WebPage());
			});

	server.on("/scc", []()      // wyślij kod klawisza C
			{
				server.send(200, "text/html", HTML_PROGRAMING_KEY+HTML_Push_Key(2));
				ga.sendcodeC();
				server.send(200, "text/html", WebPage());
			});

	server.on("/scd", []()      //wyślij kod klawisza D
			{
				server.send(200, "text/html", HTML_PROGRAMING_KEY+HTML_Push_Key(3));
				ga.sendcodeD();
				server.send(200, "text/html", WebPage());
			});

	server.on("/aca", []()      //dodaj kod klawisza A
			{
				server.send(200, "text/html", HTML_ADDING_KEY+HTML_Push_Key(0));
				ga.addcoderc(ga.readcoderc(),0);
				server.send(200, "text/html", WebPage());
			});

	server.on("/acb", []()      //dodaj kod klawisza B
			{
				server.send(200, "text/html", HTML_ADDING_KEY+HTML_Push_Key(1));
				ga.addcoderc(ga.readcoderc(),1);
				server.send(200, "text/html", WebPage());
			});

	server.on("/acc", []()      //dodaj kod klawisza C
			{
				server.send(200, "text/html", HTML_ADDING_KEY+HTML_Push_Key(2));
				ga.addcoderc(ga.readcoderc(),2);
				server.send(200, "text/html", WebPage());
			});

	server.on("/acd", []()      //dodaj kod klawisza d
			{
				server.send(200, "text/html", HTML_ADDING_KEY+HTML_Push_Key(3));
				ga.addcoderc(ga.readcoderc(),3);
				server.send(200, "text/html", WebPage());
			});

	server.on("/save", []()      // zapisz zmiany ustawień
			{
				//	   saveEEProm();
				server.send(200, "text/html", WebPage());
			});

	server.begin();                // Start serwera www
#ifdef DEBUG
	Serial.println("Server started");
#endif
}


