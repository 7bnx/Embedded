//----------------------------------------------------------------------------------
//  Author:       Semyon Ivanov
//  e-mail:       agreement90@mail.ru
//  github:       https://github.com/7bnx/Embedded
//  Description:  Time
//  TODO:
//----------------------------------------------------------------------------------

#ifndef _TIME_H
#define _TIME_H

#include <cstddef>
#include <cstdint>
#include "../Controllers/Common/Compiler/Compiler.h"

/*!
  @brief Namespace for utils
*/
namespace utils{

/*!
  @brief Time: hrs, min, sec, msec
*/
class Time{

public:

  /*!
    @brief Default-constructor. hrs = 0, min = 0, sec = 0, msec = 0 
  */
  Time(){}

  /*!
    @brief Constructor
    @param [in] hrs set hours
  */
  Time(uint8_t hrs) : hrs(hrs) {}

  /*!
    @brief Constructor
    @param [in] hrs set hours
    @param [in] min set minutes
  */
  Time(uint8_t hrs, uint8_t min) : hrs(hrs), min(min) {}

  /*!
    @brief Constructor
    @param [in] hrs set hours
    @param [in] min set minutes
    @param [in] sec set seconds
  */
  Time(uint8_t hrs, uint8_t min, uint8_t sec) : hrs(hrs), min(min), sec(sec){}

  /*!
    @brief Constructor
    @param [in] hrs set hours
    @param [in] min set minutes
    @param [in] sec set seconds
    @param [in] msec set milliseconds
  */
  Time(uint8_t hrs, uint8_t min, uint8_t sec, uint16_t msec) : hrs(hrs), min(min), sec(sec), msec(msec){}

  /*!
    @brief Get current hour
  */
  __FORCE_INLINE uint8_t GetHrs() const { return hrs; }

  /*!
    @brief Get current minute
  */
  __FORCE_INLINE uint8_t GetMin() const { return min; }

  /*!
    @brief Get current second
  */
  __FORCE_INLINE uint8_t GetSec() const { return sec; }

  /*!
    @brief Get current millisecond
  */
  __FORCE_INLINE uint16_t GetMsec() const { return msec; }


  /*!
    @brief Set new time
  */
  void Set(uint8_t hrs, uint8_t min, uint8_t sec, uint16_t msec){ *this = Time(hrs, min, sec, msec); }
  
  /*!
    @brief Set new time
    @param [in] time source
  */
  __FORCE_INLINE void Set(const Time& time){ Set(time.hrs, time.min, time.sec, time.msec); }

  /*!
    @brief Set new hours
  */
  __FORCE_INLINE void SetHrs(uint8_t value){ Set(value, min, sec, msec); }

  /*!
    @brief Set new minutes
  */
  __FORCE_INLINE void SetMin(uint8_t value){ Set(hrs, value, sec, msec); }

  /*!
    @brief Set new seconds
  */
  __FORCE_INLINE void SetSec(uint8_t value){ Set(hrs, min, value, msec); }

  /*!
    @brief Set new milliseconds
  */
  __FORCE_INLINE void SetMsec(uint16_t value){ Set(hrs, min, sec, value); }

  /*!
    @brief Add values to current time
    @return Additional days
  */
  __FORCE_INLINE size_t Add(size_t hrs, size_t min, size_t sec, size_t msec){ 
    return AddMsec(msec + 1000*(sec + 60*(min + 60*hrs))); 
  }
  
  /*!
    @brief Add time
    @param [in] time to add
    @return Additional days
  */
  __FORCE_INLINE size_t Add(const Time & time){ return Add(time.hrs, time.min, time.sec, time.msec); }
  
  /*!
    @brief Add hours
    @return Additional days
  */
  __FORCE_INLINE size_t AddHrs(size_t value = 1){ return AddMin(value*60); }
  
