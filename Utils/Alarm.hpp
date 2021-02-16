//----------------------------------------------------------------------------------
//  Author:       Semyon Ivanov
//  e-mail:       agreement90@mail.ru
//  github:       https://github.com/7bnx/Embedded
//  Description:  Alarm
//  TODO:
//----------------------------------------------------------------------------------

#ifndef _ALARM_HPP
#define _ALARM_HPP

#include "TimeDate.hpp"

/*!
  @brief Namespace for utils
*/
namespace utils{

/*!
  @brief Namespace for alarm configuration
*/
namespace alarm{

/*!
  @brief Mode of alarm
*/
enum class mode{

  //!@brief Alarm triggers once
  Once,

  //!@brief Alarm triggers on every selected day of week
  DayOfWeek,

  //!@brief Alarm triggers on every selected day of month
  DayOfMonth,

  //!@brief Alarm triggers from Monday till Friday every week
  WorkDays,

  //!@brief Alarm triggers on Sunday and Saturday every week
  Weekend,

  //!@brief Alarm triggers every day
  EveryDay
};

/*!
  @brief Structure of alarm data
*/
struct alarmData{
  alarm::mode mode = alarm::mode::Once;

  //!@brief Current trigger time
  TimeDate timedate;

  //!@brief Start time of alarm
  Time start;

  //!@brief Period to retrigger
  Time period;

  //!@brief End time of alarm
  Time end;

  //!@brief Function to execute
  void (*pFunc)() = nullptr;
  bool isEnabled = false;
  __FORCE_INLINE bool operator<(const alarmData& rhs) const { return timedate < rhs.timedate; }
  __FORCE_INLINE bool operator<=(const alarmData& rhs) const { return timedate <= rhs.timedate; }
  __FORCE_INLINE bool operator==(const alarmData& rhs) const{ return timedate == rhs.timedate; }
  __FORCE_INLINE bool operator!=(const alarmData& rhs) const{ return timedate != rhs.timedate; }
};

}

/*!
  @brief Alarm class
  @tparam <Clock> should implement: Get() - get current time and date, GetAlarm() - get current alarm, 
                  SetAlarm(timedate) - set alarm time and date
  @tparam <numberAlarms> number of alarms
*/
template<typename Clock, size_t numberAlarms = 1>
class Alarm{
public:

  /*!
    @brief Executes alarm-functions amd calculates and set next alarm time to Clock
  */
  static void Handler();

  /*!
    @brief Get enable-state of the selected alarm
  */
  static bool IsEnable(size_t number) { return alarms[number].isEnabled; }

  /*!
    @brief Get number of alarms
  */
  static size_t GetNumberAlarms(){ return numberAlarms; }

  /*!
    @brief Get number of enabled alarms
  */
  static size_t GetNumberEnabled(){ return numberEnabled; }

  /*!
    @brief Get number of disabled alarms
  */
  static size_t GetNumberDisabled(){ return numberAlarms - numberEnabled; }

  /*!
    @brief Get data of the selected alarm
  */
  static const alarm::alarmData& Get(size_t number = 0){ return alarms[number]; }

  /*!
    @brief Recalculating time and date of all enabled alarms. Use it in case of change Clock-time
  */
  static void RecalculateQueue();

  /*!
    @brief Set alarm to queue
    @param [in] alarm data of the alarm
    @param [in] number of alarm. If ommitted - first disabled number
    @return false, if current time is greater then alarm
  */
  static bool Set(const alarm::alarmData& alarm, size_t number = _GetFirstDisabledNumber()){
    return _Set(number, alarm::mode::Once, alarm.pFunc, alarm.start, alarm.period, alarm.end);
  }

  /*!
    @brief Set alarm to queue. Once-mode
    @param [in] pFunc function to execute
    @param [in] timedate time and date of alarm
    @param [in] period between trigger of alarm from start time till end time
    @param [in] end time for alarm
    @param [in] number of alarm. If ommitted - first disabled number
    @return false, if current time is greater then alarm
  */
  static bool SetOnce(void (*const pFunc)(), utils::TimeDate timedate, const utils::Time& period = utils::Time(), 
                      const utils::Time& end = utils::Time(), size_t number = _GetFirstDisabledNumber()){
    alarms[number].timedate = timedate;
    return _Set(number, alarm::mode::Once, pFunc, timedate.GetTime(), period, end);
  }

  /*!
    @brief Set alarm to queue. Day of week-mode
    @param [in] pFunc function to execute
    @param [in] start time of alarm
    @param [in] day of week 
    @param [in] period between trigger of alarm from start time till end time
    @param [in] end time for alarm
    @param [in] number of alarm. If ommitted - first disabled number
  */
  static bool SetDayOfWeek(void (*const pFunc)(), const utils::Time& start, size_t day, const utils::Time& period = utils::Time(), 
                           const utils::Time& end = utils::Time(), size_t number = _GetFirstDisabledNumber()){
    alarms[number].timedate.Set(utils::Date{0, 0, 0, day});
    return _Set(number, alarm::mode::DayOfWeek, pFunc, start, period, end);
  }

