/*
 * WebPages.h
 *
 *  Created on: 5 lip 2022
 *      Author: jant
 */

#ifndef GATEAUTOMATION_WEBPAGES_H_
#define GATEAUTOMATION_WEBPAGES_H_

#include <ESP8266WebServer.h>
#include <ESP8266HTTPUpdateServer.h>


String HTMLHeader();
String HTMLFooter();
String HTMLPage();
String HTMLPage1();
String HTMLPage2();
String HTML_Push_Key(int nk);
void setservers(void);


#endif /* GATEAUTOMATION_WEBPAGES_H_ */
