#include <unistd.h>
#include <cstddef>
#include <set>
#include <string>
#include <vector>
#include <iostream>
#include "process.h"
#include "processor.h"
#include "system.h"
// Added linux_parser's header here to make functions available to System-Class.
#include "linux_parser.h"
using std::set;
using std::size_t;
using std::string;
using std::vector;

// TODO: Return the system's CPU
Processor& System::Cpu() { return cpu_; }

// TODO: Return a container composed of the system's processes
vector<Process>& System::Processes() {
  // Initialize a clean vector of :Process
  processes_.clear();
  // Get the process-IDs using the function available in Linux-Parser  
  vector<int> processIds = LinuxParser::Pids();
  // Sort the vector of process-IDs before generating the :Process and feeding into the processes_-vector that must be returned
  sort(processIds.begin(), processIds.end(), std::greater<int>());
  // Now create a vector of :Process
  for(int id: processIds){
    processes_.push_back(Process(id));
  }
  return processes_;
}

// TODO: Return the system's kernel identifier (string) -> OK
std::string System::Kernel() { return LinuxParser::Kernel();}

// TODO: Return the system's memory utilization -> OK
float System::MemoryUtilization() { return LinuxParser::MemoryUtilization(); }

// TODO: Return the operating system name -> OK
std::string System::OperatingSystem() { return LinuxParser::OperatingSystem(); }

// TODO: Return the number of processes actively running on the system -> OK
int System::RunningProcesses() { return LinuxParser::RunningProcesses(); }

// TODO: Return the total number of processes on the system -> OK
int System::TotalProcesses() { return LinuxParser::TotalProcesses(); }

// TODO: Return the number of seconds since the system started running
long int System::UpTime() { return 0; }