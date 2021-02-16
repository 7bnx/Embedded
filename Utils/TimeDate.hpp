//----------------------------------------------------------------------------------
//  Author:       Semyon Ivanov
//  e-mail:       agreement90@mail.ru
//  github:       https://github.com/7bnx/Embedded
//  Description:  TimeDate
//  TODO:
//----------------------------------------------------------------------------------

#ifndef _TIMEDATE_H
#define _TIMEDATE_H

#include "Time.hpp"
#include "Date.hpp"

/*!
  @brief Namespace for utils
*/
namespace utils{

/*!
  @brief Aggregate Time and Date
*/
class TimeDate {

public:

  /*!
    @brief Default-constructor: year = 0, month = 0, date = 0, day = 0, isLeap = false
                                hrs = 0, min = 0, sec = 0, msec = 0
  */
  TimeDate() {}

  /*!
    @brief Copy parameters from date. Time parameters: hrs = 0, min = 0, sec = 0, msec = 0
  */
  TimeDate(const Date& date) : date(date) {}

  /*!
    @brief Copy parameters from date and time
  */
  TimeDate(const Date& date, const Time& time) : date(date), time(time) {}

  /*!
    @brief Set date parameters: year, month, date. Time parameters: hrs = 0, min = 0, sec = 0, msec = 0
  */
  TimeDate(uint16_t year, uint8_t month, uint8_t date) : date(year, month, date) {}

  /*!
    @brief Set date parameters: year, month, date and copy time
  */
  TimeDate(uint16_t year, uint8_t month, uint8_t date, const Time& time) : date(year, month, date), time(time) {}

  /*!
    @brief Copy date parameters and set time: hrs, min, sec, msec
  */
  TimeDate(const Date& date, uint8_t hrs, uint8_t min = 0, uint8_t sec = 0, uint16_t msec = 0) :
    date(date), time(hrs, min, sec, msec) {};

  /*!
    @brief Set date parameters: year, month, date and time: hrs, min, sec, msec
  */
  TimeDate(uint16_t year, uint8_t month, uint8_t date, 
           uint8_t hrs = 0, uint8_t min = 0, uint8_t sec = 0, uint16_t msec = 0) :
    date(year, month, date), time(hrs, min, sec, msec) {};

  /*!
    @brief Manually input all parameters: year, month, date, day, leap and time: hrs, min, sec, msec
  */
  TimeDate(uint16_t year, uint8_t month, uint8_t date, uint8_t day, bool isLeap, 
           uint8_t hrs = 0, uint8_t min = 0, uint8_t sec = 0, uint16_t msec = 0) :
    date(year, month, date, day, isLeap), time(hrs, min, sec, msec) {};

  __FORCE_INLINE const Time& GetTime() const { return time; }
  __FORCE_INLINE const Date& GetDate() const { return date; }

  /*!
    @brief Set date parameters
  */
  __FORCE_INLINE void Set(const Date& date) { this->date = date; };

  /*!
    @brief Set time parameters
  */
  __FORCE_INLINE void Set(const Time& time) { this->time = time; };

  /*!
    @brief Set date and time parameters
  */
  __FORCE_INLINE void Set(const Date& date, const Time& time){ this->date = date; this->time = time; }

  /*!
    @brief Set date parameters: year, month, date
  */
  __FORCE_INLINE void Set(uint16_t year, uint8_t month, uint8_t date){ this->date.Set(year, month, date); }

  /*!
    @brief Set date parameters: year, month, date and time
  */
  __FORCE_INLINE void Set(uint16_t year, uint8_t month, uint8_t date, const Time& time){
    this->date.Set(year, month, date); 
    this->time = time; 
  }
  
  /*!
    @brief Set date and time parameters: hrs, min, sec, msec
  */
  __FORCE_INLINE void Set(const Date& date, uint8_t hrs, uint8_t min = 0, uint8_t sec = 0, uint16_t msec = 0){
    this->date = date; 
    this->time.Set(hrs, min, sec, msec );
  }

  /*!
    @brief Set date parameters: year, month, date and time parameters: hrs, min, sec, msec
  */
  __FORCE_INLINE void Set(uint16_t year, uint8_t month, uint8_t date, 
                          uint8_t hrs, uint8_t min = 0, uint8_t sec = 0, uint16_t msec = 0){
    this->date.Set(year, month, date); 
    this->time.Set(hrs, min, sec, msec); 
  }

  /*!
    @brief Set millisecond
  */
  __FORCE_INLINE void SetMsec(uint16_t value){ time.SetMsec(value); }

  /*!
    @brief Set second
  */
  __FORCE_INLINE void SetSec(uint8_t value){ time.SetSec(value); }

  /*!
    @brief Set minute
  */
  __FORCE_INLINE void SetMin(uint8_t value){ time.SetMin(value); }

  /*!
    @brief Set hour
  */
  __FORCE_INLINE void SetHrs(uint8_t value){ time.SetHrs(value); }

  /*!
    @brief Set  date
  */
  __FORCE_INLINE bool SetDate(uint8_t value){ return date.SetDate(value); }

