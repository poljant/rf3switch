#include <ESP8266WebServer.h>
#include <ESP8266HTTPUpdateServer.h>
#include "Relay.h"
//#include "RCSwitch.h"
#include "RFSwitch.h"
#include "RF3Switch.h"
//extern String version;
extern Relay r1;
extern Relay r2;
extern Relay r3;
extern RFSwitch RFrelay1;
extern RFSwitch RFrelay2;
extern RFSwitch RFrelay3;
extern void saveEEProm(void);

//extern String slocation;

// login i hasło do sytemu
const char* www_login = "admin";
const char* www_pass = "esp8266";

const int port = 80;                 // port serwera www
ESP8266WebServer server(port);
ESP8266HTTPUpdateServer httpUpdate;

String HTMLHeader() {           //  nagłówek strony
//	String HTMLHeader() {                //  nagłówek strony
	String  h = "<!DOCTYPE html>\n";
	  h += "<html>";
	  h += "<head>";
	  h += "<title> RF3Switch</title>";
	//  h += "<meta http-equiv=\"Refresh\" content=\"30\" />";  //odświerzaj stronę co 30 sek.
	  h += "<meta charset=\"utf-8\">";
	  h += "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">";
	  h += "<link rel=\"stylesheet\" href=\"https://maxcdn.bootstrapcdn.com/bootstrap/3.3.4/css/bootstrap.min.css\" >";
	  h += "</head>";
	  h += "<body style=\"text-align: center;color: white; background: black;font-size: 1.5em;\">\n";
	  return h;
	}

String HTMLFooter() {             //  stopka strony www
	String  f = "";

	  f += "<p><a href = \"/save\"><button class=\"btn btn-info\">Zapisz zmiany</button></a></p>";
	  f += "<p><a href = \"/\"><button class=\"btn btn-info\">Odświerz stronę</button></a></p>";
	  f += "<p>Jan Trzciński &copy; 2016-2017</p></td></tr>";
	  f += "</body>\n";
	  f += "</html>\n";
	  return f;
	}

String HTMLPage1() {      // pierwsza część strony www
	 String t;
	 unsigned long sec = millis() / 1000;
	 unsigned long min = (sec / 60);
	 unsigned long hour = (min / 60);
	 unsigned long days = hour / 24;
	 sec = sec % 60;
	 min = min % 60;
	 hour = hour % 24;
	 t  = "<h1>3 Przekaźniki RF</h1>";
	 t += "<p> Wersja oprogramowania 1.0 z dnia 2-03-2017";
//	 t += (version);
	 t += "</p>";
	 t += "<p>Czas od uruchomienia dni: ";
	 t += (days);
	 t += " godz:" ;
	 t += ((hour<10) ? "0" : "");  //gdy mniej niż 10 wstaw zero wiodące
	 t += (hour);
	 t += ":";
	 t += ((min<10) ? "0" : "");  //gdy mniej niż 10 wstaw zero wiodące
	 t += (min);
	 t += ":";
	 t += ((sec < 10) ? "0" : "");  //gdy mniej niż 10 wstaw zero wiodące
	 t += (sec);
	 t += "</p>";
	 return t;
	}

String HTMLPage2() {            // druga część strony www
	  String p = "";
	  p += "<p>Uruchomiony na WeMos D1 mini</p>\n";
	  p += ( (r1.read()) ? "<p><a href = \"/relay1/0\"><button class=\"btn btn-danger\">Przekaźnik 1 ON</button></a></p>\n" : "<p><a href = \"/relay1/1\"><button class=\"btn btn-success\">Przekaźnik 1 OFF</button></a></p>\n");
	  p += ( (r2.read()) ? "<p><a href = \"/relay2/0\"><button class=\"btn btn-danger\">Przekaźnik 2 ON</button></a></p>\n" : "<p><a href = \"/relay2/1\"><button class=\"btn btn-success\">Przekaźnik 2 OFF</button></a></p>\n");
	  p += ( (r3.read()) ? "<p><a href = \"/relay3/0\"><button class=\"btn btn-danger\">Przekaźnik 3 ON</button></a></p>\n" : "<p><a href = \"/relay3/1\"><button class=\"btn btn-success\">Przekaźnik 3 OFF</button></a></p>\n");
	  p += "<p>Połączenia: nadajnik RF DATA na D8, VCC na 5V, odbiornik RF DATA na D5 VCC na D0</p>\n";
	  p += "<p>Połączenia: przekaźnik 1 na D1, przekaźnik 2 na D2, przekaźnik 3 na D3</p>\n";
	  p += "<p><a href = \"/setcod1\"><button class=\"btn btn-info\"> KodOn1=" + String(RFrelay1.getCodOn()) + " </button> </a> ";
	  p += "<a href = \"/setcod2\"><button class=\"btn btn-info\"> KodOn2=" + String(RFrelay2.getCodOn()) + "</button></a>  ";
	  p += "<a href = \"/setcod3\"><button class=\"btn btn-info\"> KodOn3=" + String(RFrelay3.getCodOn()) + "</button></a></p>\n";

	  return p;
	}


 String WebPage() {       // połącz wszystkie części strony www
  return (HTMLHeader()+HTMLPage1()+ HTMLPage2()+HTMLFooter());
 }

// funkcja ustawia wszystkie strony www
void setservers(){
	 httpUpdate.setup(&server,"/update", www_login, www_pass); // umożliwia aktualizację poprzez WiFi

	 server.on("/", [](){
	    server.send(200, "text/html", WebPage());
	  });

	 server.on("/relay1/0", [] ()     //  wyłącz przekaźnik 1
	  {
	    r1.setOff();
	    RFrelay1.sendOff();
	    server.send(200, "text/html", WebPage());
	  });

	 server.on("/relay1/1", []()      // załącz przekaźnik 1
	  {
	    r1.setOn();
	    RFrelay1.sendOn();
	    server.send(200, "text/html", WebPage());
	  });

	 server.on("/relay2/0", [] ()     // wyłącz przekaźnik 2
	  {
	   r2.setOff();
	   RFrelay2.sendOff();
	   server.send(200, "text/html", WebPage());
	  });

	 server.on("/relay2/1", []()      // załącz przekaźnik 2
	  {
	   r2.setOn();
	   RFrelay2.sendOn();
	   server.send(200, "text/html", WebPage());
	  });
	 server.on("/relay3/0", [] ()     // wyłącz przekaźnik 3
	  {
	   r3.setOff();
	   RFrelay3.sendOff();
	   server.send(200, "text/html", WebPage());
	  });

	 server.on("/relay3/1", []()      // załącz przekaźnik 3
	  {
	   r3.setOn();
	   RFrelay3.sendOn();
	   server.send(200, "text/html", WebPage());
	  });

	 server.on("/setcod1", []()      // ustaw kod RFrelay1
	  {
	    RFrelay1.readCod(RFPINRX);
	   server.send(200, "text/html", WebPage());
	  });

	 server.on("/setcod2", []()      // ustaw kod RFrelay2
	  {
		RFrelay2.readCod(RFPINRX);
	   server.send(200, "text/html", WebPage());
	  });

	 server.on("/setcod3", []()      // ustaw kod RFrelay3
	  {
		RFrelay3.readCod(RFPINRX);
	   server.send(200, "text/html", WebPage());
	  });

	 server.on("/save", []()      // zapisz zmiany ustawień
	  {
		   saveEEProm();
	   server.send(200, "text/html", WebPage());
	  });

 server.begin();                // Start serwera www
#ifdef DEBUG
 Serial.println("Server started");
#endif
}

