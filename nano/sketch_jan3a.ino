#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>

#define COIN_PIN 2
#define BTN_PIN 3
#define SERVO_PIN 4
#define LED_RED 5
#define LED_GREEN 6

LiquidCrystal_I2C lcd(0x27,16,2);
Servo motor;

volatile int pulseCount = 0;
unsigned long lastPulseTime = 0;

int totalMoney = 0;
const int PRICE = 5;

bool lastBtn = HIGH;     // สำหรับตรวจจับการกด 1 ครั้ง
bool vending = false;   // ล็อกไม่ให้กดซ้ำ

void coinISR(){
  pulseCount++;
  lastPulseTime = millis();
}

void setRed(){
  digitalWrite(LED_RED, LOW);
  digitalWrite(LED_GREEN, HIGH);
}

void setGreen(){
  digitalWrite(LED_RED, HIGH);
  digitalWrite(LED_GREEN, LOW);
}

void show(){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Money: ");
  lcd.print(totalMoney);
  lcd.setCursor(0,1);
  lcd.print("Item: ");
  lcd.print(totalMoney / PRICE);
}

void setup(){
  Serial.begin(9600);

  pinMode(COIN_PIN, INPUT_PULLUP);
  pinMode(BTN_PIN, INPUT_PULLUP);
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);

  motor.attach(SERVO_PIN);

  lcd.init();
  lcd.backlight();

  attachInterrupt(digitalPinToInterrupt(COIN_PIN), coinISR, FALLING);

  setRed();
  lcd.setCursor(0,0);
  lcd.print("Insert Coin");
  lcd.setCursor(0,1);
  lcd.print("Price 5 Baht");
}

void loop(){

  // ===== รับเหรียญ =====
  if (pulseCount > 0 && millis() - lastPulseTime > 300){
    int c = pulseCount;
    pulseCount = 0;
    totalMoney += c;
    show();
  }

  int canVend = totalMoney / PRICE;

  // ===== แสดงสถานะ =====
  if (canVend <= 0){
    setRed();
    lcd.setCursor(0,1);
    lcd.print("Add Coin       ");
  } else {
    setGreen();
    lcd.setCursor(0,1);
    lcd.print("Press Button   ");
  }

  // ===== ตรวจปุ่ม (กดครั้งเดียว) =====
  bool btn = digitalRead(BTN_PIN);

  if (!vending && lastBtn == HIGH && btn == LOW){
    // 👉 กดปุ่ม “ครั้งเดียว”
    if (canVend > 0){
      vending = true;

      for (int i = 0; i < canVend; i++){
        motor.write(90);
        delay(1200);
        motor.write(0);
        delay(600);
      }

      totalMoney = 0;
      show();
      setRed();

      Serial.println("BUY_EVENT");

      vending = false;
    }
  }

  lastBtn = btn;
}
