#include <Keypad.h>
#include <Keypad_I2C.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128  // OLED display width, in pixels
#define SCREEN_HEIGHT 64  // OLED display height, in pixels
#define OLED_RESET 4
#define I2CADDR 0x38

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const byte numRows = 4;  // số lượng hàng trên keypad
const byte numCols = 4;  // số lượng cột trên keypad

// sử dụng keypad để điều khiển menu
char customKey;
int currentMenuItem = 0;
bool menuMode = true;
bool itemSelected = false;
bool redrawMenu = true;
const byte numMenuItems = 6;
char menuItems[numMenuItems][20] = {
  "NHAP VAN TAY",
  "QUET VAN TAY",
  "XOA VAN TAY",
  "XUAT THONG TIN",
  "NHAP TT CA NHAN",
  "TINH LUONG"
};

// ánh xạ phím nhấn với các giá trị chữ cái và số
char keymap[numRows][numCols] = {
  { '1', '2', '3', 'A' },
  { '4', '5', '6', 'B' },
  { '7', '8', '9', 'C' },
  { '*', '0', '#', 'D' }
};

// phím được gán trên các hàng và cột
byte rowPins[numRows] = {7,6,5,4};  // các hàng từ 0 đến 3
byte colPins[numCols] = {3,2,1,0};  // các cột từ 0 đến 3

// khởi tạo Keypad
Keypad_I2C customKeypad( makeKeymap(keymap), rowPins, colPins, numRows, numCols, I2CADDR);

// các hàm thực thi
void showSplash() {
  String splashString = "MAY CHAM CONG";
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.setCursor(64 - (splashString.length() * 3), 0);
  display.print(splashString);
  display.setTextSize(1);
  splashString = "by";
  display.setCursor(64 - (splashString.length() * 5), 16);
  display.print(splashString);
  display.setTextSize(1);
  splashString = "Khanh Hoang";
  display.setCursor(64 - (splashString.length() * 3), 32);
  display.print(splashString);
   display.setTextSize(1);
  splashString = "&";
  display.setCursor(64 - (splashString.length() * 5), 43);
  display.print(splashString);
  display.setTextSize(1);
  splashString = "Viet Hoang";
  display.setCursor(64 - (splashString.length() * 3), 53);
  display.print(splashString);
  display.display();
  delay(3000);
}

void showMenu() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.print("         MENU");
  display.setCursor(0, 10);
  for (int i = 0; i < numMenuItems; i++) {
    if (currentMenuItem == i) {
      display.setTextColor(BLACK, WHITE);
      display.print(">");
    } else {
      display.setTextColor(WHITE);
      display.print(" ");
    }
    display.print(menuItems[i]);
    display.println();
  }
  display.display();
  redrawMenu = false;
}

void selectMenuItem() {
  itemSelected = true;
  switch (currentMenuItem) {
    case 0:
      // Thực hiện nhập vân tay
      display.clearDisplay();
      display.setTextSize(1);
      display.setTextColor(WHITE);
      display.setCursor(0, 0);
      display.print("     NHAP VAN TAY");

      display.display();
      break;
    case 1:
      // Thực hiện quét vân tay
      display.clearDisplay();
      display.setTextSize(1);
      display.setTextColor(WHITE);
      display.setCursor(0, 0);
      display.print("     QUET VAN TAY");

      display.display();
      break;
    case 2:
      // Thực hiện xoá vân tay
      display.clearDisplay();
      display.setTextSize(1);
      display.setTextColor(WHITE);
      display.setCursor(0, 0);
      display.print("     XOA VAN TAY");

      display.display();
      break;
    case 3:
      // Thực hiện xuất thông tin
      display.clearDisplay();
      display.setTextSize(1);
      display.setTextColor(WHITE);
      display.setCursor(0, 0);
      display.print("    XUAT THONG TIN");

      display.display();
      break;
    case 4:
      // Thực hiện tính lương
      display.clearDisplay();
      display.setTextSize(1);
      display.setTextColor(WHITE);
      display.setCursor(0, 0);
      display.print("   NHAP TT CA NHAN");

      display.display();
      break;
    case 5:
      // Thực hiện nhập thông tin cá nhân
      display.clearDisplay();
      display.setTextSize(1);
      display.setTextColor(WHITE);
      display.setCursor(0, 0);
      display.print("      TINH LUONG");

      display.display();
      break;
  }
}

void setup() {
  Wire.begin();
  customKeypad.begin( );
  // khởi tạo màn hình OLED
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  showSplash();

}

void loop() {
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