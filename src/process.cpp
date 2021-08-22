#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "process.h"
// Added linux_parser's header here to make functions available to Process-Class.
#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

// Added the constructor to set the ID
Process::Process(int i) : id(i) {}

// TODO: Return this process's ID -> OK
int Process::Pid() { return id; }

// TODO: Return this process's CPU utilization
float Process::CpuUtilization() { return 0; }

// TODO: Return the command that generated this process -> OK
string Process::Command() { return LinuxParser::Command(Pid()); }

// TODO: Return this process's memory utilization -> OK
string Process::Ram() { return LinuxParser::Ram(Pid()); }

// TODO: Return the user (name) that generated this process -> OK
string Process::User() { return LinuxParser::User(Pid()); }

// TODO: Return the age of this process (in seconds) -> OK
long int Process::UpTime() { return LinuxParser::UpTime(Pid()); }

// TODO: Overload the "less than" comparison operator for Process objects
// REMOVE: [[maybe_unused]] once you define the function
bool Process::operator<(Process const& a) const { 
  // "Size" of a process is measured by RAM-Utilization, so the RAM-Utilization
  // of this process must be compared to the RAM-Utilization of a:Process.
  // Convert String to a number & compare in one line
  return true; 
}