  /*!
    @brief Set alarm to queue. Day of month-mode
    @param [in] pFunc function to execute
    @param [in] start time of alarm
    @param [in] day of month
    @param [in] period between trigger of alarm from start time till end time
    @param [in] end time for alarm
    @param [in] number of alarm. If ommitted - first disabled number
    @return false, if there is no selected date in month. E.g.: 30th Febrary 
  */
  static bool SetDayOfMonth(void (*const pFunc)(), const utils::Time& start, size_t day, const utils::Time& period = utils::Time(),
                            const utils::Time& end = utils::Time(), size_t number = _GetFirstDisabledNumber()){
    alarms[number].timedate.Set(utils::Date{0, 0, day, 0});
    return _Set(number, alarm::mode::DayOfMonth, pFunc, start, period, end);
  }


  /*!
    @brief Set alarm to queue. Work days-mode
    @param [in] pFunc function to execute
    @param [in] start time of alarm
    @param [in] day of month
    @param [in] period between trigger of alarm from start time till end time
    @param [in] end time for alarm
    @param [in] number of alarm. If ommitted - first disabled number
  */
  static bool SetWorkDays(void (*const pFunc)(), const utils::Time& start, const utils::Time& period = utils::Time(),
                          const utils::Time& end = utils::Time(), size_t number = _GetFirstDisabledNumber()){
    return _Set(number, alarm::mode::WorkDays, pFunc, start, period, end);
  }

  /*!
    @brief Set alarm to queue. Weekend-mode
    @param [in] pFunc function to execute
    @param [in] start time of alarm
    @param [in] day of month
    @param [in] period between trigger of alarm from start time till end time
    @param [in] end time for alarm
    @param [in] number of alarm. If ommitted - first disabled number
  */
  static bool SetWeekend(void (*const pFunc)(), const utils::Time& start, const utils::Time& period = utils::Time(),
                         const utils::Time& end = utils::Time(), size_t number = _GetFirstDisabledNumber()){
    return _Set(number, alarm::mode::Weekend, pFunc, start, period, end);
  }

  /*!
    @brief Set alarm to queue. Every day-mode
    @param [in] pFunc function to execute
    @param [in] start time of alarm
    @param [in] day of month
    @param [in] period between trigger of alarm from start time till end time
    @param [in] end time for alarm
    @param [in] number of alarm. If ommitted - first disabled number
  */
  static bool SetEveryDay(void (*const pFunc)(), const utils::Time& start, const utils::Time& period = utils::Time(),
                          const utils::Time& end = utils::Time(), size_t number = _GetFirstDisabledNumber()){
    return _Set(number, alarm::mode::EveryDay, pFunc, start, period, end);
  }

  /*!
    @brief Delete selected alarm
    @param [in] number of alarm
  */
  static void Delete(size_t number);

  /*!
    @brief Delete all alarms
  */
  static void Delete();

private:

  static size_t _GetFirstDisabledNumber() {
    for (size_t i = 0; i < numberAlarms; ++i)
      if (!alarms[i].isEnabled) return i;
    return numberAlarms - 1;
  }

  static bool _Set(size_t number, alarm::mode mode, void (*const pFunc)(),
                  const utils::Time& start, const utils::Time& period, const utils::Time& end){
    alarms[number].mode = mode;
    alarms[number].start = start;
    alarms[number].period = period;
    alarms[number].end = end;
    if (!_CalculateAlarmTime(Clock::Get(), number)) return false;
    
    if (!alarms[number].isEnabled){
      numberEnabled++;
      alarms[number].isEnabled = true;
    }

    alarms[number].pFunc = pFunc;
    
    _InsertToQueue(number);
    _SetAlarmToClock();
    
    return true;
  }

  static bool _CalculateAlarmTime(const utils::TimeDate& currentTimedate, size_t number){
    using namespace alarm;

    auto timedate = currentTimedate;
    utils::Time start = alarms[number].start;
    
    bool isNextDay = !_CalculateTimeByPeriod(start, timedate.GetTime(), number);
    bool isStartTime = false;
    size_t currentDate = timedate.GetDate().GetDate();
    size_t currentDay = timedate.GetDate().GetDay();
    switch(alarms[number].mode){
      case mode::Once:
        if (alarms[number].timedate.GetDate() > timedate.GetDate()) isStartTime = true;
        else if (alarms[number].timedate.GetDate() < timedate.GetDate() ||
          (alarms[number].timedate.GetDate() == timedate.GetDate() && isNextDay)) {
          Delete(number);
          return false;
        }
        break;
      case mode::DayOfMonth:
        if (size_t date = alarms[number].timedate.GetDate().GetDate(); currentDate != date || isNextDay){
          if (date < currentDate)
            timedate.AddMonth();
          if (!timedate.SetDate(date)){
            Delete(number);
            return false;
          }
          isStartTime = true;
        }
        break;
      case mode::DayOfWeek:
        if (size_t day = alarms[number].timedate.GetDate().GetDay(); currentDay != day || isNextDay){
          timedate.SetDayOfWeek(day);
          isStartTime = true;
        }
        break;
      case mode::WorkDays:
        if (currentDay > 5 || isNextDay){
          if (currentDay >= 5) currentDay = 1;
          else currentDay++;
          timedate.SetDayOfWeek(currentDay);
          isStartTime = true;
        }
        break;
      case mode::Weekend:
        if (currentDay < 6 || isNextDay){
          if (currentDay == 6) currentDay = 7;
          else currentDay = 6;
          timedate.SetDayOfWeek(currentDay);
          isStartTime = true;
        }
        break;
      case mode::EveryDay:
        if (isNextDay){
          timedate.AddDay();
          isStartTime = true;
        }
        break;
    }
    alarms[number].timedate = utils::TimeDate{ timedate.GetDate(), isStartTime ? alarms[number].start : start };
    return true;
  }

