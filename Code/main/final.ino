#include <Keypad.h>
#include <Keypad_I2C.h>
#include <SPI.h>
#include <Wire.h>
#include "SSD1306Ascii.h"
#include "SSD1306AsciiAvrI2c.h"
#include <Time.h>
#include <DS1307RTC.h>
#include "Fingerprint.h"
#include <Arduino.h>
#include <SD.h>

#define SCREEN_WIDTH 128  // OLED display width, in pixels
#define SCREEN_HEIGHT 64  // OLED display height, in pixels
#define OLED_RESET 4
#define SCREEN_ADDRESS 0x3C
#define I2CADDR 0x38
#define ANALOG_IN_PIN A0

//finger variable
byte s;
uint8_t id = 0;
uint8_t finger_id = 0;
int pw;

tmElements_t tm;
const int chipSelect = 10;

SSD1306AsciiAvrI2c oled;
// Floats for ADC voltage & Input voltage
float adc_voltage = 0.0;
float in_voltage = 0.0;

// Floats for resistor values in divider (in ohms)
float R1 = 30000.0;
float R2 = 7500.0;

// Float for Reference Voltage
float ref_voltage = 5;

// Integer for ADC value
int adc_value = 0;

const byte numRows = 4;  // số lượng hàng trên keypad
const byte numCols = 4;  // số lượng cột trên keypad

// sử dụng keypad để điều khiển menu
char customKey;
int currentMenuItem = 0;
bool menuMode = true;
bool itemSelected = false;
bool redrawMenu = true;
const byte numMenuItems = 3;
char menuItems[numMenuItems][20] = {
  "NHAP VAN TAY",
  "QUET VAN TAY",
  "XOA VAN TAY"
};

// ánh xạ phím nhấn với các giá trị chữ cái và số
char keymap[numRows][numCols] = {
  { '1', '2', '3', 'A' },
  { '4', '5', '6', 'B' },
  { '7', '8', '9', 'C' },
  { '*', '0', '#', 'D' }
};

// phím được gán trên các hàng và cột
byte rowPins[numRows] = { 7, 6, 5, 4 };
byte colPins[numCols] = { 3, 2, 1, 0 };

// khởi tạo Keypad
Keypad_I2C customKeypad(makeKeymap(keymap), rowPins, colPins, numRows, numCols, I2CADDR);

// các hàm thực thi
void delay_millis(unsigned long ms) {
  unsigned long current_time = millis();
  while (millis() - current_time < ms) {
    // Chờ
  }
}
uint8_t readnumber() {
  unsigned long startTime = millis();
  String inputString;
  uint8_t num = 0;
  while (num == 0 && millis() - startTime < 7000) {
    customKey = customKeypad.getKey();
    if (customKey) {
      Serial.print(customKey);
      if (customKey >= '0' && customKey <= '9') {
        inputString += customKey;
      } else if (customKey == '#') {
        if (inputString.length() > 0) {
          num = inputString.toInt();
        }
      } else if (customKey == '*') {
        inputString = "";
      }
    }
  }

  if (num == 0) {  // nếu không nhận được dữ liệu
    return 255;    // trả về một giá trị khác để biểu thị không nhận được dữ liệu
  } else {
    return num;
  }
}
void showMenu() {
  oled.clear();
  oled.setCursor(0, 0);
  oled.print("         MENU\n");
  oled.setCursor(0, 10);
  for (int i = 0; i < numMenuItems; i++) {
    if (currentMenuItem == i) {
      oled.print(">");
    } else {
      oled.print(" ");
    }
    oled.print(menuItems[i]);
    oled.println();
  }
  oled.displayRows();
  redrawMenu = false;
}

