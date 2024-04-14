#include "BLEDevice.h"
#include "BLEScan.h"

class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks {
    void onResult(BLEAdvertisedDevice advertisedDevice) {
        Serial.printf("Advertised Device: %s\n", advertisedDevice.toString().c_str());
        if (advertisedDevice.haveServiceUUID()) {
            Serial.print("  Service UUIDs:");
            for (int i = 0; i < advertisedDevice.getServiceUUIDCount(); i++) {
                Serial.print(" ");
                Serial.print(advertisedDevice.getServiceUUID(i).toString().c_str());
            }
            Serial.println();
        }
    }
};

void setup() {
    Serial.begin(115200);
    Serial.println("Starting BLE Scan...");

    BLEDevice::init("");
    BLEScan* pBLEScan = BLEDevice::getScan();
    pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
    pBLEScan->setActiveScan(true);
    pBLEScan->start(5); // Scan for 10 seconds
    
    Serial.println("Scanning...");
}

void loop() {
    // You can add your own code here if needed
}