  /*!
    @brief Add minutes
    @return Additional days
  */
  __FORCE_INLINE size_t AddMin(size_t value = 1){ return AddSec(value*60); }
  
  /*!
    @brief Add seconds
    @return Additional days
  */
  __FORCE_INLINE size_t AddSec(size_t value = 1){ return AddMsec(value*1000); }
  
  /*!
    @brief Subtract milliseconds
    @return Additional days
  */
  size_t AddMsec(size_t value = 1){
    if (!value) return 0;
    this->value += value;
    return _ConvertToTime();
  }
  
  /*!
    @brief Subtract time
    @return Subtractional days
  */
  __FORCE_INLINE size_t Sub(size_t hrs, size_t min = 0, size_t sec = 0, size_t msec = 0){ 
    return SubMsec(((hrs*60 + min)*60 + sec)*1000 + msec);
  }
  
  /*!
    @brief Subtract time
    @param [in] time to substract
    @return Subtractional days
  */
  __FORCE_INLINE size_t Sub(const Time & time){ return Sub(time.hrs, time.min, time. sec, time. msec); }
  
  /*!
    @brief Subtract hours
    @return Subtractional days
  */
  __FORCE_INLINE size_t SubHrs(size_t value = 1){ return SubMin(value*60); }
  
  /*!
    @brief Subtract minutes
    @return Subtractional days
  */
  __FORCE_INLINE size_t SubMin(size_t value = 1){ return SubSec(value*60); }

  /*!
    @brief Subtract seconds
    @return Subtractional days
  */
  __FORCE_INLINE size_t SubSec(size_t value = 1){ return SubMsec(value*1000); }

  /*!
    @brief Subtract milliseconds
    @return Subtractional days
  */
  size_t SubMsec(size_t value = 1) {
    if (!value) return 0;

    if (this->value >= value) this->value -= value;
    else
      this->value = this->value + (value / msecPerDay + 1) * msecPerDay - value;

    return _ConvertToTime();
  }

  /*!
    @brief Calculate difference between times. t1 = 01/00/00, t2 = 02/00/00 -> result = 01/00/00;
    @param [in] time to compare
  */
  Time Difference(const Time& time) const {
    Time difference;
    difference.AddMsec(DifferenceInMsec(time));
    return difference;
  }
  
  /*!
    @brief Calculate difference between times. t1 = 01/00/00, t2 = 02/00/00 -> result = 23/00/00;
    @param [in] time to compare
  */
  Time DifferenceInvert(const Time& time) const {
    Time difference;
    difference.AddMsec(msecPerDay - DifferenceInMsec(time));
    return difference;
  }

  /*!
    @brief Calculate difference between times in hours
    @param [in] time to compare
  */
  __FORCE_INLINE size_t DifferenceInHrs(const Time & time) const { return DifferenceInMin(time) / 60; }

  /*!
    @brief Calculate invert difference between times in hours
    @param [in] time to compare
  */
  __FORCE_INLINE size_t DifferenceInvertInHrs(const Time & time) const { return DifferenceInvertInMin(time) / 60; }

  /*!
    @brief Calculate difference between times in minutes
    @param [in] time to compare
  */
  __FORCE_INLINE size_t DifferenceInMin(const Time & time) const { return DifferenceInSec(time) / 60; }
  
  /*!
    @brief Calculate invert difference between times in minutes
    @param [in] time to compare
  */
  __FORCE_INLINE size_t DifferenceInvertInMin(const Time & time) const { return DifferenceInvertInSec(time) / 60; }

  /*!
    @brief Calculate difference between times in seconds
    @param [in] time to compare
  */
  __FORCE_INLINE size_t DifferenceInSec(const Time& time) const { return DifferenceInMsec(time) / 1000; }

  /*!
    @brief Calculate invert difference between times in seconds
    @param [in] time to compare
  */
  __FORCE_INLINE size_t DifferenceInvertInSec(const Time& time) const { return DifferenceInvertInMsec(time) / 1000; }

