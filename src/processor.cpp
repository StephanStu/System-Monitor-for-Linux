#include "processor.h"
#include "linux_parser.h"

// TODO: Return the aggregate CPU utilization -> OK
float Processor::Utilization() { 
  return LinuxParser::ActiveJiffies() * (1.f / LinuxParser::Jiffies());
}