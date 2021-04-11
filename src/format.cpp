#include "format.h"

#include <sstream>
#include <string>

using std::string;

string Format::ElapsedTime(long seconds) {
  // inspired from
  // https://www.programmingnotes.org/2062/c-convert-time-from-seconds-into-hours-min-sec-format/
  long int time = seconds;
  int hour = 0;
  int min = 0;
  int sec = 0;

  hour = time / 3600;
  time = time % 3600;
  min = time / 60;
  time = time % 60;
  sec = time;

  std::ostringstream oss;
  oss << "" << hour << ":" << min << ":" << sec;

  return oss.str();
}