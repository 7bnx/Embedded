//----------------------------------------------------------------------------------
//  Author:       Semyon Ivanov
//  e-mail:       agreement90@mail.ru
//  github:       https://github.com/7bnx/Embedded
//  Description:  Date
//  TODO:
//----------------------------------------------------------------------------------

#ifndef _DATE_H
#define _DATE_H

#include <cstddef>
#include <cstdint>
#include "../Controllers/Common/Compiler/Compiler.h"

/*!
  @brief Namespace for utils
*/
namespace utils{

/*!
  @brief Date: yaer, month, date, day
*/
class Date{

public:

  /*!
    @brief Default-constructor: year = 0, month = 0, date = 0, day = 0, isLeap = false
  */
  Date(){}

  /*!
    @brief Automatically calculating day of week and the leap year
  */
  Date(uint16_t year, uint8_t month, uint8_t date){ Set(year, month, date); }
  
  /*!
    @brief Manually input for all parameters
  */
  Date(uint16_t year, uint8_t month, uint8_t date, uint8_t day, bool isLeap = false) : 
    year(year), month(month), date(date), day(day), isLeap(isLeap) {};

  /*!
    @brief Get current year
  */
  __FORCE_INLINE size_t GetYear() const { return year; }

  /*!
    @brief Get current month
  */
  __FORCE_INLINE size_t GetMonth() const { return month; }

  /*!
    @brief Get current date
  */
  __FORCE_INLINE size_t GetDate() const { return date; }

  /*!
    @brief Get current day of week
  */
  __FORCE_INLINE size_t GetDay() const { return day; }

  /*!
    @brief Get number of days in current month
  */
  __FORCE_INLINE size_t DaysInMonth() const { return DaysInMonth(isLeap, month); }

  /*!
    @brief Get number of days in current year
  */
  __FORCE_INLINE size_t DaysInYear() const { return isLeap ? 366 : 365; }

  /*!
    @brief Get number of days from start of current year
  */
  size_t DaysFromYearStart() const { return  daysFromStart[month - 1] + ((isLeap && month > 2) ? 1UL : 0) + date; }

  /*!
    @brief Get number of days till start of current year
  */
  __FORCE_INLINE size_t DaysTillYearEnd() const { return  DaysInYear() - DaysFromYearStart(); }

  /*!
    @brief Get number of days till end of current month
  */
  __FORCE_INLINE size_t DaysTillMonthEnd() const { return DaysInMonth() - date; }

  /*!
    @brief Get number of days in current month
    @param [in] year check, if year is leap
    @param [in] month to check
  */
  __FORCE_INLINE static uint8_t DaysInMonth(uint16_t year, uint8_t month) { return DaysInMonth(IsLeap(year), month); }

  /*!
    @brief Get number of days in current month
    @param [in] leap check, if leap
    @param [in] month to check
  */
  static uint8_t DaysInMonth(bool isLeap, uint8_t month) { return daysInMonth[month] + (month == 2 && isLeap ? 1UL : 0); }

  /*!
    @brief Check if the current year is leap
  */
  __FORCE_INLINE bool IsLeap() const { return isLeap; }

  /*!
    @brief Check if the year is leap
  */
  static bool IsLeap(uint16_t year){ return !(year % 4 || (year % 100 == 0 && year % 400)); }

  /*!
    @brief Copy parameters from date
  */
  __FORCE_INLINE void Set(const Date& date){ *this = date; }

  /*!
    @brief Set new parameters. Automatically calculates day of week and leap
  */
  bool Set(uint16_t year, uint8_t month, uint8_t date);

  /*!
    @brief Manually set new parameters
  */
  __FORCE_INLINE void Set(uint16_t year, uint8_t month, uint8_t date, uint8_t day, bool isLeap = false) {
    *this = Date{ year, month, date, day, isLeap };
  };

  /*!
    @brief Set year to date
  */
  bool SetYear(uint16_t value);

