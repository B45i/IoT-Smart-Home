#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>
#include <FirebaseArduino.h>
#include <ArduinoJson.h> //From FirebaseArduino
#include <FS.h>

#define FIREBASE_HOST "iotsmarthome-309fa.firebaseio.com"
#define FIREBASE_AUTH "mkE2Ktf36bS7urHXX0g8wyg7lHUeyZi0Nr5zU3pV"

//String username =  "MVs13qV0Txgxh3u8zIXk5VaOJDZ2";

int noPins = 8;
int pins[] = {D0, D1, D2, D3, D4, D5, D6, D7};

char UID[64] = "";

bool shouldSaveConfig = false;

void saveConfigCallback () {
  Serial.println("Should save config");
  shouldSaveConfig = true;
}

void loadSavedConfig() {

  Serial.println("mounting FS...");

  if (SPIFFS.begin()) {
    Serial.println("mounted file system");
    if (SPIFFS.exists("/config.json")) {
      //file exists, reading and loading
      Serial.println("reading config file");
      File configFile = SPIFFS.open("/config.json", "r");
      if (configFile) {
        Serial.println("opened config file");
        size_t size = configFile.size();
        // Allocate a buffer to store contents of the file.
        std::unique_ptr<char[]> buf(new char[size]);

        configFile.readBytes(buf.get(), size);
        DynamicJsonBuffer jsonBuffer;
        JsonObject& json = jsonBuffer.parseObject(buf.get());
        json.printTo(Serial);
        if (json.success()) {
          Serial.println("\nparsed json");
          strcpy(UID, json["UID"]);
        }
        else {
          Serial.println("failed to load json config");
        }
      }
    }
  }
  else {
    Serial.println("failed to mount FS");
  }
}


void setupConnection() {
  // id/name placeholder/prompt default length
  WiFiManagerParameter custom_UID("UID", "UID", UID, 64);
  WiFiManager wifiManager;
  wifiManager.setSaveConfigCallback(saveConfigCallback);
  wifiManager.addParameter(&custom_UID);
  wifiManager.autoConnect("IoT SmartHome");
  // default IP: 192.168.4.1
  Serial.println("connected...");

  strcpy(UID, custom_UID.getValue());

  // save  to FS
  if (shouldSaveConfig) {
    Serial.println("saving config");
    DynamicJsonBuffer jsonBuffer;
    JsonObject& json = jsonBuffer.createObject();
    json["UID"] = UID;

    File configFile = SPIFFS.open("/config.json", "w");
    if (!configFile) {
      Serial.println("failed to open config file for writing");
    }

    json.printTo(Serial);
    json.printTo(configFile);
    configFile.close();
  }

  Serial.print("local ip");
  Serial.println(WiFi.localIP());

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
}

void setupPins() {

  for (int i = 0; i < 8; i++) {
    pinMode(pins[i], OUTPUT);
  }

}

void togglePins(int pin) {

  String pinPath = UID;
  switch (pin) {
    case 0:
      pinPath += "/b1";
      break;

    case 1:
      pinPath += "/b2";
      break;

    case 2:
      pinPath += "/b3";
      break;

    case 3:
      pinPath += "/b4";
      break;

    case 4:
      pinPath += "/b5";
      break;

    case 5:
      pinPath += "/b6";
      break;

    case 6:
      pinPath += "/b7";
      break;

    case 7:
      pinPath += "/b8";
      break;
  }

  bool pinStatus  = Firebase.getInt(pinPath);
  if (Firebase.failed()) {
    Serial.print("getting value failed ");
    Serial.println(Firebase.error());
    return;
  }
  else {
    Serial.println(pinPath + " " + pinStatus);
    digitalWrite(pins[pin], pinStatus);
  }
}

void setup() {
  Serial.begin(9600);

  //clean FS, for testing
  //SPIFFS.format();
  setupPins();
  loadSavedConfig();
  setupConnection();

}

void loop() {
  for (int i = 0; i < noPins; i++) {
    togglePins(i);
  }
  //Serial.print("User name :");
  //Serial.println(UID);

}


