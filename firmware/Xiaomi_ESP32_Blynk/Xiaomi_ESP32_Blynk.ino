#define BLYNK_PRINT Serial

#include <WiFi.h>
#include <WiFiClient.h>
#include <Preferences.h>
#include <Esp32WifiManager.h>
#include <BlynkSimpleEsp32.h>
#include "BLEDevice.h"
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"

String receivedTemperatureValue = "";
String receivedHumidityValue = "";
#define uS_TO_S_FACTOR 1000000  //Conversion factor for micro seconds to seconds
#define TIME_TO_SLEEP  5       //Time ESP32 will go to sleep (in seconds)
RTC_DATA_ATTR int bootCount = 0;
RTC_DATA_ATTR int reconnectCount = 0;
long OnTime1 = 250; 
long OnTime2 = 360;
unsigned long previousMillis1 = 0;
unsigned long previousMillis2 = 0;


static BLEAddress *addressOfOurThermometer;
BLERemoteService* remoteServiceOfTheThermometer;
static BLERemoteCharacteristic* characteristicOfTheTemperatureMeasurementValue;
BLERemoteDescriptor* descriptorForStartingAndEndingNotificationsFromCharacteristic;
BLEClient*  thisOurMicrocontrollerAsClient;
unsigned long startTime PROGMEM ;

// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "238dc3bbbcfc4ed39a97c212d51f313a";

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "kenken64";
char pass[] = "7730112910100";
BlynkTimer timer;

class theEventsThatWeAreInterestedInDuringScanning: public BLEAdvertisedDeviceCallbacks {                    
  void onResult(BLEAdvertisedDevice advertisedDevice) {                                                      
    if (advertisedDevice.getName() == "MJ_HT_V1") {                                                          
      advertisedDevice.getScan()->stop();                                                                    
      addressOfOurThermometer = new BLEAddress(advertisedDevice.getAddress()); } } };                        

static void notifyAsEachTemperatureValueIsReceived(BLERemoteCharacteristic* pBLERemoteCharacteristic, uint8_t* receivedNotification, size_t length, bool isNotify) { 
  unsigned long currentMillis = millis();
  if((currentMillis - previousMillis2 >= OnTime2))
  {
    delay(500);                                                                                                               
    for (int i=2; i<=5; i++) {
      receivedTemperatureValue += (char)*(receivedNotification+i); 
    }
  
    for (int i=9; i<=12; i++) {
      receivedHumidityValue += (char)*(receivedNotification+i); 
    }
    Serial.println(receivedTemperatureValue);
    Serial.println(receivedHumidityValue);
    if (receivedTemperatureValue.length() < 0 && receivedHumidityValue.length() < 0) return;
    delay(4000);
    Blynk.virtualWrite(V0, receivedTemperatureValue);
    Blynk.virtualWrite(V1, receivedHumidityValue);
    delay(20000);
    Serial.println("Disconnect from BLE device.");
    WiFi.disconnect(true);
    delay(4000);
    //thisOurMicrocontrollerAsClient->disconnect();
    //hibernate();
    previousMillis2 = currentMillis;
    ESP.restart();
  }
  
} 

void hibernate() {
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
  Serial.println("Setup ESP32 to sleep for every " + String(TIME_TO_SLEEP) +
  " Seconds");
  Serial.println("Going to deep sleep now.");
  Serial.flush();
  //WiFi.disconnect(false,true);
  ++bootCount;
  Serial.print("bootCount = " + bootCount);
  if(bootCount > 7){
    ESP.restart();
  }
  delay(3000);
  esp_deep_sleep_start();
}


