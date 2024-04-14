#include "BLEDevice.h"
#include "BLEUtils.h"
#include "BLEServer.h"
#include "BLEBeacon.h"
#include "esp_sleep.h"
// Define the name of the BLE server
#define SERVER_NAME "LAPTOP-U0NSBAGO"
// this is esp11
#define GPIO_DEEP_SLEEP_DURATION 10  // sleep x seconds and then wake up
#define BEACON_UUID "12345678-1234-5678-1234-56789abcdef1" // UUID 1 128-Bit
RTC_DATA_ATTR static uint32_t bootcount; // remember number of boots in RTC Memory


#define GPIO_PIN 2


const char* message = "#de2323 esp11";
uint8_t ledR = 39; // ESP32-S3 pin for red LED | GPIO 39 
uint8_t ledG = 40; // ESP32-S3 pin for green LED | GPIO 40 
uint8_t ledB = 41; // ESP32-S3 pin for blue LED | GPIO 41 
 
const boolean invert = true; // true: common anode logic '1' | false: common cathode logic '0' 
 
String hexValue_S = "#005ac8";

String lastEightChars = "";

BLEAdvertising *pAdvertising; // BLE Advertisement type
BLEScan *pBLEScan;            // BLE Scan type
BLECharacteristic *pCharacteristic; // BLE Characteristic for sending message

class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks {
    void onResult(BLEAdvertisedDevice advertisedDevice) {
        int pinValue = digitalRead(GPIO_PIN);
        delay(2000);
        if(pinValue == 0) {
          if (advertisedDevice.haveName() && advertisedDevice.getName() == SERVER_NAME) {
              for (int i = 0; i < advertisedDevice.getServiceUUIDCount(); i++) {
                  BLEUUID serviceUUID = advertisedDevice.getServiceUUID(i);
                  String uuidStr = serviceUUID.toString().c_str();
                  // Extract the last 8 characters of the UUID
                  String newLastEightChars = uuidStr.substring(28);
                  if (uuidStr.indexOf("a07498ca-ad5b-474e-940d-16f") != -1 && newLastEightChars != lastEightChars) {
                      lastEightChars = newLastEightChars;
                      Serial.println(lastEightChars); // when this is printed, it means the user init new color on the website
                  } else {Serial.println("Nth change");}
              }
          } 
        } else if (pinValue == 1) { // switch back later 1
            if (advertisedDevice.getName().find("esp11") != std::string::npos) { // to filter the unwanted signals
                          std::string namePrefix = advertisedDevice.getName().substr(0, 7); // get hex
                          Serial.println(namePrefix.c_str());
                          Serial.println(advertisedDevice.getRSSI()); //printing distance
            }
        }
    }
};


void scanningSetup() {
    BLEDevice::init(message);
    pBLEScan = BLEDevice::getScan();
    BLEScan* pBLEScan = BLEDevice::getScan();
    pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
    pBLEScan->setActiveScan(true);
    
    while (true) {
        pBLEScan->start(2); // Scan for 10 seconds
        delay(500); // Wait for 2 seconds
    }
}

void setBeacon() {
    BLEBeacon oBeacon = BLEBeacon();
    oBeacon.setManufacturerId(0x4C00); // fake Apple 0x004C LSB (ENDIAN_CHANGE_U16!)
    oBeacon.setProximityUUID(BLEUUID(BEACON_UUID));
    oBeacon.setMajor((bootcount & 0xFFFF0000) >> 16);
    oBeacon.setMinor(bootcount & 0xFFFF);

    BLEAdvertisementData oAdvertisementData = BLEAdvertisementData();
    BLEAdvertisementData oScanResponseData = BLEAdvertisementData();

    oAdvertisementData.setFlags(0x04); // BR_EDR_NOT_SUPPORTED 0x04

    std::string strServiceData = "";
    oAdvertisementData.addData(strServiceData);

    pAdvertising->setAdvertisementData(oAdvertisementData);
    pAdvertising->setScanResponseData(oScanResponseData);
}
void advertisingSetup() {
    // Create the BLE Device for sender mode
    BLEDevice::init(message);
    pAdvertising = BLEDevice::getAdvertising();
    BLEDevice::startAdvertising();
    setBeacon();
}

void characteristicSetup() {
    // Create BLE server
    BLEServer *pServer = BLEDevice::createServer();
    BLEService *pService = pServer->createService(BLEUUID("12345678-1234-5678-1234-56789abcdef1")); // Create a service

    // Create a BLE characteristic
    pCharacteristic = pService->createCharacteristic(
        BLEUUID("12345678-1234-5678-1234-56789abcdef1"),
        BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_NOTIFY
    );

    // Start the service
    pService->start();
}

void setup() {
    Serial.begin(115200);

    pinMode(GPIO_PIN, INPUT);

    advertisingSetup();
    characteristicSetup();
    scanningSetup();
}  

void loop() {
  pAdvertising->start();
}