  /*!
    @brief Set month(1-12)
  */
  bool SetMonth(uint8_t value);

  /*!
    @brief Change day of week. Adding days to current date
  */
  bool SetDayOfWeek(uint8_t value);

  /*!
    @brief Change date. Depends on current month
  */
  bool SetDate(uint8_t value);

  /*!
    @brief Add parameters to current
  */
  __FORCE_INLINE void Add(const Date& date){ Add(date.year, date.month, date.date); }

  /*!
    @brief Add parameters to current date
  */
  void Add(size_t year, size_t month, size_t date);

  /*!
    @brief Add years to current date
  */
  void AddYear(size_t value = 1);

  /*!
    @brief Add months to current date
  */
  void AddMonth(size_t value = 1);

  /*!
    @brief Add days to current date
  */
  __FORCE_INLINE void AddDay(size_t value = 1) { _AddSubDay(true, value); return; }

  /*!
    @brief Add weeks to current date
  */
  __FORCE_INLINE void AddWeek(size_t value = 1){ AddDay(value * 7); }

  /*!
    @brief Substruct parameters from current date
  */
  __FORCE_INLINE bool Sub(const Date& date){ return Sub(date.year, date.month, date.date); }

  /*!
    @brief Substruct parameters from current date
  */
  bool Sub(size_t year, size_t month, size_t date);

  /*!
    @brief Substruct years from current date
  */
  bool SubYear(size_t value = 1);
  
  /*!
    @brief Substruct months from current date
  */
  bool SubMonth(size_t value = 1);

  /*!
    @brief Substruct days from current date
  */
  __FORCE_INLINE bool SubDay(size_t value = 1) { return _AddSubDay(false, value); }

  /*!
    @brief Substruct weeks from current date
  */
  __FORCE_INLINE bool SubWeek(size_t value = 1){ return SubDay(value * 7); }

  /*!
    @brief Difference between current-date and date
  */
  Date Difference(const Date& date) const;

  /*!
    @brief Difference between current-date and date. Return in years
  */
  size_t DifferenceInYear(const Date& date) const;

  /*!
    @brief Difference between current-date and date. Return in months
  */
  size_t DifferenceInMonth(const Date& date) const;
  
  /*!
    @brief Difference between current-date and date. Return in days
  */
  size_t DifferenceInDay(const Date& date) const;

  /*!
    @brief Difference between current-date and date. Return in weeks
  */
  __FORCE_INLINE size_t DifferenceInWeek(const Date& date) const { return DifferenceInDay(date) / 7; }

  size_t ConvertToDays() const;

  Date& operator+=(const Date& rhs){ Add(rhs); return *this; }
  Date& operator-=(const Date& rhs){ Sub(rhs); return *this; }

  __FORCE_INLINE bool operator<(const Date& rhs) const { 
    return ((year*32UL + month)*32 + date) < ((rhs.year*32UL + rhs.month)*32 + rhs.date);
  }
  __FORCE_INLINE bool operator>(const Date& rhs) const { return rhs < *this; }
  __FORCE_INLINE bool operator<=(const Date& rhs) const { return !(*this > rhs); }
  __FORCE_INLINE bool operator>=(const Date& rhs) const { return !(*this < rhs); }

  __FORCE_INLINE bool operator==(const Date& rhs) const {return (year == rhs.year && month == rhs.month && date == rhs.date); }
  __FORCE_INLINE bool operator!=(const Date& rhs) const { return !(*this == rhs); }

  Date operator+(const Date& rhs){ return *this += rhs; }
  Date operator-(const Date& rhs){ return *this -= rhs; }

private:

  uint16_t year = 0;
  uint8_t month = 0;
  uint8_t date = 0;
  uint8_t day = 0;
  bool isLeap = false;

