#include <BLEDevice.h>
#include <BLEClient.h>

// Define the name of the BLE server
#define SERVER_NAME "LAPTOP-U0NSBAGO"

BLEClient* pClient = nullptr;

class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks {
    void onResult(BLEAdvertisedDevice advertisedDevice) {
        if (advertisedDevice.haveName() && advertisedDevice.getName() == SERVER_NAME) { //check for laptop server
            Serial.println("Found BLE server");
            Serial.printf("Name: %s, Address: %s, txPower: %d, RSSI: %d\n",
                          advertisedDevice.getName().c_str(),
                          advertisedDevice.getAddress().toString().c_str(),
                          advertisedDevice.getTXPower(),
                          advertisedDevice.getRSSI());

            for (int i = 0; i < advertisedDevice.getServiceUUIDCount(); i++) {
                BLEUUID serviceUUID = advertisedDevice.getServiceUUID(i);
                String uuidStr = serviceUUID.toString().c_str();
                // Extract the last 8 characters of the UUID
                String lastEightChars = uuidStr.substring(28);
                if (uuidStr.indexOf("a07498ca-ad5b-474e-940d-16f") != -1) { 
                Serial.println(lastEightChars);
                }
            }
            
            if (!pClient->connect(&advertisedDevice)) {
                Serial.println("Failed to connect to server");
                return;
            }

        }
    }
};

class MyClientCallbacks : public BLEClientCallbacks {
    void onConnect(BLEClient* pclient) {
        Serial.println("Connected to server");
    }

    void onDisconnect(BLEClient* pclient) {
        Serial.println("Disconnected from server");
    }
};

void scanForBLEServer() {
    Serial.println("Scanning for BLE server...");
    BLEScan* pBLEScan = BLEDevice::getScan();
    pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
    pBLEScan->setActiveScan(true);
    while (true) {
        pBLEScan->start(5); // Scan for 5 seconds
        delay(2000); // Wait for 2 seconds
    }
}

void setup() {
    Serial.begin(115200);
    Serial.println("Starting BLE client...");
    BLEDevice::init("");
    pClient = BLEDevice::createClient();
    pClient->setClientCallbacks(new MyClientCallbacks());
    scanForBLEServer();
}

void loop() {
    
}