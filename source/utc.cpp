#include <ctime>
#include <utc.hpp>

namespace tmUtil {
int const tm_yearCorrection = -1900;
int const tm_monthCorrection = -1;
int const tm_isdst_dontKnow = -1;

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