  /*!
    @brief Calculate difference between times in milliseconds
    @param [in] time to compare
  */
  __FORCE_INLINE size_t DifferenceInMsec(const Time& time) const {
    return *this > time ? this->value - time.value : time.value - this->value; 
  }

  /*!
    @brief Calculate invert difference between times in milliseconds
    @param [in] time to compare
  */
  __FORCE_INLINE size_t DifferenceInvertInMsec(const Time& time) const {
    return msecPerDay - DifferenceInMsec(time);
  }

  /*!
    @brief Get number of minutes, passed from 00:00:00:000
  */
  __FORCE_INLINE size_t GetHrsTillDayStart() const { return hrs; }

  /*!
    @brief Get number of minutes, passed from 00:00:00:000
  */
  __FORCE_INLINE size_t GetMinTillDayStart() const { return min + hrs*60; }

  /*!
    @brief Get number of seconds, passed from 00:00:00:000
  */
  __FORCE_INLINE size_t GetSecTillDayStart() const { return sec + GetMinTillDayStart()*60; }

  /*!
    @brief Get number of milliseconds, passed from 00:00:00:000
  */
  __FORCE_INLINE size_t GetMsecTillDayStart() const { return value; }

  /*!
    @brief Get number of hours, before day's end
  */
  __FORCE_INLINE size_t GetHrsTillDayEnd() const { return GetMsecTillDayEnd() / msecPerHour; }

  /*!
    @brief Get number of minutes, before day's end
  */
  __FORCE_INLINE size_t GetMinTillDayEnd() const { return GetMsecTillDayEnd() / msecPerMinute; }

  /*!
    @brief Get number of seconds, before day's end
  */
  __FORCE_INLINE size_t GetSecTillDayEnd() const { return GetMsecTillDayEnd() / msecPerSecond; }

  /*!
    @brief Get number of milliseconds, before day's end
  */
  __FORCE_INLINE size_t GetMsecTillDayEnd() const { return msecPerDay - value; }

  Time& operator+=(const Time& rhs){ Add(rhs); return *this; }
  Time& operator-=(const Time& rhs){ Sub(rhs); return *this; }

  __FORCE_INLINE bool operator<(const Time& rhs) const { return value < rhs.value; }
  __FORCE_INLINE bool operator>(const Time& rhs) const { return  rhs < *this; }
  __FORCE_INLINE bool operator<=(const Time& rhs) const { return *this > rhs; }
  __FORCE_INLINE bool operator>=(const Time& rhs) const { return !(*this < rhs); }

  __FORCE_INLINE bool operator==(const Time& rhs) const { return value == rhs.value; }
  __FORCE_INLINE bool operator!=(const Time& rhs) const { return !(*this == rhs);}
  
  __FORCE_INLINE operator bool() const { return value; }

  Time operator+(const Time& rhs){ return *this += rhs; }
  Time operator-(const Time& rhs){ return *this -= rhs; }

private:

  uint8_t hrs = 0;
  uint8_t min = 0;
  uint8_t sec = 0;
  uint16_t msec = 0;
  size_t value = ((hrs*60UL + min)*60 + sec)*1000 + msec;

  static constexpr uint16_t msecPerSecond = 1000;
  static constexpr uint16_t msecPerMinute = 60 * msecPerSecond;
  static constexpr size_t msecPerHour = 60 * msecPerMinute;
  static constexpr size_t msecPerDay = 24 * msecPerHour;

  __FORCE_INLINE size_t _ConvertToTime(){
    size_t tmp = value;
    msec = tmp % 1000;
    tmp /= 1000;
    sec = tmp % 60;
    tmp /= 60;
    min = tmp % 60;
    tmp /= 60;
    hrs = tmp % 24;
    value %= msecPerDay;
    return tmp / 24;
  }

};

} // namespace utils

#endif // !_TIME_H