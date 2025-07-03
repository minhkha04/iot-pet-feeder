#define BLYNK_PRINT Serial

#define BLYNK_TEMPLATE_ID "TMPL6dnBSDBrj"
#define BLYNK_TEMPLATE_NAME "Feeding System"
#define BLYNK_AUTH_TOKEN "P3kMzT00nMmCn2tu3JF6oB4ys9B3MTa1"

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <Servo.h>
#include <HX711.h>

// WiFi và Blynk
char ssid[] = "MINHKHA";
char pass[] = "11111111";

// Servo
Servo myServo;
int onPos = 80;            // Vị trí bật của servo
int offPos = 0;            // Vị trí tắt của servo
int lastPosition = offPos; // Vị trí hiện tại của servo
bool moving = false;       // Trạng thái di chuyển của servo
unsigned long lastMoveTime = 0;
int moveDelay = 20;        // Thời gian trễ giữa mỗi bước di chuyển servo
bool autoMode = false;

// Load cell
#define DT_PIN D5   // Chân Data kết nối với D5
#define SCK_PIN D6  // Chân Clock kết nối với D6
HX711 scale;
int fill = 30;
int stop = 80;

// Relay (máy bơm)
int myRelay = D1;
bool pumpState = false;  // 0: tắt, 1: bật

// Cảm biến nước
#define waterSensor A0
#define SENSOR_MIN 0      // Giá trị nhỏ nhất của cảm biến (0%)
#define SENSOR_MAX 280    // Giá trị lớn nhất của cảm biến (100%)
int waterLevel = 0;       // Giá trị đọc từ cảm biến
int waterPercentage = 0;  // Phần trăm nước trong khay

// Biến kiểm soát thời gian
unsigned long previousMillis = 0;    // Lưu thời điểm lần đọc cuối cùng
const unsigned long interval = 500;  // Khoảng thời gian giữa các lần đọc (ms)

void setup() {
  Serial.begin(9600);
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  // Servo
  myServo.attach(D0);
  myServo.write(offPos);

  // Máy bơm
  pinMode(myRelay, OUTPUT);
  digitalWrite(myRelay, LOW);

  // Load cell
  scale.begin(DT_PIN, SCK_PIN);
  scale.tare();

  // Khởi tạo giá trị Blynk
  Blynk.virtualWrite(V3, 0);  // Mặc định chế độ Manual
  Blynk.virtualWrite(V0, 0);  // Mặc định Servo ở trạng thái Off
  Blynk.virtualWrite(V1, 0);  // Mặc định Pump ở trạng thái Off
}

void loop() {
  Blynk.run();                      // Giữ Blynk hoạt động
  moveServoSmoothly(lastPosition);  // Điều khiển servo mượt mà

  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    readAndSendWeight();  // Đọc cảm biến Load cell và điều khiển servo
    readWaterSensor();    // Đọc cảm biến nước và điều khiển máy bơm
  }
}

// Hàm đọc và điều khiển servo từ Load cell
void readAndSendWeight() {
  if (scale.is_ready()) {
    long reading = scale.get_units() / 1000;
    Serial.print("Trọng lượng: ");
    Serial.println(reading);

    // Gửi dữ liệu đến Blynk
    Blynk.virtualWrite(V2, reading);

    if (autoMode) {  // Chỉ tự động nếu đang ở chế độ Auto
      if (reading <= fill && lastPosition != onPos) {
        lastPosition = onPos;
        moving = true;
        Blynk.virtualWrite(V0, 1);
        Serial.println("Servo mở (Auto).");
      } else if (reading >= stop && lastPosition != offPos) {
        lastPosition = offPos;
        moving = true;
        Blynk.virtualWrite(V0, 0);
        Serial.println("Servo đóng (Auto).");
      }
    }
  }
}

