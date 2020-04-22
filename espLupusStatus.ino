
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

#define MY_SSID "xxx"
#define MY_KEY  "xxx"

#define GET_STATUS  0
#define GET_WINDOW1 1
#define GET_WINDOW2 2


const char *ssid = MY_SSID;
const char *password = MY_KEY;

ESP8266WebServer server(80);

const int led_red = D1;
const int led_green1 = D2;
const int led_green2 = D3;
const int led_yellow = D5;   // D4 is the internal LED on NodeMCU

const char* getLEDStatus(int n) {
  switch (n) {
    case GET_STATUS:
      if (digitalRead(led_red) == HIGH)
        return "armed";
      else if (digitalRead(led_yellow) == HIGH)
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
  // to use flash (PROGMEM or SPIFF) or send to client in parts. This one here is quick and dirty ;-)

  int sec = millis() / 1000;
  int min = sec / 60;
  int hr = min / 60;

  snprintf(http, 1000,
           "<!DOCTYPE html>\
<html>\
    <meta http-equiv='refresh' content='5'/>\
    <title>ESP8266 Lupus Status</title>\
    <style>\
      body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #000000  }\
    </style>\
  </head>\
  <body>\
    <h1>Lupus Status</h1>\
    <table>\
        <tr><td>Status:</td><td>%s</td></tr>\
        <tr><td>Fenster OG:</td><td>%s</td></tr>\
        <tr><td>Fenster EG:</td><td>%s</td></tr>\
    </table>\
    <p>Uptime: %02d:%02d:%02d</p>\
  </body>\
</html>\
",
           getLEDStatus(GET_STATUS), getLEDStatus(GET_WINDOW1), getLEDStatus(GET_WINDOW2), hr, min % 60, sec % 60
          );
  server.send(200, "text/html", http);
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
    digitalWrite(led_red, HIGH);
    digitalWrite(led_yellow, LOW);
    ok();
  });

  server.on("/home", []() {
    digitalWrite(led_red, LOW);
    digitalWrite(led_yellow, HIGH);
    ok();
  });

  server.on("/wopen1", []() {
    digitalWrite(led_green1, HIGH);
    ok();
  });

  server.on("/wopen2", []() {
    digitalWrite(led_green2, HIGH);
    ok();
  });

  server.on("/disarm", []() {
    digitalWrite(led_red, LOW);
    digitalWrite(led_yellow, LOW);
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

  server.onNotFound([]() {
    ok();
  });

  server.begin();
  Serial.println("HTTP server started");
}

void loop(void) {
  server.handleClient();
  MDNS.update();
}
