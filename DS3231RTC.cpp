/*
 * DS3231RTC.h - library for DS3231 RTC
  
  Based on the DS1307RTC library which is copyright (c) Michael Margolis 2009
  Alterations copyright (c) Nick Bulleid 2013

  This library is intended to be uses with Arduino Time.h library functions

  The library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
  
  19 Nov 2013 - Initial release
 */

#include <Wire.h>
#include "DS3231RTC.h"

#define DS3231_CTRL_ID 0x68 

DS3231RTC::DS3231RTC()
{
  Wire.begin();
}
  
// PUBLIC FUNCTIONS
time_t DS3231RTC::get()   // Acquire data from buffer and convert to time_t
{
  tmElements_t tm;
  read(tm);
  return(makeTime(tm));
}

time_t DS3231RTC::getAlarmOne()
{
  tmElements_t tm;
  readAlarmOne(tm);
  return(makeTime(tm));
}

time_t DS3231RTC::getAlarmTwo()
{
  tmElements_t tm;
  readAlarmTwo(tm);
  return(makeTime(tm));
}

void  DS3231RTC::set(time_t t)
{
  tmElements_t tm;
  breakTime(t, tm);
  write(tm); 
}

void DS3231RTC::set(int hr,int min,int sec,int dy, int mnth, int yr){
 // year can be given as full four digit year or two digts (2010 or 10 for 2010);  
 //it is converted to years since 1970
  tmElements_t tm;
  if( yr > 99)
      yr = yr - 1970;
  else
      yr += 30;  
  tm.Year = yr;
  tm.Month = mnth;
  tm.Day = dy;
  tm.Hour = hr;
  tm.Minute = min;
  tm.Second = sec;
  write(tm);
}

void DS3231RTC::setAlarmOne(int hr, int min, int sec, int alarmState){
  tmElements_t tm;
  tm.Hour = hr;
  tm.Minute = min;
  tm.Second = sec;
  writeAlarmOne(tm);
  writeA1state(alarmState);
}

void DS3231RTC::setAlarmTwo(int hr, int min, int alarmState){
  tmElements_t tm;
  tm.Hour = hr;
  tm.Minute = min;
  writeAlarmTwo(tm);
  writeA2state(alarmState);
}

void DS3231RTC::resetA1()
{
  Wire.beginTransmission(DS3231_CTRL_ID);
#if ARDUINO >= 100
  Wire.write(0x0F);
  Wire.write(0x8A);
#else
  Wire.send(0x0F);
  Wire.send(0x8A);
#endif
  Wire.endTransmission();
}

void DS3231RTC::resetA2()
{
  Wire.beginTransmission(DS3231_CTRL_ID);
#if ARDUINO >= 100
  Wire.write(0x0F);
  Wire.write(0x89);
#else
  Wire.send(0x0F);
  Wire.send(0x89);
#endif
  Wire.endTransmission();
}

// Aquire data from the RTC chip in BCD format
void DS3231RTC::read( tmElements_t &tm)
{
  Wire.beginTransmission(DS3231_CTRL_ID);
#if ARDUINO >= 100  
  Wire.write((uint8_t)0x00); 
#else
  Wire.send(0x00);
#endif  
  Wire.endTransmission();

  // request the 7 data fields   (secs, min, hr, dow, date, mth, yr)
  Wire.requestFrom(DS3231_CTRL_ID, tmNbrFields);
#if ARDUINO >= 100    
  tm.Second = bcd2dec(Wire.read() );
  tm.Minute = bcd2dec(Wire.read() );
  tm.Hour =   bcd2dec(Wire.read() & 0x3f);  // mask assumes 24hr clock
  tm.Wday = bcd2dec(Wire.read() );
  tm.Day = bcd2dec(Wire.read() );
  tm.Month = bcd2dec(Wire.read() );
  tm.Year = y2kYearToTm((bcd2dec(Wire.read())));
#else
  tm.Second = bcd2dec(Wire.receive() );   
  tm.Minute = bcd2dec(Wire.receive() );
  tm.Hour =   bcd2dec(Wire.receive() & 0x3f);  // mask assumes 24hr clock
  tm.Wday = bcd2dec(Wire.receive() );
  tm.Day = bcd2dec(Wire.receive() );
  tm.Month = bcd2dec(Wire.receive() );
  tm.Year = y2kYearToTm((bcd2dec(Wire.receive())));
#endif  
}

