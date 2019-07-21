// Custom Adaptation of uRTC for DS3231M
#include <TinyWireM.h>
#include "uTime.h"
#define _rtc_address 0x68

#include "ssd1306.h"


#define uRTCLIB_decToBcd(val) ((uint8_t) ((val / 10 * 16) + (val % 10)))
#define uRTCLIB_bcdToDec(val) ((uint8_t) ((val / 16 * 10) + (val % 16)))

float _temp = 9999;

//communications and real functions
void refreshRTCI2c() {
  TinyWireM.beginTransmission(_rtc_address);
  TinyWireM.write(0); // set DS3231 register pointer to 00h
  TinyWireM.endTransmission();

  TinyWireM.requestFrom(_rtc_address, 15);
  // RTC rad data
  uint8_t _second = 0;
  uint8_t _minute = 0;
  uint8_t _hour = 0;
  uint8_t _day = 0;
  uint8_t _month = 0;
  uint16_t _year = 0;
  uint8_t _dayOfWeek = 0;
  _second = TinyWireM.read();
  _second = uRTCLIB_bcdToDec(_second);
  _minute = TinyWireM.read();
  _minute = uRTCLIB_bcdToDec(_minute);
  _hour = TinyWireM.read() & 0b111111;
  _hour = uRTCLIB_bcdToDec(_hour);
  _dayOfWeek = TinyWireM.read();
  _dayOfWeek = uRTCLIB_bcdToDec(_dayOfWeek);
  _day = TinyWireM.read();
  _day = uRTCLIB_bcdToDec(_day);
  _month = TinyWireM.read();
  _month = uRTCLIB_bcdToDec(_month);
  _year = TinyWireM.read();
  _year = uRTCLIB_bcdToDec(_year);

  //upload data to internal RAM
  setTime(_hour, _minute, _second, _day, _month, _year, _dayOfWeek);

  _temp = 9999; // Some obvious error value
  // Temperature registers (11h-12h) get updated automatically every 64s
  TinyWireM.beginTransmission(_rtc_address);
  TinyWireM.write(0x11);
  TinyWireM.endTransmission();
  TinyWireM.requestFrom(_rtc_address, 2);
  byte MSB, LSB;
  MSB = TinyWireM.read(); //2's complement int portion
  LSB = TinyWireM.read(); //fraction portion
  _temp = ((((short)MSB << 8) | (short)LSB) >> 6) / 4.0;
}

void setRTCI2c(const uint8_t second, const uint8_t minute, const uint8_t hour, const uint8_t dayOfMonth, const uint8_t month, const uint8_t year, const uint8_t dayOfWeek) {
  //set SQWG off
  uint8_t status, processAnd = 0b11111111, processOr = 0b00000100;
  TinyWireM.beginTransmission(_rtc_address);
  TinyWireM.write(0x0E);
  TinyWireM.endTransmission();
  TinyWireM.requestFrom(_rtc_address, 1);
  status = TinyWireM.read();
  status = status & processAnd | processOr;
  TinyWireM.beginTransmission(_rtc_address);
  TinyWireM.write(0x0E);
  TinyWireM.write(status);
  TinyWireM.endTransmission();

  //set time
  TinyWireM.beginTransmission(_rtc_address);
  TinyWireM.write(0); // set next input to start at the seconds register
  TinyWireM.write(uRTCLIB_decToBcd(second)); // set seconds
  TinyWireM.write(uRTCLIB_decToBcd(minute)); // set minutes
  TinyWireM.write(uRTCLIB_decToBcd(hour)); // set hours
  TinyWireM.write(uRTCLIB_decToBcd(dayOfWeek)); // set day of week (1=Sunday, 7=Saturday)
  TinyWireM.write(uRTCLIB_decToBcd(dayOfMonth)); // set date (1 to 31)
  TinyWireM.write(uRTCLIB_decToBcd(month)); // set month
  TinyWireM.write(uRTCLIB_decToBcd(year)); // set year (0 to 99)
  TinyWireM.endTransmission();

  refreshRTCI2c();
}

float tempRTCI2c() {
  return _temp;
}
