#include <Wire.h>
#include <LiquidCrystal.h>
#include <MPU6050.h>

MPU6050 mpu;
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

// مداخل الفليكس سنسورز
int flexPins[5] = {A0, A1, A2, A3, A4};
int flexValues[5];

void setup() {
  Wire.begin();
  mpu.initialize();
  lcd.begin(16, 2);
  lcd.print("Sign Language");
  delay(1000);
  lcd.clear();
}

void loop() {
  // قراءة الفليكسات
  for (int i = 0; i < 5; i++) {
    flexValues[i] = analogRead(flexPins[i]);
  }

  // قراءة الميل من MPU6050
  int16_t ax, ay, az;
  mpu.getAcceleration(&ax, &ay, &az);
  float angle = atan2(ay, az) * 57.3; // زاوية اليد التقريبية

  // حساب المتوسط بتاع الفليكس
  int avgFlex = (flexValues[0] + flexValues[1] + flexValues[2] + flexValues[3] + flexValues[4]) / 5;

  lcd.clear();

  // تحديد الكلمة بناءً على الزوايا
  if (avgFlex > 400 && avgFlex < 600 && angle > 20 && angle < 40) {
    lcd.setCursor(0, 0);
    lcd.print("3amel ae?");
  }
  else if (avgFlex > 700 && avgFlex < 900 && angle > 50 && angle < 70) {
    lcd.setCursor(0, 0);
    lcd.print("اelhamdollah");
  }
  else {
    lcd.setCursor(0, 0);
    lcd.print("...");
  }

  delay(500);
}
