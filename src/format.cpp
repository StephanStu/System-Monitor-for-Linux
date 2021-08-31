#include <string>
#include <sstream>
#include <iomanip>

#include "format.h"

using std::string;

// TODO: Complete this helper function -> OK
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// Taken from 
// - https://knowledge.udacity.com/questions/155686 
// - https://docs.microsoft.com/de-de/cpp/standard-library/iomanip-functions?view=msvc-160#setfill, 
// - https://stackoverflow.com/questions/48310746/converting-time-into-hours-minutes-and-seconds
// iomanip-headers make it possible to format outputs as required here.
string Format::ElapsedTime(long seconds) { 
  int hours, mins, secs;
  std::ostringstream elapsedTime;
  hours = (seconds / 3600) % 100;
  mins = (seconds / 60) % 60;
  secs = seconds % 60;
  elapsedTime  << std::setw(2) << std::setfill('0') << hours << ":" 
     << std::setw(2) << std::setfill('0') << mins << ":"
     << std::setw(2) << std::setfill('0') << secs;
  return elapsedTime.str();
}