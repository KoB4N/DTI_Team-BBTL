#include "BLEDevice.h"
#include "BLEUtils.h"
#include "BLEServer.h"
#include "BLEBeacon.h"
#include "esp_sleep.h"

#define GPIO_DEEP_SLEEP_DURATION 10  // sleep x seconds and then wake up

//adress 30:30:f9:72:a3:2d esp11
// 12345678-1234-5678-1234-56789abcdef1 esp11
RTC_DATA_ATTR static time_t last;        // remember last boot in RTC Memory
RTC_DATA_ATTR static uint32_t bootcount; // remember number of boots in RTC Memory

BLEAdvertising *pAdvertising; // BLE Advertisement type
BLEScan *pBLEScan;            // BLE Scan type
BLECharacteristic *pCharacteristic; // BLE Characteristic for sending message

struct timeval now;

#define BEACON_UUID "12345678-1234-5678-1234-56789abcdef1" // UUID 1 128-Bit
const char* message = "#d694e esp21";
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
    strServiceData += "#d699ee esp11";
    strServiceData += (char)26;   // Len
    strServiceData += (char)0xFF; // Type
    strServiceData += oBeacon.getData();

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

class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks {
    void onResult(BLEAdvertisedDevice advertisedDevice) {
        // Check device address instead of name
        if (advertisedDevice.getAddress().toString() == "30:30:f9:72:a3:2d") { 
            Serial.printf("Received Advertisement: Name: %s, Address: %s, txPower: %d, rssi: %d\n",
                          advertisedDevice.getName().c_str(),
                          advertisedDevice.getAddress().toString().c_str(),
                          advertisedDevice.getTXPower(),
                          advertisedDevice.getRSSI());
          } if (advertisedDevice.getName().find("esp11") != std::string::npos) { // Change "esp21" to your desired message
              std::string namePrefix = advertisedDevice.getName().substr(0, 7);
              Serial.println(namePrefix.c_str());
              Serial.println(advertisedDevice.getRSSI());
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
        pBLEScan->start(5); // Scan for 10 seconds
        delay(1000); // Wait for 2 seconds
    }
}

void characteristicSetup() {
    // Create BLE server
    BLEServer *pServer = BLEDevice::createServer();
    BLEService *pService = pServer->createService(BLEUUID("12345678-1234-5678-1234-56789abcdef0")); // Create a service

    // Create a BLE characteristic
    pCharacteristic = pService->createCharacteristic(
        BLEUUID("12345678-1234-5678-1234-56789abcdef0"),
        BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_NOTIFY
    );

    // Start the service
    pService->start();
}

void setup() {
    Serial.begin(115200);
    Serial.printf("start ESP32 %d\n", bootcount++);

    advertisingSetup();
    scanningSetup();
    characteristicSetup();
}   

void loop() {
    // Handle sender functionality (advertising)
    pAdvertising->start();
}
