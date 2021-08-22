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
int Process::Pid() { return this->id; }

// TODO: Return this process's CPU utilization
float Process::CpuUtilization() { return 0; }

// TODO: Return the command that generated this process -> OK
string Process::Command() { return LinuxParser::Command(this->Pid()); }

// TODO: Return this process's memory utilization -> OK
string Process::Ram() { return LinuxParser::Ram(this->Pid()); }

// TODO: Return the user (name) that generated this process -> OK
string Process::User() { return LinuxParser::User(this->Pid()); }

// TODO: Return the age of this process (in seconds) -> OK
long int Process::UpTime() { return LinuxParser::UpTime(this->Pid()); }

// TODO: Overload the "less than" comparison operator for Process objects
// REMOVE: [[maybe_unused]] once you define the function
bool Process::operator<(Process const& a[[maybe_unused]]) const { return true; }