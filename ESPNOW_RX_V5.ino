#include <SimpleKalmanFilter.h>

#include <esp_now.h>
#include <WiFi.h>
#include <ESP32Servo.h>
int potAngle;
// Structure example to receive data
// Must match the sender structure
typedef struct struct_message {
    int potValue;     // Giá trị từ biến trở
    int joyXValue;    // Giá trị từ trục X của joystick
    //int joyYValue;    // Giá trị từ trục Y của joystick
    bool buttonState; // Trạng thái nút bấm của joystick
} struct_message;

// Create a struct_message called myData
struct_message myData;

const int ledPin = 2;  // Chân kết nối LED
const int servoPin1 = 25; // Chân kết nối servo 1
const int R_PWM = 26;
const int R_EN = 27;
const int L_PWM = 14;
const int L_EN = 13;

Servo servo1; // Servo điều khiển bằng biến trở

SimpleKalmanFilter kalmanFilterPot(52, 52, 0.1); // Thông số cho biến trở
SimpleKalmanFilter kalmanFilterJoyX(52, 52, 0.1); // Thông số cho trục X của joystick

// callback function that will be executed when data is received
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&myData, incomingData, sizeof(myData));

   // Áp dụng bộ lọc Kalman cho các giá trị đọc
  float filteredPotValue = kalmanFilterPot.updateEstimate(myData.potValue);
  float filteredJoyXValue = kalmanFilterJoyX.updateEstimate(myData.joyXValue);



      // Xuất giá trị trước và sau khi lọc ra Serial Monitor
//  Serial.print("Biến trở: "); Serial.print(myData.potValue);
//  Serial.print(" Filtered Pot: "); Serial.print(filteredPotValue);
//  Serial.print(" ,Joystick X: "); Serial.print(myData.joyXValue);
//  Serial.print(" Filtered JoyX: "); Serial.println(filteredJoyXValue);


    if (filteredPotValue > 3700)  { potAngle = map(filteredPotValue, 3700, 4095, 90, 145); servo1.write(potAngle);}
    else if (filteredPotValue < 300)  { potAngle = map(filteredPotValue, 300, 0, 90, 50); servo1.write(potAngle);}
   // int potAngle = map(filteredPotValue, 0, 4095, 50, 145);
    //servo1.write(potAngle);

     int joyX = filteredJoyXValue;
      if (joyX > 2000 && joyX < 3500) {
    
    digitalWrite(R_PWM, HIGH);
    digitalWrite(L_PWM, LOW);
    int speed = map(joyX, 2000, 3499, 0, 160);
    analogWrite(R_EN, speed);
    analogWrite(L_EN, speed);
  } else if (joyX >= 3500) {
    // Động cơ quay tiến
    digitalWrite(R_PWM, HIGH);
    digitalWrite(L_PWM, LOW);
    int speed = map(joyX, 3500, 4095, 160, 255);
    analogWrite(R_EN, speed);
    analogWrite(L_EN, speed);
    } else if (joyX < 1800) {
    
    digitalWrite(R_PWM, LOW);
    digitalWrite(L_PWM, HIGH);
    int speed = map(joyX, 1800, 0, 0, 120);
    analogWrite(L_EN, speed);
    analogWrite(R_EN, speed);
    } else {
    // Động cơ dừng lại
    digitalWrite(R_PWM, LOW);
    digitalWrite(L_PWM, LOW);
    analogWrite(R_EN, 0);
    analogWrite(L_EN, 0);
  }
}


 
void setup() {
  // Initialize Serial Monitor
  Serial.begin(115200);
  
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  
  // Once ESPNow is successfully Init, we will register for recv CB to
  // get recv packer info
  esp_now_register_recv_cb(esp_now_recv_cb_t(OnDataRecv));
          // Gắn servo vào chân điều khiển
    servo1.attach(servoPin1);

  pinMode(R_PWM, OUTPUT);
  pinMode(R_EN, OUTPUT);
  pinMode(L_PWM, OUTPUT);
  pinMode(L_EN, OUTPUT);

}
 
void loop() {


}
