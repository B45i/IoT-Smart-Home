#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>

#define FIREBASE_HOST "iot-smart-home-01.firebaseio.com"
#define FIREBASE_AUTH "QwRHZ5YdmTrhlP0CG83lkwJOMRkUABhnCvIY5OzJ"
#define WIFI_SSID "www.amalshajan.me"
#define WIFI_PASSWORD "amalshajan.me"

String username =  "amalshajan2011";

int pins[] = {D1, D2, D3, D4, D5};

void setupConnection() {
    // connect to wifi.
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("connecting");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("connected: ");
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
  setupConnection();

}

void loop() {
  for(int i =0;i<5;i++){
    togglePins(i);
  }

}