void readTempHumidity() {
  unsigned long currentMillis = millis();
  if((currentMillis - previousMillis1 >= OnTime1))
  {
    if (thisOurMicrocontrollerAsClient->isConnected() == false) {
      thisOurMicrocontrollerAsClient->disconnect(); 
      delay(20); 
      thisOurMicrocontrollerAsClient->connect(*addressOfOurThermometer); 
      startTime = millis(); 
    } // Here the our ESP32 as a client asks for a connection to the desired target device.
    
    if( thisOurMicrocontrollerAsClient->isConnected() == false ) {
      Serial.println(F("e4 Connection couln't be established"));
    }
    
    if (remoteServiceOfTheThermometer == nullptr) { 
      remoteServiceOfTheThermometer = thisOurMicrocontrollerAsClient->getService("226c0000-6476-4566-7562-66734470666d"); 
    }                                            
    
    if (remoteServiceOfTheThermometer == nullptr) {
      thisOurMicrocontrollerAsClient->disconnect(); 
    }                  
    
    if (characteristicOfTheTemperatureMeasurementValue == nullptr) { 
      characteristicOfTheTemperatureMeasurementValue = remoteServiceOfTheThermometer->getCharacteristic("226caa55-6476-4566-7562-66734470666d"); 
    }    
    
    if (characteristicOfTheTemperatureMeasurementValue == nullptr) {
      thisOurMicrocontrollerAsClient->disconnect(); 
    } 
  
    if(characteristicOfTheTemperatureMeasurementValue != nullptr){
      characteristicOfTheTemperatureMeasurementValue->registerForNotify(notifyAsEachTemperatureValueIsReceived); 
    }
    
    if (descriptorForStartingAndEndingNotificationsFromCharacteristic == nullptr) { 
      descriptorForStartingAndEndingNotificationsFromCharacteristic = characteristicOfTheTemperatureMeasurementValue->getDescriptor(BLEUUID((uint16_t)0x2902));
    }

    if (descriptorForStartingAndEndingNotificationsFromCharacteristic == nullptr) {
      thisOurMicrocontrollerAsClient->disconnect(); 
    } 
    
    uint8_t startNotifications[2] = {0x01,0x00}; 
    if(descriptorForStartingAndEndingNotificationsFromCharacteristic != nullptr){
      descriptorForStartingAndEndingNotificationsFromCharacteristic->writeValue(startNotifications, 2, false);      
    }
                                                                                                                                                                    // Ideas: https://stackoverflow.com/questions/1269568/how-to-pass-a-constant-array-literal-to-a-function-that-takes-a-pointer-without
    startTime = millis(); 
    while( ( (millis() - startTime) < 5000) && (receivedTemperatureValue.length() < 4) )
    { 
      if (thisOurMicrocontrollerAsClient->isConnected() == false) {
      } 
    }
    
    characteristicOfTheTemperatureMeasurementValue->registerForNotify(NULL);
    uint8_t endNotifications[2] = {0x00,0x00}; 
    descriptorForStartingAndEndingNotificationsFromCharacteristic->writeValue(endNotifications, 2, false);
    
    if (receivedTemperatureValue.length() < 4) Serial.println(F("e14 No proper temperature measurement value catched."));
    BLEScan* myBLEScanner = BLEDevice::getScan();
    myBLEScanner->setActiveScan(false);
    thisOurMicrocontrollerAsClient->disconnect();
    previousMillis1 = currentMillis;
  }
}

void setup() {
  Serial.begin(115200);
  delay(300);
  Serial.println(WiFi.getMode());
  WiFi.mode(WIFI_STA);
  delay(1000);
  Serial.print("WIFI status = ");
  Serial.println(WiFi.getMode());
  // End silly stuff !!!
  Blynk.begin(auth, ssid, pass, IPAddress(188,166,206,43), 80);
  Blynk.syncAll();
  
  timer.setInterval(1000L, readTempHumidity);
  timer.setInterval(30*1000, reconnectBlynk); //run every 30s

  BLEDevice::init("");
  BLEScan* myBLEScanner = BLEDevice::getScan();
  myBLEScanner->setAdvertisedDeviceCallbacks(new theEventsThatWeAreInterestedInDuringScanning());
  myBLEScanner->setActiveScan(true);
  while (addressOfOurThermometer == nullptr) {
    myBLEScanner->start(30); startTime=millis();
    while ( (millis()-startTime <50) && (addressOfOurThermometer == nullptr) ) { delay(1); } }
  thisOurMicrocontrollerAsClient = BLEDevice::createClient();
}

void reconnectBlynk() {
  if (!Blynk.connected()) {
    Serial.println("Lost connection");
    if(Blynk.connect()) {
      ++reconnectCount;
      Serial.println("Reconnected");
      if(reconnectCount > 6){
        ESP.restart();
      }
    }
    else {
      Serial.println("Not reconnected");
    }
  }
}

void loop() {
  Blynk.run();
  timer.run();
}
