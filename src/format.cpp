#include <string>
#include <sstream>

#include "format.h"

using std::string;

// TODO: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
string Format::ElapsedTime(long seconds) { 
  int hours, mins, secs;
  std::stringstream elapsedTime;
  hours = (seconds / 3600) % 100;
  mins = (seconds / 60) % 60;
  secs = seconds % 60;
  elapsedTime << hours << ":" << mins << ":" << secs;
  return elapsedTime.str();
}