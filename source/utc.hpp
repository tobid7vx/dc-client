#include <ctime>
#include <vector>

namespace tmUtil {
static bool isLeap(int year);
static int daysIn(int year);
} // namespace tmUtil

struct CTIME_Z {
  int ye;
  int mo;
  int da;
  int h;
  int m;
  int s;
};

time_t utc(int year, int mon, int day, int hour, int min, int sec);
time_t utcnow();
std::vector<int> GetYearList();
std::vector<int> GetMonthList();
std::vector<int> GetDayList();
std::vector<int> GetHourList(bool ampm = false);
std::vector<int> GetMinSecList();
CTIME_Z GetCurrentTime();