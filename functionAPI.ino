void handleRoot() {

  server.on("/", HTTP_GET, []() {
    server.send(200, F("text/html"),
                F("Welcome to the Parking System"));
  });

  server.on(F("/api/set/network/station"), HTTP_POST, setNetworkStation);
  server.on(F("/api/set/distance-value"), HTTP_POST, setDistanceValue);
  server.on(F("/api/get/sensor/info"), HTTP_GET, getSensorInfo);
  server.on(F("/api/get/network/info"), HTTP_GET, getNetworkInfo);
}

void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  handlePost("Failed", message, 404);
}

void handlePost(const String statusMessage, const String message, const int statusCode) {
  StaticJsonDocument<200> doc;
  doc["status"] = statusMessage;
  doc["message"] = message;
  String data_message;
  serializeJson(doc, data_message);
  server.send(statusCode, "application/json", data_message);
}

void handlePostErorr(const String message, const String dataLabel, const String dataInput) {
  StaticJsonDocument<200> doc;
  doc["status"] = "Failed";
  doc["message"] = message;
  doc["data"][dataLabel] = dataInput;
  String data_message;
  serializeJson(doc, data_message);
  server.send(400, "application/json", data_message);
}


bool isNumeric(const String& str) {
  for (size_t i = 0; i < str.length(); i++) {
    if (!isdigit(str.charAt(i))) {
      return false;
    }
  }
  return true;
}

void setNetworkStation() {
  String data_string = server.arg("plain");
  StaticJsonDocument<400> jDoc;
  DeserializationError err = deserializeJson(jDoc, data_string);
  String ssid_input = jDoc["ssid"];
  String password_input = jDoc["password"];
  flash.writeFile("/ssid_sta.txt", ssid_input.c_str());
  flash.writeFile("/pass_sta.txt", password_input.c_str());
  ssid = ssid_input;
  password = password_input;
  Serial.println("--- Request POST Station Network ---");
  Serial.println("Input SSID : " + ssid_input);
  Serial.println("Password SSID : " + password_input);
  Serial.println("------------------------------------");
  Serial.println("");
  trigger = 1;
  StaticJsonDocument<200> doc;
  doc["status"] = "Success";
  doc["message"] = "Setting Station Network Successfully";
  doc["data"]["ssid"] = ssid;
  doc["data"]["password"] = password;
  String data_message;
  serializeJson(doc, data_message);
  server.send(200, "application/json", data_message);
}


void setDistanceValue() {
  Serial.println("--- Request POST Set Pin AC ---");
  String data_string = server.arg("plain");
  StaticJsonDocument<400> jDoc;
  DeserializationError err = deserializeJson(jDoc, data_string);
  String valueDistance[2] = { jDoc["minDistance"], jDoc["maxDistance"]};
  String errors[2];
  Serial.println("Minimum Distance: " + valueDistance[0]);
  Serial.println("Maximum Distance: " + valueDistance[1]);
  Serial.println("---------------------------");
  Serial.println("");
  for (int i = 0; i < 2; i++) {
    if (valueDistance[i] != "") {
      if (!isNumeric(valueDistance[i])) {
        errors[i] = "variabel must be a number";
        handlePostErorr("Invalid input variabel must be a number", "Value" + String(i + 1), valueDistance[i]);
        Serial.println("Invalid input variabel must be a number");
        return;
      }
      int paramValue = valueDistance[i].toInt();
      //      if (paramValue <= 0) {
      //        errors[i] = "paramValue AC must be greater than 0";
      //        handlePostErorr("Invalid input paramValue AC must be greater than 0", "Value" + String(i + 1), valueDistance[i]);
      //        Serial.println("Invalid input paramValue AC must be greater than 0");
      //        return;
      //      }
      //      for (int j = 0; j < 4; j++) {
      //        if (i != j && paramValue == valueDistance[j].toInt()) {
      //          errors[i] = "Each AC paramValue must have a different number";
      //          handlePostErorr("Invalid input Each AC paramValue must have a different number", "ac" + String(i + 1), valueDistance[i]);
      //          Serial.println("Invalid input Each AC paramValue must have a different number");
      //          return;
      //        }
      //      }
      flash.writeFile(("/value" + String(i + 1) + ".txt").c_str(), valueDistance[i].c_str());
      switch (i) {
        case 0: minDistance = paramValue; break;
        case 1: maxDistance = paramValue; break;
      }
    }
  }

  StaticJsonDocument<200> doc;
  doc["status"] = "Success";
  doc["message"] = "Setting AC Pin Successfully";
  doc["data"]["minimum_distance"] = minDistance;
  doc["data"]["maximum_distance"] = maxDistance;
  String data_message;
  serializeJson(doc, data_message);
  server.send(200, "application/json", data_message);
}


void getNetworkInfo() {
  WiFiState();
  StaticJsonDocument<400> doc;
  String modewifi;
  if (dual_wifi == 1) {
    modewifi = "AP Mode";
  } else {
    modewifi = "Dual Mode";
  }
  doc["status"] = "Success";
  doc["message"] = "Get Network Station Succesfully";
  doc["data"]["mode"] = modewifi;
  doc["data"]["ap"]["mac"] = mac;
  doc["data"]["ap"]["ip"] = ip_ap;
  doc["data"]["ap"]["ssid"] = ssid_ap;
  doc["data"]["ap"]["password"] = password_ap;
  doc["data"]["station"]["mac"] = mac_station;
  doc["data"]["station"]["ip"] = ip;
  doc["data"]["station"]["ssid"] = ssid;
  doc["data"]["station"]["password"] = password;
  doc["data"]["station"]["rssi"] = wifi_rssi;
  doc["data"]["station"]["state"] = status_wifi;
  String data_string;
  serializeJson(doc, data_string);
  Serial.println("--- Request GET Network ---");
  Serial.println("Mode WiFi : " + modewifi);
  Serial.println("MAC Address AP : " + mac);
  Serial.println("MAC Address Station : " + mac_station);
  Serial.println("SSID AP : " + ssid_ap);
  Serial.println("IP Address AP : " + ip_ap);
  Serial.println("SSID Station : " + ssid);
  Serial.println("IP Address Station : " + ip);
  Serial.println("RSSI Station : " + wifi_rssi);
  Serial.println("---------------------------");
  Serial.println("");
  server.send(200, "text/json", data_string);
}


void getSensorInfo() {
  readSensor();
  if (jarak >= minDistance && jarak <= maxDistance) {
    statusPark1 = 1;
  } else {
    statusPark1 = 0;
  }

  StaticJsonDocument<400> doc;
  doc["device"] = "Parking1";
  doc["status"] = "Success";
  doc["message"] = "Get Distance Succesfully";
  doc["data"]["distance"] = jarak;
  doc["data"]["minimum_distance"] = minDistance;
  doc["data"]["maximum_distance"] = maxDistance;
  doc["data"]["status_parking"] = statusPark1;
  String data_string;
  serializeJson(doc, data_string);
  Serial.println("--- Request GET Distance Information ---");
  Serial.println("Device : Parking1");
  Serial.println("Jarak : " + String(jarak));
  Serial.println("Status Parking : " + String(statusPark1));
  Serial.println("---------------------------");
  Serial.println("");
  server.send(200, "text/json", data_string);
}
