uint8_t ledR = 39; // ESP32-S3 pin for red LED | GPIO 39 
uint8_t ledG = 40; // ESP32-S3 pin for green LED | GPIO 40 
uint8_t ledB = 41; // ESP32-S3 pin for blue LED | GPIO 41 
 
const boolean invert = true; // true: common anode logic '1' | false: common cathode logic '0' 
 
String hexValue_S = "#005ac8"; 
 
// Function to set PWM duty cycle for each color 
void setRGBColor(uint8_t R, uint8_t G, uint8_t B) { 
    // Print debug information 
    Serial.println("Setting RGB values:"); 
    Serial.print("R: "); Serial.println(R); 
    Serial.print("G: "); Serial.println(G); 
    Serial.print("B: "); Serial.println(B); 
 
    // Set PWM duty cycle for each color 
    ledcWrite(1, R); 
    ledcWrite(2, G); 
    ledcWrite(3, B); 
} 
 
void setup() { 
    // Setup PWM channels for each LED pin 
    ledcAttachPin(ledR, 1); // assign RGB led pins to channels 
    ledcAttachPin(ledG, 2); 
    ledcAttachPin(ledB, 3); 
     
    // Initialize PWM channels 
    ledcSetup(1, 12000, 8); // 12 kHz PWM, 8-bit resolution 
    ledcSetup(2, 12000, 8); 
    ledcSetup(3, 12000, 8); 
     
    // Initialize Serial communication 
    Serial.begin(115200); 
} 
 
void loop() { 
    // Convert hex string to integer 
    uint32_t hexValue = (uint32_t) strtol(hexValue_S.substring(1).c_str(), NULL, 16); // Skip the '#' character 
 
    // Extract RGB components 
    uint8_t R = (hexValue >> 16) ; // Red component 
    uint8_t G = (hexValue >> 8);  // Green component 
    uint8_t B = hexValue ;         // Blue component 
     
    // Set LED color based on RGB values 
    setRGBColor(R, G, B); 
     
    // Print the hex value and the corresponding RGB components 
    Serial.print("Hex Value: "); 
    Serial.print(hexValue_S); 
    Serial.print("\nRGB: "); 
     
    delay(2000); 
}