/*
 * DS3231RTC.h - library for DS3231 RTC
 * This library is intended to be uses with Arduino Time.h library functions
 */

#ifndef DS3231RTC_h
#define DS3231RTC_h

#include <Time.h>

// library interface description
class DS3231RTC
{
  // user-accessible "public" interface
  public:
    DS3231RTC();
    static time_t get();
    static time_t getAlarmOne();
    static time_t getAlarmTwo();
    static void set(time_t t);
    static void set(int hr,int min,int sec,int dy, int mnth, int yr);
    static void setAlarmOne(int hr, int min, int sec, int alarmState);
    static void setAlarmTwo(int hr, int min, int alarmState);
    static void resetA1();
    static void resetA2();
    static void read(tmElements_t &tm);
    static void readAlarmOne(tmElements_t &tm);
    static void readAlarmTwo(tmElements_t &tm);
    static int readA1state();
    static int readA2state();
	static int checkA1();
	static int checkA2();
    static void write(tmElements_t &tm);
    static void writeAlarmOne(tmElements_t &tm);
	static void writeA1state(int alarmState);
    static void writeAlarmTwo(tmElements_t &tm);
	static void writeA2state(int alarmState);

  private:
    static uint8_t dec2bcd(uint8_t num);
    static uint8_t bcd2dec(uint8_t num);
};

extern DS3231RTC RTC;

#endif
 

