#include <ctime>

namespace tmUtil {
static bool isLeap(int year);
static int daysIn(int year);
} // namespace tmUtil

time_t utc(int year, int mon, int day, int hour, int min, int sec);
time_t utcnow();