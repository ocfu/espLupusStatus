
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

#define MY_SSID "xxx"
#define MY_KEY  "xxx"

#define GET_STATUS  0
#define GET_WINDOW1 1
#define GET_WINDOW2 2
 
#define STATUS_DISARMED 0
#define STATUS_ARMED 1
#define STATUS_HOME 2

#define BLINK_PERIOD 1000   // ms

unsigned long last_blink = 0;


const char *ssid = MY_SSID;
const char *password = MY_KEY;

ESP8266WebServer server(80);

const int led_red = D1;
const int led_green1 = D2;
const int led_green2 = D3;
const int led_yellow = D5;   // D4 is the internal LED on NodeMCU

unsigned cnt_red = 0;
unsigned cnt_green1 = 0;
unsigned cnt_green2 = 0;
unsigned cnt_yellow = 0;
unsigned cnt_door = 0;
bool sta_door = false;     // true, if door is open

int lupus_status = STATUS_DISARMED;

const char* getLEDStatus(int n) {
  switch (n) {
    case GET_STATUS:
      if (lupus_status == STATUS_ARMED)
        return "armed";
      else if (lupus_status == STATUS_HOME)
        return "home";
      else
        return "disarmed";
      break;
    case GET_WINDOW1:
      if (digitalRead(led_green1) == HIGH)
        return "open";
      else
        return "closed";
      break;
    case GET_WINDOW2:
      if (digitalRead(led_green2) == HIGH)
        return "open";
      else
        return "closed";
      break;
  }
  return "";
}


void handleRoot() {
  char http[1000];
  // be careful, max. stack size for ESP8266 is 4kB. If you need bigger pages, then consider
  // to use flash (PROGMEM or SPIFF). This one here is quick and dirty ;-)

  int sec = millis() / 1000;
  int min = sec / 60;
  int hr = min / 60;

  server.setContentLength(CONTENT_LENGTH_UNKNOWN);

  snprintf(http, sizeof(http) - 1,
    "<!DOCTYPE html>\
    <html>\
    <head>\
        <meta http-equiv='refresh' content='5'/>\
        <title>ESP8266 Lupus Status</title>\
        <style>\
          body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #000000  }\
          table, th, td {\
               border: 1px solid black;\
               border-collapse: collapse;\
          }\
          th, td {\
            padding: 5px;\
          }\
          th {\
            text-align: left;\
          }\
        </style>\
    </head>");
    
  http[sizeof(http) - 1] = '\0';
  server.send(200, "text/html", http);

  snprintf(http, sizeof(http) - 1,
    "<body>\
    <h1>Lupus Status</h1>\
    <table style=\"width:50%%\">\
        <tr><th></th><th>Status</th><th>Zaehler</th></tr>\
    ");
  http[sizeof(http) - 1] = '\0';
  server.sendContent(http);
        
  snprintf(http, sizeof(http) - 1,
    "<tr><td>Zentrale:</td><td>%s</td><td>%d (armed)/%d (home)</td></tr>\
    ", getLEDStatus(GET_STATUS), cnt_red, cnt_yellow);
  http[sizeof(http) - 1] = '\0';
  server.sendContent(http);
    
  snprintf(http, sizeof(http) - 1,
    "<tr><td>Fenster OG:</td><td>%s</td><td>%d</td></tr>\
    ", getLEDStatus(GET_WINDOW1), cnt_green1);
  http[sizeof(http) - 1] = '\0';
  server.sendContent(http);
    
  snprintf(http, sizeof(http) - 1,
    "<tr><td>Fenster EG:</td><td>%s</td><td>%d</td></tr>\
    ", getLEDStatus(GET_WINDOW2), cnt_green2);
  http[sizeof(http) - 1] = '\0';
  server.sendContent(http);
    
  snprintf(http, sizeof(http) - 1,
    "<tr><td>Haustuer:</td><td>%s</td><td>%d</td></tr>\
    ", sta_door ? "open" : "closed", cnt_door);
  http[sizeof(http) - 1] = '\0';
  server.sendContent(http);
  
  snprintf(http, sizeof(http) - 1,
    "</table>\
    <p>Uptime: %02d:%02d:%02d</p>\
    <form action=\"/resetcnt\">\
        <input type=\"submit\" value=\"Zaehler zuruecksetzen\" />\
    </form>\
    </body>\
    </html>\
    ", hr, min % 60, sec % 60);
  http[sizeof(http) - 1] = '\0';
  server.sendContent(http);
  server.client().stop(); // no more content
}

void redirect() {
  server.sendHeader("Location", String("/"), true);
  server.send ( 302, "text/plain", "");
}

void ok() {
  server.send (200);
}


void setup(void) {
  pinMode(led_red, OUTPUT);
  pinMode(led_green1, OUTPUT);
  pinMode(led_green2, OUTPUT);
  pinMode(led_yellow, OUTPUT);
  digitalWrite(led_red, LOW);
  digitalWrite(led_green1, LOW);
  digitalWrite(led_green2, LOW);
  digitalWrite(led_yellow, LOW);

  Serial.begin(115200);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("esplupus")) {
    Serial.println("MDNS responder started");
  }

  server.on("/", handleRoot);

  server.on("/arm", []() {
    lupus_status = STATUS_ARMED;
    cnt_red++;
    ok();
  });

  server.on("/home", []() {
    lupus_status = STATUS_HOME;
    cnt_yellow++;
    ok();
  });

  server.on("/wopen1", []() {
    digitalWrite(led_green1, HIGH);
    cnt_green1++;
    ok();
  });

  server.on("/wopen2", []() {
    digitalWrite(led_green2, HIGH);
    cnt_green2++;
    ok();
  });

  server.on("/dopen", []() {
    cnt_door++;
    sta_door = true;
    ok();
  });

  server.on("/disarm", []() {
    lupus_status = 0;
    ok();
  });

  server.on("/wclosed", []() {
    digitalWrite(led_green1, LOW);
    digitalWrite(led_green2, LOW);
    ok();
  });

  server.on("/wclosed1", []() {
    digitalWrite(led_green1, LOW);
    ok();
  });

  server.on("/wclosed2", []() {
    digitalWrite(led_green2, LOW);
    ok();
  });

  server.on("/dclosed", []() {
    sta_door = false;
    ok();
  });

  server.on("/resetcnt", []() {
    cnt_red = 0;
    cnt_green1 = 0;
    cnt_green2 = 0;
    cnt_yellow = 0;
    cnt_door = 0;
    redirect();
  });

  server.onNotFound([]() {
    redirect();
  });

  server.begin();
  Serial.println("HTTP server started");
}

void loop(void) {
  server.handleClient();
  MDNS.update();

  if ((millis() - last_blink) > BLINK_PERIOD) {
    switch (lupus_status) {
      case 1:  // blink red led
        digitalWrite(led_red, !digitalRead(led_red));
        digitalWrite(led_yellow, LOW);
        break;
      case 2:
        digitalWrite(led_yellow, !digitalRead(led_yellow));
        digitalWrite(led_red, LOW);
        break;
      default:
        digitalWrite(led_red, LOW);
        digitalWrite(led_yellow, LOW);
    }
    last_blink = millis();
  }

  delay(10);
}