void DS3231RTC::readAlarmOne(tmElements_t &tm)
{
  Wire.beginTransmission(DS3231_CTRL_ID);
#if ARDUINO >= 100  
  Wire.write((uint8_t)0x07); 
#else
  Wire.send(0x07);
#endif  
  Wire.endTransmission();

  // request 4 data fields
  Wire.requestFrom(DS3231_CTRL_ID, 4);
#if ARDUINO >= 100
  tm.Second = bcd2dec(Wire.read() );
  tm.Minute = bcd2dec(Wire.read() );
  tm.Hour =   bcd2dec(Wire.read() & 0x3f);  // mask assumes 24hr clock
  tm.Day = bcd2dec(Wire.read() );
#else
  tm.Second = bcd2dec(Wire.receive() );
  tm.Minute = bcd2dec(Wire.receive() );
  tm.Hour =   bcd2dec(Wire.receive() & 0x3f);  // mask assumes 24hr clock
  tm.Day = bcd2dec(Wire.receive() );
#endif  
}

void DS3231RTC::readAlarmTwo(tmElements_t &tm)
{
  Wire.beginTransmission(DS3231_CTRL_ID);
#if ARDUINO >= 100  
  Wire.write((uint8_t)0x0B); 
#else
  Wire.send(0x0B);
#endif  
  Wire.endTransmission();

  // request 3 data fields
  Wire.requestFrom(DS3231_CTRL_ID, 3);
#if ARDUINO >= 100
  tm.Minute = bcd2dec(Wire.read() );
  tm.Hour =   bcd2dec(Wire.read() & 0x3f);  // mask assumes 24hr clock
  tm.Day = bcd2dec(Wire.read() );
#else
  tm.Minute = bcd2dec(Wire.receive() );
  tm.Hour =   bcd2dec(Wire.receive() & 0x3f);  // mask assumes 24hr clock
  tm.Day = bcd2dec(Wire.receive() );
#endif  
}

int DS3231RTC::readA1state()
{
  Wire.beginTransmission(DS3231_CTRL_ID);
#if ARDUINO >= 100
  Wire.write((uint8_t)0x0E);
#else
  Wire.send(0x0E);
#endif
  Wire.endTransmission();

  // request 1 data field
  Wire.requestFrom(DS3231_CTRL_ID, 1);
#if ARDUINO >= 100
  return((Wire.read() & 0x01));
#else
  return((Wire.receive() & 0x01));
#endif
}


int DS3231RTC::readA2state()
{
  Wire.beginTransmission(DS3231_CTRL_ID);
#if ARDUINO >= 100
  Wire.write((uint8_t)0x0E);
#else
  Wire.send(0x0E);
#endif
  Wire.endTransmission();

  // request 1 data field
  Wire.requestFrom(DS3231_CTRL_ID, 1);
#if ARDUINO >= 100
  return((Wire.read() & 0x02) >> 1);
#else
  return((Wire.receive() & 0x02) >> 1);
#endif
}

int DS3231RTC::checkA1()
{
  Wire.beginTransmission(DS3231_CTRL_ID);
#if ARDUINO >= 100
  Wire.write((uint8_t)0x0F);
#else
  Wire.send(0x0F);
#endif
  Wire.endTransmission();

  // request 1 data field
  Wire.requestFrom(DS3231_CTRL_ID, 1);
#if ARDUINO >= 100
  return((Wire.read() & 0x01));
#else
  return((Wire.receive() & 0x01));
#endif
}

int DS3231RTC::checkA2()
{
  Wire.beginTransmission(DS3231_CTRL_ID);
#if ARDUINO >= 100
  Wire.write((uint8_t)0x0F);
#else
  Wire.send(0x0F);
#endif
  Wire.endTransmission();

  // request 1 data field
  Wire.requestFrom(DS3231_CTRL_ID, 1);
#if ARDUINO >= 100
  return((Wire.read() & 0x02) >> 1);
#else
  return((Wire.receive() & 0x02) >> 1);
#endif
}

