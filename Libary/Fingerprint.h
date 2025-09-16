#ifndef FINGERPRINT_H
#define FINGERPRINT_H
#include <SoftwareSerial.h>
#include <Adafruit_Fingerprint.h>
uint8_t enrollFingerID(uint8_t &id, byte &s); //Thêm vân tay
uint8_t scanFinger(uint8_t &finger_id); //Quét vân tay
uint8_t deleteFingerID(uint8_t &id); //Xoá vân tay
void fingerprintSetup();
#endif