// Hàm đọc cảm biến nước và điều khiển máy bơm
void readWaterSensor() {
  waterLevel = analogRead(waterSensor);

  waterPercentage = map(waterLevel, SENSOR_MIN, SENSOR_MAX, 0, 100);
  waterPercentage = constrain(waterPercentage, 0, 100);

  Serial.print("Water Level (raw): ");
  Serial.println(waterLevel);
  Serial.print("Water Level (%): ");
  Serial.println(waterPercentage);
  Blynk.virtualWrite(V4, waterPercentage);

  if (autoMode) {  // Chỉ tự động nếu đang ở chế độ Auto
    if (waterPercentage < 65 && !pumpState) {
      digitalWrite(myRelay, HIGH);
      pumpState = true;
      Blynk.virtualWrite(V1, 1);
      Serial.println("Máy bơm bật (Auto).");
    } else if (waterPercentage >= 75 && pumpState) {
      digitalWrite(myRelay, LOW);
      pumpState = false;
      Blynk.virtualWrite(V1, 0);
      Serial.println("Máy bơm tắt (Auto).");
    }
  }
}

// Hàm điều khiển servo mượt mà
void moveServoSmoothly(int targetPos) {
  if (!moving) return;

  int currentPos = myServo.read();  // Đọc vị trí hiện tại của servo
  if (currentPos != targetPos) {
    unsigned long currentTime = millis();
    if (currentTime - lastMoveTime >= moveDelay) {
      // Tăng tốc bằng cách di chuyển nhiều bước hơn mỗi lần
      int step = 5;  // Tăng số bước di chuyển (mặc định là 1)
      if (abs(currentPos - targetPos) < step) {
        currentPos = targetPos;  // Đặt trực tiếp nếu gần đến vị trí mục tiêu
      } else {
        currentPos += (currentPos < targetPos) ? step : -step;  // Di chuyển nhanh hơn
      }
      myServo.write(currentPos);  // Ghi giá trị góc mới cho servo
      lastMoveTime = currentTime;
    }
  } else {
    moving = false;  // Dừng khi đạt mục tiêu
  }
}


// Xử lý chuyển chế độ Manual/Auto
BLYNK_WRITE(V3) {
  int mode = param.asInt();
  autoMode = (mode == 1);
  Serial.println(autoMode ? "Chế độ: Auto" : "Chế độ: Manual");

  if (!autoMode) {  // Nếu chuyển sang chế độ Manual
    // Tắt máy bơm
    digitalWrite(myRelay, LOW);
    pumpState = false;
    Blynk.virtualWrite(V1, 0);  // Cập nhật trạng thái máy bơm trên ứng dụng Blynk
    Serial.println("Manual Mode: Máy bơm tắt.");

    // Đưa servo về trạng thái tắt
    lastPosition = offPos;
    moving = true;             // Bắt đầu quá trình di chuyển servo
    Blynk.virtualWrite(V0, 0); // Cập nhật trạng thái servo trên ứng dụng Blynk
    Serial.println("Manual Mode: Servo đóng.");
  }
}

// Điều khiển servo thủ công qua Blynk
BLYNK_WRITE(V0) {
  if (!autoMode) {
    int buttonValue = param.asInt();
    if (buttonValue == 1 && lastPosition != onPos) {
      lastPosition = onPos;
      moving = true;
      Serial.println("Servo mở (Manual).");
    } else if (buttonValue == 0 && lastPosition != offPos) {
      lastPosition = offPos;
      moving = true;
      Serial.println("Servo đóng (Manual).");
    }
  }
}

// Điều khiển máy bơm thủ công qua Blynk
BLYNK_WRITE(V1) {
  if (!autoMode) {
    int buttonValue = param.asInt();
    if (buttonValue == 1 && !pumpState) {
      digitalWrite(myRelay, HIGH);
      pumpState = true;
      Serial.println("Máy bơm bật (Manual).");
    } else if (buttonValue == 0 && pumpState) {
      digitalWrite(myRelay, LOW);
      pumpState = false;
      Serial.println("Máy bơm tắt (Manual).");
    }
  }
}