  __FORCE_INLINE static bool _CalculateTimeByPeriod(utils::Time& start, const utils::Time& current, size_t number){
    if (current < start) return true;
    if (current >= alarms[number].end || !alarms[number].period || !alarms[number].end) return false;
    while(current >= start)
      if (start.Add(alarms[number].period) || start > alarms[number].end)
        return false;
    return true;
  }


  static void _InsertToQueue(size_t number) {
    if (numberEnabled < 2) {
      queue[0] = number;
      return;
    }
    
    size_t numberTMP = queue[0];
    size_t currentPosition = number == numberTMP ? 0 : numberEnabled;
    size_t newPosition = numberEnabled;

    for (size_t i = 1; i < currentPosition || i < newPosition; ++i) {
      
      if ((currentPosition == numberEnabled) && number == queue[i]) currentPosition = i;

      if (newPosition == numberEnabled && (alarms[number] < alarms[queue[i - 1]] || 
         (number == queue[i - 1] && alarms[number] < alarms[queue[i]]))) {
        newPosition = i - 1;
        numberTMP = queue[i - 1];
      }

      if (newPosition != numberEnabled || currentPosition != numberEnabled) {
        if (newPosition > currentPosition)
          queue[i-1] = queue[i];
        else if (currentPosition > newPosition){
          size_t tmp2 = queue[i];
          queue[i] = numberTMP;
          numberTMP = tmp2;
        }
      }
    }

    if (newPosition == numberEnabled) 
      --newPosition;

    queue[newPosition] = number;
  }

  static bool _SetAlarmToClock(){
    size_t number = queue[0];
    numberToExecute = 1;
    while (numberToExecute < numberEnabled && alarms[number] == alarms[queue[numberToExecute]])
      numberToExecute++;
    if (alarms[number].timedate != Clock::GetAlarm()) {
      Clock::SetAlarm(alarms[number].timedate);
      return true;
    }
    return false;
  }

  static inline size_t numberEnabled = 0;
  static inline size_t queue[numberAlarms];
  static inline size_t numberToExecute = 0;
  static inline alarm::alarmData alarms[numberAlarms];

};

template<typename Clock, size_t numberAlarms>
void Alarm<Clock, numberAlarms>::Handler() {
  utils::TimeDate currentTimedate = Clock::Get();
  if (numberToExecute && currentTimedate >= alarms[queue[0]].timedate) {
    for (size_t i = 0; i < numberToExecute; ++i) {
      size_t number = queue[i];
      if (alarms[number].pFunc) alarms[number].pFunc();
      _CalculateAlarmTime(currentTimedate, number);
      _InsertToQueue(number);
    }
    _SetAlarmToClock();
  }
}

template<typename Clock, size_t numberAlarms>
void Alarm<Clock, numberAlarms>::Delete(size_t number){
  if (alarms[number].isEnabled){
    alarms[number].isEnabled = false;
    size_t queuePosition = numberEnabled;
    for (size_t i = 0; i < numberEnabled; ++i){
      size_t numberIter = queue[i];
      if (queuePosition == numberEnabled && numberIter == number) queuePosition = i;
      if (queuePosition != numberEnabled) numberIter = queue[i + 1];
    }
    numberEnabled--;
    if (queuePosition == 0 && numberEnabled) _SetAlarmToClock();
  }
}

template<typename Clock, size_t numberAlarms>
void Alarm<Clock, numberAlarms>::Delete(){
  for (size_t i = 0; i < numberEnabled; i++)
    alarms[queue[i]].isEnabled = false;

  numberEnabled = 0;
  numberToExecute = 0;
}

template<typename Clock, size_t numberAlarms>
void Alarm<Clock, numberAlarms>::RecalculateQueue() {
  utils::TimeDate currentTimedate = Clock::Get();
  for (size_t i = 0; i < numberEnabled; ++i)
    _CalculateAlarmTime(currentTimedate, queue[i]);
  for (size_t i = 0, countEnabled = 0; i < numberAlarms && countEnabled < numberEnabled; ++i) 
    if (alarms[i].isEnabled) {
      countEnabled++;
      _InsertToQueue(i);
    }
  _SetAlarmToClock();
}

} // !namespace utils

#endif // !_ALARM_HPP