  static constexpr uint8_t dayCacl[] = { 0, 12, 8, 8, 11, 13, 9, 11, 7, 10, 12, 8, 10 };
  static constexpr uint8_t daysInMonth[] = { 0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
  static constexpr uint16_t daysFromStart[] = { 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365 };
  
  bool _IsLeap(){ return IsLeap(year); }

  void _CalculationDay();

  bool _CalculationSubDayMonth(uint8_t& t, uint8_t compareValue, size_t value);
  void _CalculationAddDayMonth(uint8_t& t, uint8_t compareValue, size_t value);

  bool _AddSubDay(bool isAdd, size_t value);

  __FORCE_INLINE size_t _convertToMonth() const{ return year * 12UL + month; }

};

bool Date::Set(uint16_t year, uint8_t month, uint8_t date){
  this->isLeap = IsLeap(year);
  if(!month || !date || month > 12 || date > DaysInMonth(this->isLeap, month)) return false;
  this->year = year;
  this->month = month;
  this->date = date;
  _CalculationDay();
  return true;
}

bool Date::SetYear(uint16_t value){
  if (value == year) return false;
  year = value;
  isLeap = _IsLeap();
  if (date > DaysInMonth()) date = DaysInMonth();
  _CalculationDay();
  return true;
}

bool Date::SetMonth(uint8_t value){
  if(!value || value > 12 || value == month) return false;
  month = value;
  if (date > DaysInMonth()) date = DaysInMonth();
  _CalculationDay();
  return true;
}

bool Date::SetDayOfWeek(uint8_t value){
  if(!value || value > 7) return false;
  value = value >= day ? value - day : 7 - (day - value);
  AddDay(value);
  return true;
}

bool Date::SetDate(uint8_t value){
  if(!value || value == date || value > DaysInMonth()) return false;
  date = value;
  _CalculationDay();
  return true;
}

void Date::Add(size_t year, size_t month, size_t date){
  if(year) this->year += year + (this->month + month) / 12UL;
  if (month) _CalculationAddDayMonth(this->month, 12, month);
  isLeap = _IsLeap();
  if (this->date > DaysInMonth()) this->date = DaysInMonth();
  _CalculationDay();
  AddDay(date);
}

void Date::AddYear(size_t value){
  if (!value) return;

  year += value;
  isLeap = _IsLeap();

  if (date > DaysInMonth()) date = DaysInMonth();
  _CalculationDay();
}

void Date::AddMonth(size_t value){
  if (!value) return;
  size_t years = (value + month) / 12UL;
  _CalculationAddDayMonth(month, 12, value);
  if (years){
    year += years;
    isLeap = _IsLeap();
  }
  if (date > DaysInMonth()) date = DaysInMonth();
  _CalculationDay();
}

bool Date::Sub(size_t year, size_t month, size_t date){
  if (!year && !month && !date) return false;

  if (_CalculationSubDayMonth(this->month, 12, month)) year += month / 12 + 1;

  if (year) {
    this->year -= year;
    isLeap = _IsLeap();
  }

  if (date) SubDay(date);
  else if (this->date > DaysInMonth()) this->date = DaysInMonth();
  _CalculationDay();

  return true;
}

bool Date::SubYear(size_t value){
  if (!value || value > year) return false;
  year -= value;
  isLeap = IsLeap();
  if (date > DaysInMonth()) date = DaysInMonth();
  _CalculationDay();
  return true;
}

bool Date::SubMonth(size_t value){
  if (!value || (value > _convertToMonth() )) return false;

  if (_CalculationSubDayMonth(month, 12, value)){
    year -= month / 12 + 1;
    isLeap = _IsLeap();
  }

  if (date > DaysInMonth()) date = DaysInMonth();
  _CalculationDay();
  return true;
}

Date Date::Difference(const Date& date) const {
  if (*this == date) return Date();
  
  size_t month = DifferenceInMonth(date);
  size_t year = month / 12;
  month = month % 12;
  size_t days;
  bool isGreater = *this > date;
  size_t lowerDate = isGreater ? date.date : this->date;
  size_t greaterDate = isGreater ? this->date : date.date;
  if (greaterDate >= lowerDate) days = greaterDate - lowerDate;
  else {
    size_t daysDaysTillMonthEnd = isGreater ? date.DaysTillMonthEnd() : DaysTillMonthEnd();
    days = daysDaysTillMonthEnd + greaterDate;
    if (month) month--;
    else {
      year--;
      month = 11;
    }
  }

  return Date{ (uint16_t)year, (uint8_t)month, (uint8_t)days, 0 };
}

size_t Date::DifferenceInYear(const Date& date) const {
  if (this->year == date.year) return 0;
  size_t difference;
  size_t substractionalDay;
  if (this->year > date.year){
    substractionalDay = (this->month*32UL + this->date) >= (date.month*32UL + date.date) ? 0 : 1;
    difference = this->year - date.year;
  } else{
    substractionalDay = (date.month*32UL + date.date) >= (this->month*32UL + this->date) ? 0 : 1;
    difference = date.year - this->year;    
  }
  return difference - substractionalDay;
}

size_t Date::DifferenceInMonth(const Date& date) const {
  size_t month1 = _convertToMonth();
  size_t month2 = date._convertToMonth();
  if (month1 == month2) return 0;
  if (month1 > month2){
    size_t sub = this->date >= date.date ? 0 : 1;
    return month1 - month2 - sub;
  } else{
    size_t sub = date.date >= this->date ? 0 : 1;
    return month2 - month1 - sub; 
  }
}

size_t Date::DifferenceInDay(const Date& date) const {
  if (*this == date) return 0;
  size_t days1 = ConvertToDays();
  size_t days2 = date.ConvertToDays();
  return days1 > days2 ? days1 - days2 : days2 - days1;
}

size_t Date::ConvertToDays() const {
  size_t m = month;
  size_t y = year;
  if (m < 3) {
    y--;
    m += 12;
  }
  return 365 * y + y / 4 - y / 100 + y / 400 + (153 * m - 457) / 5 + date - 306;
}

void Date::_CalculationDay() {
  size_t tmp = year + year / 4 + dayCacl[month] + date;
  if (isLeap && (month < 3)) tmp -= 1;
  day = tmp % 7;
  if (!day) day = 7;

}

bool Date::_CalculationSubDayMonth(uint8_t& t, uint8_t compareValue, size_t value) {
  if (value >= t) {
    t = (t + compareValue) - value % compareValue;
    t = t - (t > compareValue ? compareValue : 0);
    return true;
  } else t -= value;
  return false;
}

void Date::_CalculationAddDayMonth(uint8_t& t, uint8_t compareValue, size_t value) {
  t = (t + value) % compareValue;
  if (!t) t = compareValue;
}

bool Date::_AddSubDay(bool isAdd, size_t value) {

  if (!value) return false;

  size_t tillEnd;
  size_t fromStart;
  size_t iter;

  if (isAdd) {
    _CalculationAddDayMonth(day, 7, value);
    tillEnd = DaysTillYearEnd();
    fromStart = value > tillEnd ? 0 : DaysFromYearStart();
    iter = tillEnd;
  } else {
    _CalculationSubDayMonth(day, 7, value);
    fromStart = DaysFromYearStart();
    tillEnd = value > fromStart ? 0 : DaysTillYearEnd();
    iter = fromStart;
  }

  while (value >= iter) {
    if (isAdd) year++;
    else year--;
    isLeap = IsLeap(year);
    value -= iter;
    iter = isLeap ? 366 : 365;
  }

  fromStart = isAdd ? fromStart + value : iter - value;
  month = 1;

  auto addLeapDay = [=](size_t m) -> size_t {return (isLeap && m > 1) ? 1 : 0; };

  while (daysFromStart[month] + addLeapDay(month) < fromStart) month++;
  date = fromStart - daysFromStart[month - 1] - addLeapDay(month - 1);

  return true;
}

} // namespace utils

#endif // !_DATE_H