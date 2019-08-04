#define BLYNK_PRINT Serial


#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <DHT.h>

// Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "238dc3bbbcfc4ed39a97c212d51f313a";

// WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "kenken64";
char pass[] = "7730112910100";


#define DHTPIN 22          // What digital pin is connected to
const int soilpin = 32;
const int POWER_PIN = 34;
const int LIGHT_PIN = 33;

#define DHTTYPE DHT11     // DHT 11
//#define DHTTYPE DHT22   // DHT 22, AM2302, AM2321
//#define DHTTYPE DHT21   // DHT 21, AM2301

DHT dht(DHTPIN, DHTTYPE);
BlynkTimer timer;

// This function sends Arduino's up time every second to Virtual Pin (5).
// In the app, Widget's reading frequency should be set to PUSH. This means
// that you define how often to send data to Blynk App.
void sendSensor()
{
  float h = dht.readHumidity();
  float t = dht.readTemperature(); // or dht.readTemperature(true) for Fahrenheit

  int soilmoisture = analogRead(soilpin);
  int lightlevel = analogRead(LIGHT_PIN);


  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  if (isnan(t)) {
    return;
  }
  if (t > 25) {
    Blynk.notify(String("Teperature is too high: ") + t);
  }
  // send any value at any time.
  // No more that 10 values per second.
  Blynk.virtualWrite(V5, h);
  Blynk.virtualWrite(V6, t);
  Blynk.virtualWrite(V7, soilmoisture);
  Blynk.virtualWrite(V8, lightlevel);
}



void setup()
{
  // Debug console
  Serial.begin(9600);
  Blynk.begin(auth, ssid, pass);
  dht.begin();
  // Setup a function to be called every second
  timer.setInterval(1000L, sendSensor);
}

void loop()
{
  Blynk.run();
  timer.run();
}