void selectMenuItem() {
  itemSelected = true;
  switch (currentMenuItem) {
    case 0:
      // Thực hiện chức năng 1
      oled.clear();
      oled.setCursor(0, 0);
      oled.print("\n");
      oled.print("    NHAP MAT KHAU\n");
      pw = readnumber();
      if (pw == 145) {
        oled.clear();
        oled.setCursor(0, 0);
        oled.print("\n");
        oled.print("  NHAP ID VAN TAY\n");
        oled.displayRows();
        id = readnumber();
        enrollFingerID(id, s);
        oled.print("Nhap van tay thanh cong");
        delay_millis(1000);
      } else {
        oled.print("    SAI MAT KHAU\n");
        menuMode = true;
      }
      break;
    case 1:
      // Thực hiện chức năng 2
      oled.clear();
      oled.setCursor(0, 0);
      oled.print("\n");
      oled.print("     QUET VAN TAY");
      oled.displayRows();
      scanFinger(finger_id);
      if (finger_id != 0) {
        oled.clear();
        oled.print("\n");
        oled.print("     ID phu hop");
        // delay_millis(1000);
        // oled.clear();
        menuMode = true;
      } else {
        oled.print("\n");
        oled.print("  ID khong phu hop");
        menuMode = true;
      }
      break;
    case 2:
      // Thực hiện chức năng 3
      oled.clear();
      oled.setCursor(0, 0);
      oled.print("\n");
      oled.print("    NHAP MAT KHAU\n");
      pw = readnumber();
      if (pw == 145) {
        oled.clear();
        oled.setCursor(0, 10);
        oled.print("\n");
        oled.print("     XOA VAN TAY\n");
        oled.displayRows();
        id = readnumber();
        deleteFingerID(id);
        oled.print("Xoa van tay thanh cong");
        delay_millis(1000);
      } else {
        oled.print("    SAI MAT KHAU\n");
        menuMode = true;
      }
      break;
  }
}

void readKey() {
  // đọc phím nhấn từ keypad
  customKey = customKeypad.getKey();

  // điều khiển menu
  if (menuMode) {
    if (redrawMenu) {
      showMenu();
    }
    if (customKey) {
      switch (customKey) {
        case 'A':
          if (currentMenuItem > 0) {
            currentMenuItem--;
            redrawMenu = true;
          }
          break;
        case 'B':
          if (currentMenuItem < numMenuItems - 1) {
            currentMenuItem++;
            redrawMenu = true;
          }
          break;
        case 'C':
          selectMenuItem();
          break;
        case 'D':
          menuMode = true;
          redrawMenu = true;
          break;
      }
    }
  } else {
    // quay trở lại menu nếu đã hoàn thành chức năng
    if (itemSelected) {
      menuMode = true;
      itemSelected = false;
      redrawMenu = true;
    }
  }
}

void volRead() {
  // Read the Analog Input
  adc_value = analogRead(ANALOG_IN_PIN);

  // Determine voltage at ADC input
  adc_voltage = (adc_value * ref_voltage) / 1024.0;

  // Calculate voltage at divider input
  in_voltage = adc_voltage / (R2 / (R1 + R2));

  int phantrampin = (in_voltage / ref_voltage) * 10;

  oled.setCursor(99, 0);
  oled.print(phantrampin);
  oled.println("%");
  oled.displayRows();
}

void dongho() {
  if (RTC.read(tm)) {
    //hien thi gio, phut
    oled.setCursor(0, 0);
    print2digits(tm.Hour);
    oled.write(':');
    print2digits(tm.Minute);
    oled.displayRows();
  }
}

void print2digits(int number) {
  if (number >= 0 && number < 10) {
    oled.write('0');
  }
  oled.print(number);
}

void setup() {
  Wire.begin();
  Serial.begin(9600);
  customKeypad.begin();
  oled.begin(&Adafruit128x64, SCREEN_ADDRESS);
  oled.setFont(Adafruit5x7);
  fingerprintSetup();
  pinMode(SS, OUTPUT);
  if (!SD.begin(chipSelect)) {
    Serial.println(F("Not found SD"));
    return;
  }
  Serial.println("SD ready");
}
void SDcard() {
  File myFile = SD.open("test.txt", FILE_WRITE);
  if (myFile) {
    if (finger_id != 0) {
      myFile.print(F("ID: "));
      myFile.print(finger_id);
    }
    myFile.print(F(","));
    myFile.print(tm.Hour);
    myFile.print(F(":"));
    myFile.print(tm.Minute);
    myFile.print(F(":"));
    myFile.print(tm.Second);
    myFile.print(F(","));
    myFile.print(tm.Day);
    myFile.print(F("/"));
    myFile.print(tm.Month);
    myFile.print(F("/"));
    myFile.print(tmYearToCalendar(tm.Year));
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening file");
  }
}
void loop() {
  readKey();
  dongho();
  volRead();
  // SDcard();
  finger_id = 0;
  // Cập nhật đồng hồ và pin mỗi phút một lần
  static unsigned long last_update = 0;
  if (millis() - last_update >= 60000) {
    redrawMenu = true;
    last_update = millis();
  }
}