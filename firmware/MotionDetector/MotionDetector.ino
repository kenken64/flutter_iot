#include <WiFi.h>
#include <HTTPClient.h>
const int motionSensor PROGMEM = 12;

const char* ssid = "xxx";
const char* password = "xxx";
WiFiClient espClient;

void invokeRequest(String value){
  HTTPClient http;
  http.begin(value); //Specify the URL
  int httpCode = http.GET();                                        //Make the request

  if (httpCode > 0) { //Check for the returning code

      String payload = http.getString();
      Serial.println(httpCode);
      Serial.println(payload);
    }

  else {
    Serial.println("Error on HTTP request");
    ESP.restart();
  }

  http.end(); //Free the resources
  
}
void turnONLight(){
  invokeRequest("http://blynk-cloud.com/85a4ddf0b61149149fdff2506590484c/update/V1?value=255"); //v1 to 255
  delay(50);
  invokeRequest("http://blynk-cloud.com/85a4ddf0b61149149fdff2506590484c/update/V2?value=255"); //v2 to 255
  delay(50);
  invokeRequest("http://blynk-cloud.com/85a4ddf0b61149149fdff2506590484c/update/V3?value=255"); //v3 to 255
  delay(50);
  invokeRequest("http://blynk-cloud.com/238dc3bbbcfc4ed39a97c212d51f313a/update/V3?value=1"); //v3 to 1
  delay(50);
}

void offLight(){
  invokeRequest("http://blynk-cloud.com/85a4ddf0b61149149fdff2506590484c/update/V1?value=0"); //v1 to 0
  delay(50);
  invokeRequest("http://blynk-cloud.com/85a4ddf0b61149149fdff2506590484c/update/V2?value=0"); //v2 to 0
  delay(50);
  invokeRequest("http://blynk-cloud.com/85a4ddf0b61149149fdff2506590484c/update/V3?value=0"); //v3 to 0
  delay(50);
  invokeRequest("http://blynk-cloud.com/238dc3bbbcfc4ed39a97c212d51f313a/update/V3?value=0"); //v3 to 1
  delay(50);
}
void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}


void setup() {
  Serial.begin(115200);
  Serial.setDebugOutput(0);
  pinMode(motionSensor, INPUT);
  setup_wifi();
  printWifiStatus();
  offLight();
}

void loop() {
  int motion = digitalRead(motionSensor);
  Serial.println(motion);
  if(motion == 1){
    Serial.println("MOTION DETECTED!!!");
    Serial.println("LOSER !!!");
    turnONLight();
    delay(15000);
    offLight();
  }
  Serial.println("WINNER !!!");
  delay(2000);
}

void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}