  /*!
    @brief Set  date
  */
  __FORCE_INLINE bool SetDayOfWeek(uint8_t value){ return date.SetDayOfWeek(value); }

  /*!
    @brief Set month
  */
  __FORCE_INLINE bool SetMonth(uint8_t value){ return date.SetMonth(value); }

  /*!
    @brief Set year
  */
  __FORCE_INLINE bool SetYear(uint16_t value){ return date.SetYear(value); }

  /*!
    @brief Add timedate parameters to current
  */
  void Add(const TimeDate& timedate){ 
    size_t days = time.Add(timedate.time);
    date.Add(timedate.date.GetYear(), timedate.date.GetMonth(), timedate.date.GetDate() + days); 
  }

  /*!
    @brief Add date parameters to current
  */
  __FORCE_INLINE void Add(const Date& date){ this->date.Add(date); }

  /*!
    @brief Add time parameters to current
    @return Number of added days
  */
  __FORCE_INLINE size_t Add(const Time& time){ 
    size_t days = this->time.Add(time); 
    date.AddDay(days); 
    return days;
  }

  /*!
    @brief Add time parameters: hrs, min, sec, msec
    @return Number of added days
  */
  __FORCE_INLINE size_t Add(size_t hrs, size_t min, size_t sec, size_t msec){ return Add(Time(hrs, min, sec, msec)); }

  /*!
    @brief Add date parameters: year, month, date
  */
  __FORCE_INLINE void Add(size_t year, size_t month, size_t date){ this->date.Add(year, month, date); }

  /*!
    @brief Add milliseconds
    @return Number of added days
  */
  __FORCE_INLINE size_t AddMsec(size_t value = 1){ return AddDay(time.AddMsec(value)); }

  /*!
    @brief Add seconds
    @return Number of added days
  */
  __FORCE_INLINE size_t AddSec(size_t value = 1){ return AddDay(time.AddSec(value)); }
  
  /*!
    @brief Add minutes
    @return Number of added days
  */
  __FORCE_INLINE size_t AddMin(size_t value = 1){ return AddDay(time.AddMin(value)); }
  
  /*!
    @brief Add hours
    @return Number of added days
  */
  __FORCE_INLINE size_t AddHrs(size_t value = 1){ return AddDay(time.AddHrs(value)); }
  
  /*!
    @brief Add days
    @return Number of added days
  */
  __FORCE_INLINE size_t AddDay(size_t value = 1){ date.AddDay(value); return value; }
  
  /*!
    @brief Add weeks
  */
  __FORCE_INLINE void AddWeek(size_t value = 1){ date.AddWeek(value); }
  
  /*!
    @brief Add months
  */
  __FORCE_INLINE void AddMonth(size_t value = 1){ date.AddMonth(value); }
  
  /*!
    @brief Add years
  */
  __FORCE_INLINE void AddYear(size_t value = 1){ date.AddYear(value); }

  /*!
    @brief Add timedate parameters to current
  */
  void Sub(const TimeDate& timedate){ 
    size_t days = time.Sub(timedate.time);
    date.Sub(timedate.date.GetYear(), timedate.date.GetMonth(), timedate.date.GetDate() + days); 
  }

  /*!
    @brief Substract date parameters from current
  */
  __FORCE_INLINE void Sub(const Date& date){ this->date.Sub(date); }

  /*!
    @brief Substract time parameters from current
  */
  __FORCE_INLINE void Sub(const Time& time){ date.SubDay(this->time.Sub(time)); }

  /*!
    @brief Substract time parameters: hrs, min, sec, msec
  */
  __FORCE_INLINE void Sub(size_t hrs, size_t min, size_t sec, size_t msec){ date.SubDay(time.Sub(hrs, min, sec, msec)); }

  /*!
    @brief Substract date parameters: year, month, date
  */
  __FORCE_INLINE void Sub(size_t year, size_t month, size_t date){ this->date.Sub(year, month, date); }

  /*!
    @brief Substract milliseconds
  */
  __FORCE_INLINE void SubMsec(size_t value = 1){ date.SubDay(time.SubMsec(value)); }

  /*!
    @brief Substract seconds
  */
  __FORCE_INLINE void SubSec(size_t value = 1){ date.SubDay(time.SubSec(value)); }

  /*!
    @brief Substract minutes
  */
  __FORCE_INLINE void SubMin(size_t value = 1){ date.SubDay(time.SubMin(value)); }
  
  /*!
    @brief Substract hours
  */
  __FORCE_INLINE void SubHrs(size_t value = 1){ date.SubDay(time.SubHrs(value)); }
  
  /*!
    @brief Substract days
  */
  __FORCE_INLINE void SubDay(size_t value = 1){ date.SubDay(value); }
  
  /*!
    @brief Substract weeks
  */
  __FORCE_INLINE void SubWeek(size_t value = 1){ date.SubWeek(value); }

  /*!
    @brief Substract months
  */
  __FORCE_INLINE void SubMonth(size_t value = 1){ date.SubMonth(value); }

