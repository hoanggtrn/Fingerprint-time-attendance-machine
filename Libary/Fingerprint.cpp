#include "fingerprint.h"
#include <Arduino.h>

SoftwareSerial mySerial(2, 3);  // RX, TX
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

/**
*@brief
- Thiết lập cho Sensor
*@param
*@retval
*/
void fingerprintSetup() {
  unsigned long currentMillis = millis();
  finger.begin(57600);
  if (finger.verifyPassword()) {
    Serial.println(F("Sensor Ready"));
  } else {
    Serial.println(F("Sensor not found"));
    while (1) {
      while (millis() - currentMillis < 1)
        ;
    }
  }
}

/**
*@brief
-	Lưu vân tay vào bộ nhớ
*@param
-	id: ID của vân tay
*@retval
*/
uint8_t enrollFingerID(uint8_t &id, byte &s) {

  int p = -1;
  Serial.print(F("Waiting for valid finger to enroll as #"));
  Serial.println(id);
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
      case FINGERPRINT_OK:
        Serial.println(F("Image taken"));
        break;
      case FINGERPRINT_NOFINGER:
        Serial.println(F("."));
        break;
      case FINGERPRINT_PACKETRECIEVEERR:
        s = 0;
        break;
      case FINGERPRINT_IMAGEFAIL:
        s = 0;
        break;
      default:
        Serial.println(F("Error Occured!!"));
        s = 0;
        break;
    }
  }

  // OK success!

  p = finger.image2Tz(1);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println(F("Image converted"));
      break;
    case FINGERPRINT_IMAGEMESS:
      s = 0;
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      s = 0;
      return p;
    case FINGERPRINT_FEATUREFAIL:
      s = 0;
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      s = 0;
      return p;
    default:
      Serial.println(F("Error Occured!!"));
      s = 0;
      return p;
  }
  Serial.println(F("Remove finger"));
  p = 0;
  while (p != FINGERPRINT_NOFINGER) {
    p = finger.getImage();
  }
  p = -1;
  Serial.println(F("Place same finger again"));
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
      case FINGERPRINT_OK:
        Serial.println(F("Image taken"));
        break;
      case FINGERPRINT_NOFINGER:
        Serial.print(F("."));
        break;
      case FINGERPRINT_PACKETRECIEVEERR:
        s = 0;
        break;
      case FINGERPRINT_IMAGEFAIL:
        s = 0;
        break;
      default:
        Serial.println(F("Error Occured!!"));
        s = 0;
        break;
    }
  }

  // OK success!

  p = finger.image2Tz(2);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println(F("Image converted"));
      break;
    case FINGERPRINT_IMAGEMESS:
      s = 0;
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      s = 0;
      return p;
    case FINGERPRINT_FEATUREFAIL:
      s = 0;
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      s = 0;
      return p;
    default:
      Serial.println(F("Error Occured!!"));
      s = 0;
      return p;
  }

  // OK converted!
  Serial.print(F("Creating model for #"));
  Serial.println(id);

  p = finger.createModel();
  if (p == FINGERPRINT_OK) {
    Serial.println(F("Prints matched!"));
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    s = 0;
    return p;
  } else if (p == FINGERPRINT_ENROLLMISMATCH) {
    s = 0;
    return p;
  } else {
    Serial.println(F("Error Occured!!"));
    s = 0;
    return p;
  }

  p = finger.storeModel(id);
  if (p == FINGERPRINT_OK) {
    Serial.println(F("Stored!"));
    s = 1;
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    s = 0;
    return p;
  } else if (p == FINGERPRINT_BADLOCATION) {
    s = 0;
    return p;
  } else if (p == FINGERPRINT_FLASHERR) {
    s = 0;
    return p;
  } else {
    Serial.println(F("Error Occured!!"));
    s = 0;
    return p;
  }

  return true;
}
/**
*@brief
-	Quét vân tay và so sánh dữ liệu
*@param
*@retval
-	Trả về giá trị số nguyên của ID nếu dấu vân tay trùng
*/
uint8_t scanFinger(uint8_t &finger_id) {
  uint8_t p = finger.getImage();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println(F("Image taken"));
      break;
    case FINGERPRINT_NOFINGER:
      Serial.println(F("No finger detected"));
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      return p;
    case FINGERPRINT_IMAGEFAIL:
      return p;
    default:
      Serial.println(F("Error Occured!!"));
      return p;
  }

  // OK success!

  p = finger.image2Tz();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println(F("Image converted"));
      break;
    case FINGERPRINT_IMAGEMESS:
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      return p;
    case FINGERPRINT_FEATUREFAIL:
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      return p;
    default:
      Serial.println(F("Error Occured!!"));
      return p;
  }

  // OK converted!
  p = finger.fingerSearch();
  if (p == FINGERPRINT_OK) {
    Serial.println(F("Found a print match!"));
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    return p;
  } else if (p == FINGERPRINT_NOTFOUND) {
    return p;
  } else {
    Serial.println(F("Error Occured!!"));
    return p;
  }

  // found a match!
  finger_id = finger.fingerID;
  return finger.fingerID;
}
/**
*@brief
-	Xóa dấu vân tay.
*@param
-	int id: ID vân tay cần xóa.
*@retval
*/
uint8_t deleteFingerID(uint8_t &id) {
  uint8_t p = -1;
  Serial.print(F("Waiting for valid finger to delete as #"));
  Serial.println(id);
  p = finger.deleteModel(id);
  if (p == FINGERPRINT_OK) {
    Serial.println(F("Deleted!"));
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
  } else if (p == FINGERPRINT_BADLOCATION) {
  } else if (p == FINGERPRINT_FLASHERR) {
  } else {
    Serial.print(F("Error Occured: 0x"));
    Serial.println(p, HEX);
  }

  return p;
}