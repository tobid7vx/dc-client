#include <ctime>
#include <utc.hpp>

namespace tmUtil {
int const tm_yearCorrection = -1900;
int const tm_monthCorrection = -1;
int const tm_isdst_dontKnow = -1;
int const tm_ryearCorrection = +1900;
int const tm_rmonthCorrection = +1;

#if !defined(DEBUG_DATETIME_TIMEGM_ENVVARTZ) &&                                \
    !(defined(UNIX) && !defined(DEBUG_DATETIME_TIMEGM))
static bool isLeap(int year) {
  return (year % 4) ? false : (year % 100) ? true : (year % 400) ? false : true;
}

static int daysIn(int year) { return isLeap(year) ? 366 : 365; }
#endif
} // namespace tmUtil

time_t utc(int year, int mon, int day, int hour, int min, int sec) {
  struct tm time = {0};
  time.tm_year = year + tmUtil::tm_yearCorrection;
  time.tm_mon = mon + tmUtil::tm_monthCorrection;
  time.tm_mday = day;
  time.tm_hour = hour;
  time.tm_min = min;
  time.tm_sec = sec;
  time.tm_isdst = tmUtil::tm_isdst_dontKnow;

  time_t result;
  result = std::mktime(&time);
  return result;
}

time_t utcnow() {
  std::time_t t = std::time(0); // get time now
  std::tm *now = std::localtime(&t);
  std::time_t res = std::mktime(now);
  return res;
}

std::vector<int> GetYearList() {
  std::vector<int> res;
  std::time_t t = std::time(0); // get time now
  std::tm *now = std::localtime(&t);
  for (int i = 1990; i < now->tm_year + 1; i++) {
    res.push_back(i);
  }
  return res;
}

std::vector<int> GetMonthList() {
  std::vector<int> res;
  for (int i = 1; i < 13; i++) {
    res.push_back(i);
  }
  return res;
}

std::vector<int> GetDayList() {
  std::vector<int> res;
  for (int i = 1; i < 32; i++) {
    res.push_back(i);
  }
  return res;
}

std::vector<int> GetHourList(bool ampm) {
  std::vector<int> res;
  for (int i = 0; i < /*ampm ? 13 : */ 24; i++) {
    res.push_back(i);
  }
  return res;
}

std::vector<int> GetMinSecList() {
  std::vector<int> res;
  for (int i = 0; i < 60; i++) {
    res.push_back(i);
  }
  return res;
}

CTIME_Z GetCurrentTime() {
  CTIME_Z res;
  std::time_t t = std::time(0); // get time now
  std::tm *now = std::localtime(&t);
  res.ye = now->tm_year + tmUtil::tm_ryearCorrection;
  res.mo = now->tm_mon + tmUtil::tm_rmonthCorrection;
  res.da = now->tm_mday;
  res.h = now->tm_hour;
  res.m = now->tm_min;
  res.s = now->tm_sec;
  return res;
}