void DS3231RTC::write(tmElements_t &tm)
{
  Wire.beginTransmission(DS3231_CTRL_ID);
#if ARDUINO >= 100  
  Wire.write((uint8_t)0x00); // reset register pointer  
  Wire.write(dec2bcd(tm.Second)) ;   
  Wire.write(dec2bcd(tm.Minute));
  Wire.write(dec2bcd(tm.Hour));      // sets 24 hour format
  Wire.write(dec2bcd(tm.Wday));   
  Wire.write(dec2bcd(tm.Day));
  Wire.write(dec2bcd(tm.Month));
  Wire.write(dec2bcd(tmYearToY2k(tm.Year))); 
#else  
  Wire.send(0x00); // reset register pointer  
  Wire.send(dec2bcd(tm.Second)) ;   
  Wire.send(dec2bcd(tm.Minute));
  Wire.send(dec2bcd(tm.Hour));      // sets 24 hour format
  Wire.send(dec2bcd(tm.Wday));   
  Wire.send(dec2bcd(tm.Day));
  Wire.send(dec2bcd(tm.Month));
  Wire.send(dec2bcd(tmYearToY2k(tm.Year)));   
#endif
  Wire.endTransmission();  
}

void DS3231RTC::writeAlarmOne(tmElements_t &tm) // Sets the alarm to go off at the same time every day
{
  Wire.beginTransmission(DS3231_CTRL_ID);
#if ARDUINO >= 100
  Wire.write((uint8_t)0x07); // set to alarm 1 register
  Wire.write(dec2bcd(tm.Second));
  Wire.write(dec2bcd(tm.Minute));
  Wire.write(dec2bcd(tm.Hour));
  Wire.write(0x80);
#else
  Wire.send((uint8_t)0x07); // set to alarm 1 register
  Wire.send(dec2bcd(tm.Second));
  Wire.send(dec2bcd(tm.Minute));
  Wire.send(dec2bcd(tm.Hour));
  Wire.send(0x80);
#endif
  Wire.endTransmission();
}

void DS3231RTC::writeA1state(int alarmState) //TODO: read and adjust control byte
{
  Wire.beginTransmission(DS3231_CTRL_ID);
#if ARDUINO >= 100
  Wire.write((uint8_t)0x0E);
  Wire.write(0x1C|((uint8_t)alarmState));
#else
  Wire.send(0x0E);
  Wire.send(0x1C | (alarmState));
#endif
  Wire.endTransmission();
}


void DS3231RTC::writeAlarmTwo(tmElements_t &tm) // Sets the alarm to go off at the same time every day
{
  Wire.beginTransmission(DS3231_CTRL_ID);
#if ARDUINO >= 100
  Wire.write((uint8_t)0x0B); // set to alarm 1 register
  Wire.write(dec2bcd(tm.Minute));
  Wire.write(dec2bcd(tm.Hour));
  Wire.write((uint8_t)0x80);
#else
  Wire.send(0x0B); // set to alarm 1 register
  Wire.send(dec2bcd(tm.Minute));
  Wire.send(dec2bcd(tm.Hour));
  Wire.send(0x80);
#endif
  Wire.endTransmission();
}

void DS3231RTC::writeA2state(int alarmState) //TODO: read and adjust control byte
{
  Wire.beginTransmission(DS3231_CTRL_ID);
#if ARDUINO >= 100
  Wire.write((uint8_t)0x0E);
  Wire.write(0x1C|((uint8_t)alarmState << 1));
#else
  Wire.send(0x0E);
  Wire.send(0x1C | (alarmState << 1));
#endif
  Wire.endTransmission();
}

// PRIVATE FUNCTIONS

// Convert Decimal to Binary Coded Decimal (BCD)
uint8_t DS3231RTC::dec2bcd(uint8_t num)
{
  return ((num/10 * 16) + (num % 10));
}

// Convert Binary Coded Decimal (BCD) to Decimal
uint8_t DS3231RTC::bcd2dec(uint8_t num)
{
  return ((num/16 * 10) + (num % 16));
}

DS3231RTC RTC = DS3231RTC(); // create an instance for the user