  /*!
    @brief Substract years
  */
  __FORCE_INLINE void SubYear(size_t value = 1){ date.SubYear(value); }

  /*!
    @brief Get difference between timedate and current
  */
  TimeDate Difference(const TimeDate& timedate) const;

  /*!
    @brief Get difference between timedate and current in milliseconds
  */
  size_t DifferenceInMsec(const TimeDate& timedate) const {
    bool hasSubtractionalDay;
    size_t msec = _DifferenceInDayHelper(timedate, hasSubtractionalDay) * 86400000;
    return msec + (hasSubtractionalDay ? time.DifferenceInvertInMsec(timedate.time) : time.DifferenceInMsec(timedate.time));
  }

  /*!
    @brief Get difference between timedate and current in seconds
  */
  size_t DifferenceInSec(const TimeDate& timedate) const { 
    bool hasSubtractionalDay;
    size_t sec = _DifferenceInDayHelper(timedate, hasSubtractionalDay) * 86400;
    return sec + (hasSubtractionalDay ? time.DifferenceInvertInSec(timedate.time) : time.DifferenceInSec(timedate.time));
  }

  /*!
    @brief Get difference between timedate and current in minutes
  */
  size_t DifferenceInMin(const TimeDate& timedate) const {
    bool hasSubtractionalDay;
    size_t min = _DifferenceInDayHelper(timedate, hasSubtractionalDay) * 1440;
    return min + (hasSubtractionalDay ? time.DifferenceInvertInMin(timedate.time) : time.DifferenceInMin(timedate.time));
  }

  /*!
    @brief Get difference between timedate and current in hours
  */
  size_t DifferenceInHrs(const TimeDate& timedate) const {
    bool hasSubtractionalDay;
    size_t hrs = _DifferenceInDayHelper(timedate, hasSubtractionalDay) * 24;
    return hrs + (hasSubtractionalDay ? time.DifferenceInvertInHrs(timedate.time) : time.DifferenceInHrs(timedate.time));
 }

  /*!
    @brief Get difference between timedate and current in days
  */
  size_t DifferenceInDay(const TimeDate& timedate) const { 
    bool dummy;
    return _DifferenceInDayHelper(timedate, dummy);
  }

  /*!
    @brief Get difference between timedate and current in weeks
  */
  __FORCE_INLINE size_t DifferenceInWeek(const TimeDate& timedate) const { return DifferenceInDay(timedate) / 7; }

  /*!
    @brief Get difference between timedate and current in months
  */
  __FORCE_INLINE size_t DifferenceInMonth(const TimeDate& timedate) const { return date.DifferenceInMonth(timedate.date); }

  /*!
    @brief Get difference between timedate and current in years
  */
  __FORCE_INLINE size_t DifferenceInYear(const TimeDate& timedate) const { return date.DifferenceInYear(timedate.date); }

  template<typename T>
  TimeDate& operator+=(const T& rhs){ Add(rhs); return *this; }
  template<typename T>
  TimeDate& operator-=(const T& rhs){ Sub(rhs); return *this; }

  bool operator<(const TimeDate& rhs) const { 
    return (date < rhs.date || (time < rhs.time && date == rhs.date));
  }
  bool operator>(const TimeDate& rhs) const { return rhs < *this; }
  bool operator<=(const TimeDate& rhs) const { return !(*this > rhs); }
  bool operator>=(const TimeDate& rhs) const { return !(*this < rhs); }

  bool operator==(const TimeDate& rhs) const{ return (date == rhs.date && time == rhs.time); }
  bool operator!=(const TimeDate& rhs) const { return !(*this == rhs);}

  TimeDate operator+(const TimeDate& rhs){ return *this += rhs; }
  TimeDate operator-(const TimeDate& rhs){ return *this -= rhs; }

private:

  Date date;
  Time time;

  size_t _DifferenceInDayHelper(const TimeDate& timedate, bool& hasSubtractionalDay) const {
    hasSubtractionalDay = false;
    if (this->date == timedate.date) return 0;
    size_t days1 = this->date.ConvertToDays();
    size_t days2 = timedate.date.ConvertToDays();
    size_t difference;
    if (days1 > days2){
      hasSubtractionalDay = !(this->time >= timedate.time);
      difference = days1 - days2;
    } else{
      hasSubtractionalDay = !(timedate.time >= this->time);
      difference = days2 - days1;
    }
    return difference - hasSubtractionalDay;
  }

};

TimeDate TimeDate::Difference(const TimeDate& timedate) const {
  bool isGreater = *this > timedate;
  bool isSubDay = (isGreater && this->time < timedate.time) || (!isGreater && this->time > timedate.time);
  Date date1{ isGreater ? this->date : timedate.date };
  Date date2{ !isGreater ? this->date : timedate.date };
  Time time = (!isSubDay ? this->time.Difference(timedate.time) : this->time.DifferenceInvert(timedate.time));
  date1.Sub(date2.GetYear(), date2.GetMonth(), date2.GetDate() + size_t(isSubDay));
  return TimeDate{ date1, time };
}

} // namespace utils

#endif // !_TIMEDATE_H