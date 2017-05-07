/*
 * RFSwitch3Web.h
 *
 *  Created on: 25.02.2017
 *      Author: Jan Trzciński
 */

#ifndef RF3SWITCHWEB_H_
#define RF3SWITCHWEB_H_

#include <ESP8266WebServer.h>
#include <ESP8266HTTPUpdateServer.h>

String HTMLHeader();
String HTMLFooter();
String HTMLPage();
String HTMLPage1();
String HTMLPage2();
void setservers(void);

#endif /* RF3SWITCHWEB_H_ */
