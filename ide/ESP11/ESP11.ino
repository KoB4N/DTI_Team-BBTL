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

const float alpha = 0.2; // Smoothing factor, adjust as needed

// Initialize smoothed RSSI value
float smoothedRSSI = 0;

const char* message = "esp11";
uint8_t ledR = 39; // ESP32-S3 pin for red LED | GPIO 39 
uint8_t ledG = 40; // ESP32-S3 pin for green LED | GPIO 40 
uint8_t ledB = 41; // ESP32-S3 pin for blue LED | GPIO 41 

const boolean invert = true; // true: common anode logic '1' | false: common cathode logic '0' 
 
String hexValue_S = "#005ac8";
//7bffff04
String lastEightChars = "";

int prevRSSI = 0;
int prevShade = 0;

double dist_percent = 1.0;

BLEAdvertising *pAdvertising; // BLE Advertisement type
BLEScan *pBLEScan;            // BLE Scan type
BLECharacteristic *pCharacteristic; // BLE Characteristic for sending message

void setRGBColor(uint8_t R, uint8_t G, uint8_t B) { 
    // Print debug information 
    // Serial.println("Setting RGB values:"); 
    // Serial.print("R: "); Serial.println(R); 
    // Serial.print("G: "); Serial.println(G); 
    // Serial.print("B: "); Serial.println(B); 
 
    // Set PWM duty cycle for each color 
    ledcWrite(1, R); 
    ledcWrite(2, G); 
    ledcWrite(3, B); 
}

void ledSetup() {
    // Setup PWM channels for each LED pin 
    ledcAttachPin(ledR, 1); // assign RGB led pins to channels 
    ledcAttachPin(ledG, 2); 
    ledcAttachPin(ledB, 3); 
    
    // Initialize PWM channels 
    ledcSetup(1, 12000, 8); // 12 kHz PWM, 8-bit resolution 
    ledcSetup(2, 12000, 8); 
    ledcSetup(3, 12000, 8); 

}

void extractRGB(String hexColor, uint8_t& red, uint8_t& green, uint8_t& blue) {
    // Convert hex color string to uint32_t
    uint32_t colorValue = (uint32_t)strtol(hexColor.c_str(), NULL, 16);

    // Extract RGB components
    red = (colorValue >> 16) & 0xFF;
    green = (colorValue >> 8) & 0xFF;
    blue = colorValue & 0xFF;
}

class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks {
    void onResult(BLEAdvertisedDevice advertisedDevice) {
        
        // int pinValue = digitalRead(GPIO_PIN);
        // delay(2000);
        // if(pinValue == 0) { 
        // if (false) { 
            if (advertisedDevice.haveName() && advertisedDevice.getName() == SERVER_NAME) {
                for (int i = 0; i < advertisedDevice.getServiceUUIDCount(); i++) {
                    BLEUUID serviceUUID = advertisedDevice.getServiceUUID(i);
                    String uuidStr = serviceUUID.toString().c_str();
                    // Extract the last 8 characters of the UUID
                    String newLastEightChars = uuidStr.substring(28);
                    if (uuidStr.indexOf("a07498ca-ad5b-474e-940d-16f") != -1 && newLastEightChars != lastEightChars) {
                        lastEightChars = newLastEightChars;
                        Serial.println(lastEightChars); // when this is printed, it means the user init new color on the website
                        // Convert lastEightChars to std::string for manipulation
                        std::string stdLastEightChars = lastEightChars.c_str();
                        stdLastEightChars.erase(stdLastEightChars.size() - 2);
                        lastEightChars = stdLastEightChars.c_str();
                        hexValue_S = "#" + lastEightChars;
                        uint32_t hexValue = (uint32_t)strtol(hexValue_S.substring(1).c_str(), NULL, 16); // Skip the '#' character 
            
                        // Extract RGB components 
                        uint8_t R = (hexValue >> 16) ; // Red component 
                        uint8_t G = (hexValue >> 8);  // Green component 
                        uint8_t B = hexValue ;         // Blue component 
                        
                        // Set LED color based on RGB values 
                        // setRGBColor(R, G, B); 
                        
                        // Print the hex value and the corresponding RGB components 
                        // Serial.print("Hex Value: "); 
                        // Serial.print(hexValue_S); 
                        // Serial.print("\nRGB: "); 
                        
                        delay(1000);
                    }
                // }
            } 
        } 
        // else if (pinValue == 1) { // switch back later 1
        
             if (advertisedDevice.getName().find("esp11") != std::string::npos ) { // Filter unwanted signals
                std::string namePrefix = advertisedDevice.getName().substr(0, 7); // Get hex
                Serial.println(namePrefix.c_str());
                int rssi = advertisedDevice.getRSSI(); // Retrieve RSSI value
                Serial.println(rssi); // Print RSSI

                // Extract RGB components from original color hex string
                uint8_t originalRed = strtol(hexValue_S.substring(1, 3).c_str(), NULL, 16);
                uint8_t originalGreen = strtol(hexValue_S.substring(3, 5).c_str(), NULL, 16);
                uint8_t originalBlue = strtol(hexValue_S.substring(5, 7).c_str(), NULL, 16);
                // Smooth the RSSI value using EMA
                smoothedRSSI = alpha * rssi + (1 - alpha) * smoothedRSSI;
                int darkness_level = map(smoothedRSSI, -60, -20, 10, 1); // Map smoothed RSSI to darkness level (1 to 10)

                // Calculate darker shades
                int darkerRed = originalRed * (10 - darkness_level) / 10;
                int darkerGreen = max(originalGreen * (10 - darkness_level) / 10, 0); 
                int darkerBlue = max(originalBlue * (10 - darkness_level) / 10, 0);


                // Set LED color
                setRGBColor(darkerRed, darkerGreen, darkerBlue);
                Serial.println(darkerRed );
                Serial.println(darkerGreen);
                Serial.println(darkerBlue);
            }
        // }
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
    ledSetup();

    Serial.begin(115200);

    pinMode(GPIO_PIN, INPUT);

    advertisingSetup();
    characteristicSetup();
    scanningSetup();
}  

void loop() {
  pAdvertising->start();
}
