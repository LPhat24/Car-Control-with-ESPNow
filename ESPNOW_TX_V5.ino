#include <esp_now.h>
#include <WiFi.h>


uint8_t broadcastAddress[] = {0x3C, 0x8A, 0x1F, 0xA2, 0x77, 0xCC}; //3C:8A:1F:A2:77:CC


const int potPin = 34;    // Chân ADC cho biến trở
const int joyXPin = 32;   // Chân ADC cho joystick trục X
const int joyYPin = 33;   // Chân ADC cho joystick trục Y
const int buttonPin = 35; // Chân digital cho nút bấm của joystick
const int ledPin = 2;     // Chân kết nối LED

typedef struct struct_message {
  int potValue;     // Giá trị từ biến trở
    int joyXValue;    // Giá trị từ trục X của joystick
    bool buttonState; // Trạng thái nút bấm của joystick
} struct_message;

// Create a struct_message called myData
struct_message myData;

esp_now_peer_info_t peerInfo;

// callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
//  Serial.print("\r\nLast Packet Send Status:\t");
//  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
  if (status != ESP_NOW_SEND_SUCCESS) {
        digitalWrite(ledPin, HIGH);
        delay(100);
        digitalWrite(ledPin, LOW);
        delay(100);
    } else {
        digitalWrite(ledPin, LOW);
    }
}
 
void setup() {
  // Init Serial Monitor
  Serial.begin(115200);
 
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_register_send_cb(OnDataSent);
  
  // Register peer
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  
  // Add peer        
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    //Serial.println("Failed to add peer");
    return;
  }
      pinMode(buttonPin, INPUT_PULLUP);
    // Cấu hình chân LED là đầu ra
    pinMode(ledPin, OUTPUT);
}
 
void loop() {
  // Set values to send
// Đọc giá trị từ biến trở
    myData.potValue = analogRead(potPin); 
    // Đọc giá trị từ joystick
    myData.joyXValue = analogRead(joyXPin);
    //myData.joyYValue = analogRead(joyYPin); 
    // Đọc trạng thái nút bấm
    myData.buttonState = !digitalRead(buttonPin);
  
  // Send message via ESP-NOW
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));
   
//  if (result == ESP_OK) {
//    Serial.println("Sent with success");
//  }
//  else {
//    Serial.println("Error sending the data");
//  }
//  delay(10);
}
