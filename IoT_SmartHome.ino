#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>
#include <FirebaseArduino.h>
#include <ArduinoJson.h> //From FirebaseArduino
#include <FS.h>

#define FIREBASE_HOST "iot-smart-home-01.firebaseio.com"
#define FIREBASE_AUTH "QwRHZ5YdmTrhlP0CG83lkwJOMRkUABhnCvIY5OzJ"

String username =  "amalshajan2011";

int pins[] = {D1, D2, D3, D4, D5};

char user_name[34] = "";

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
          strcpy(user_name, json["user_name"]);
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
  
  WiFiManagerParameter custom_user_name("user_name", "USER NAME", user_name, 34);
  WiFiManager wifiManager;
  wifiManager.setSaveConfigCallback(saveConfigCallback);
  wifiManager.addParameter(&custom_user_name);
  wifiManager.autoConnect("IoT SmartHome");
  // default IP: 192.168.4.1
  Serial.println("connected...");

  strcpy(user_name, custom_user_name.getValue());

  // save  to FS
  if (shouldSaveConfig) {
    Serial.println("saving config");
    DynamicJsonBuffer jsonBuffer;
    JsonObject& json = jsonBuffer.createObject();
    json["user_name"] = user_name;

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

  for(int i=0;i<5;i++) {
    pinMode(pins[i], OUTPUT);
  }

}

void togglePins(int pin) {
  
  String pinPath = "users/"+username;
  switch(pin){
    case 0:
      pinPath += "/pin1";
    break;

    case 1:
      pinPath += "/pin2";
    break;

    case 2:
      pinPath += "/pin3";
    break;

    case 3:
      pinPath += "/pin4";
    break;

    case 4:
      pinPath += "/pin5";
    break;
  } 

  bool pinStatus  = Firebase.getBool(pinPath);
  if (Firebase.failed()) {
    Serial.print("getting value failed ");
    Serial.println(Firebase.error());
    return;
  }
  else {
    Serial.println(pinPath +" "+pinStatus);
    digitalWrite(pins[pin], pinStatus);
  }
}

void setup() {
  Serial.begin(9600);
  setupPins();
  loadSavedConfig();
  setupConnection();

}

void loop() {
  for(int i =0;i<5;i++){
    togglePins(i);
  }
  Serial.print("User name :");
  Serial.println(user_name);

}


