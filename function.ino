void WiFiBegin() {
  Serial.println("--- Setup Network ---");
  boolean scan = Connections.scanWifi(ssid.c_str());
  printf("scan : %d", scan);
  if (scan == false) {
    dual_wifi = 1;
    Serial.println("AP Mode - Cannot Found SSID " + String(ssid.c_str()));
    dataNetwork network = Connections.setAP(ssid_ap.c_str(), password_ap.c_str());
    Serial.println("IP Address AP : " + String(network.ip.c_str()));
    Serial.println("MAC Address : " + String(network.mac.c_str()));
    ip_ap = network.ip;
    ip = "Unset";
    mac = network.mac;
  } else {
    dual_wifi = 2;
    Serial.println("Dual Mode WiFi");
    dualNetwork network = Connections.dualWifi(ssid.c_str(), password.c_str(), ssid_ap.c_str(), password_ap.c_str());
    Serial.println("IP Address AP : " + String(network.ip_ap.c_str()));
    Serial.println("IP Address Station : " + String(network.ip.c_str()));
    Serial.println("MAC Address Station : " + String(network.mac.c_str()));
    Serial.println("MAC Address AP : " + String(network.mac_AP.c_str()));
    ip_ap = network.ip_ap;
    ip = network.ip;
    mac_station = network.mac;
    mac = network.mac_AP;
    Serial.println("SSID STA: " + ip);

  }
  WiFiState();
  Serial.println("---------------------");
  Serial.println("");
  trigger = 0;
}

void WiFiState() {
  statusNetwork network = Connections.statusWifi();
  status_wifi = network.status;
  wifi_rssi = String(network.rssi);
}

void WiFiHandle() {
  if (dual_wifi == 1) {
    WiFiBegin();
    delay(1000);
  }
}



void readFlash() {
  Serial.println("--- Read Flash ---");
  String ssid_station  = flash.readFile("/ssid_sta.txt");
  String pass_station = flash.readFile("/pass_sta.txt");
  Serial.println("SSID Flash : " + ssid_station);
  Serial.println("Password Flash : " + pass_station);
  //  ssid = ssid_station;
  //  password = pass_station;
  ssid = "KONTRAKAN ALUMNI";
  password = "TMUA1443H";
  String minimumDistanceFlash = flash.readFile("/value1.txt");
  String maximumDistanceFlash = flash.readFile("/value2.txt");
  minDistance = minimumDistanceFlash.toInt();
  maxDistance = maximumDistanceFlash.toInt();
  Serial.println("Minimum Distance : " + minimumDistanceFlash);
  Serial.println("Maximum Distance : " + maximumDistanceFlash);
  Serial.println("------------------");
  Serial.println("");
}
