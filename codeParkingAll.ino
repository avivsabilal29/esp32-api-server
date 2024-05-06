#include <ArduinoJson.h>
#include <SynapsisLibrary.h>
#include <WebServer.h>

Connection Connections;
String hostname = "Parking Device";
String ssid_ap     = "Parking Hotspot";
String password_ap = "12345678";
String ssid = "";
String password = "";
String ip, ip_ap, mac, wifi_, mac_station, wifi_rssi;
int status_wifi = 0; int dual_wifi = 0; int trigger = 0; int statusPark1 = 0; int minDistance = 0; int maxDistance = 0;



WebServer server(80);
Memory flash;

const int TRIGPIN = 18;
const int ECHOPIN = 19;
long timer;
int jarak;

void setup()
{
  Serial.begin(115200);
  flash.begin();
  readFlash();
  setupSensor();
  WiFiBegin();
  handleRoot();
  server.onNotFound(handleNotFound);
  server.begin();
}

void loop()
{
  server.handleClient();
  if (trigger == 1) {
    WiFiBegin();
  }
  WiFiHandle();
}
