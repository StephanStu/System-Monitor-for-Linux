#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>
#include <assert.h>
#include <iostream>

#include "linux_parser.h"

using std::string;
using std::vector;

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, version, kernel;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

// TODO: Read and return the system memory utilization -> OK
// Get it by
//
// # cat /proc/meminfo 
//
// This returns something like
// MemTotal:        7648760 kB
// MemFree:         4639460 kB
// MemAvailable:    6459764 kB
// Buffers:          145020 kB
// Cached:          1766788 kB
// ...
float LinuxParser::MemoryUtilization() {
  string line, attribute, value, memoryFree, memoryTotal;
  std::ifstream memoryInfo(kProcDirectory + kMeminfoFilename);
  assert(memoryInfo.is_open());
  while(std::getline(memoryInfo, line)){
    std::istringstream currentLine(line);
    while(currentLine >> attribute >> value){
      if(attribute == "MemTotal:"){memoryTotal = value;}
      if(attribute == "MemFree:"){memoryFree = value;}
    }
  }
  return (std::stof(memoryTotal) - std::stof(memoryFree)) / std::stof(memoryTotal);
}

// TODO: Read and return the system uptime -> OK
// Get it by
//
// #cat /proc/uptime 
//
// The first value represents the total number of seconds the system has been up. 
// The second value is the sum of how much time each core has spent idle, in 
// seconds. Consequently, the second value may be greater than the overall system 
// uptime on systems with multiple cores
long LinuxParser::UpTime() {
  string totalSecondsUp;
  string line;
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> totalSecondsUp;
  }
  return std::stol(totalSecondsUp);
}

// TODO: Read and return the number of jiffies for the system -> Ok
// Get it by
//
// #cat /proc/stat 
// 
// Call to CpuUtilization gives the numbers as strings already.
long LinuxParser::Jiffies() {
  vector<string> values = LinuxParser::CpuUtilization();
  long sum = 0;
  vector<CPUStates> allRelevantStates = {kUser_, kNice_, kSystem_, kIdle_, kIOwait_, kIRQ_, kSoftIRQ_, kSteal_};
  for (int i : allRelevantStates) {
    sum += std::stol(values[i]);
  };
  return sum;
}

// TODO: Read and return the number of active jiffies for a PID -> OK
// Get it by
//
// # cat /proc/1613/stat for process with ID 1613
// 
// This returns something like 
// 1613 (bash) S 41 1613 1613 34816 2462 4194304 240 173380 0 0 0 0 396 71 20 0 1 0 65907 18460672 694 18446744073709551615 4194304 5192964 140726364545424 0 0 0 65536 4100 65538 1 0 0 17 0 0 0 0 0 0 7290352 7326856 22392832 140726364552505 140726364552771 140726364552771 140726364553198 0
long LinuxParser::ActiveJiffies(int pid) {
  string line, value;
  vector<string> values;
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    while (linestream >> value) {
      values.push_back(value);
    }
  }
  return std::stol(values[13] + values[14]);
}

// TODO: Read and return the number of active jiffies for the system -> OK
// Get it by
//
// #cat /proc/stat and take the sum over CPU-States "user", "system", "soft-irq", "nice", "irq", "steal". 
// 
// Call to CpuUtilization gives the numbers as strings already.
long LinuxParser::ActiveJiffies() {
  vector<string> allJiffies = LinuxParser::CpuUtilization();
  return std::stol(allJiffies[CPUStates::kUser_]) + std::stol(allJiffies[CPUStates::kNice_]) +
         std::stol(allJiffies[CPUStates::kSystem_]) + std::stol(allJiffies[CPUStates::kIRQ_]) +
         std::stol(allJiffies[CPUStates::kSoftIRQ_]) + std::stol(allJiffies[CPUStates::kSteal_]);
}

// TODO: Read and return the number of idle jiffies for the system -> OK
// Get it by
//
// #cat /proc/stat and take the sum over CPU-States "idle" + "waiting". 
// 
// Call to CpuUtilization gives the numbers as strings already.
long LinuxParser::IdleJiffies() {
  vector<string> allJiffies = LinuxParser::CpuUtilization();
  return std::stol(allJiffies[CPUStates::kIdle_]) + std::stol(allJiffies[CPUStates::kIOwait_]);
}

// TODO: Read and return CPU utilization -> OK
// Get it by
//
// #cat /proc/stat 
//
// This returns something like 
// cpu  17049 8894 6469 274558 1472 0 55 0 0 0 --> These numbers are clock ticks, a.k.a. Jiffies
// cpu0 9600 4584 3222 135603 1175 0 35 0 0 0
// cpu1 7448 4309 3246 138954 296 0 19 0 0 0
// intr 588118 39 9 0 0 10025 0 0 0 1 0 0 0 152 0 0 0 0 0 0 0 0 0 0 0 0 0 0 115389 0 3 ...
// ...
// Let us catch the values in the first line here.
vector<string> LinuxParser::CpuUtilization() {
  string line, value, attribute;
  vector<string> values;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> attribute;
    while (linestream >> value) {
      values.push_back(value);
    };
  }
  return values;

}

// TODO: Read and return the total number of processes -> OK
// Get it by
//
// #cat /proc/stat
//
// This will return something like
// ...
// ctxt 1568270
// btime 1630232809
// processes 32739
// procs_running 1
// procs_blocked 0
// softirq 589392 0 180964 0 91672 0 0 312 117239 0 199205
// 
// Now catch "processes"
int LinuxParser::TotalProcesses() {
  string line, attribute, value;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      while (linestream >> attribute >> value) {
        if (attribute == "processes") {
          return std::stoi(value);
        }
      }
    }
  }
  return 0;
}

// TODO: Read and return the number of running processes -> OK
// Get it by
//
// #cat /proc/stat
//
// This will return something like
// ...
// ctxt 1568270
// btime 1630232809
// processes 32739
// procs_running 1
// procs_blocked 0
// softirq 589392 0 180964 0 91672 0 0 312 117239 0 199205
// 
// Now catch "procs_running"
int LinuxParser::RunningProcesses() {
  string line, attribute, value;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      while (linestream >> attribute >> value) {
        if (attribute == "procs_running") {
          return std::stoi(value);
        }
      }
    }
  }
  return 0;
}

// TODO: Read and return the command associated with a process -> OK
// Get it by
// 
// #cat /proc/1613/cmdline  for process 1613
//
// This will return something like
// /bin/bash-c
//     export NODE_ENV=development;
//     export WORKSPACEID=r812755c825360xREACToxev3j25;
//     export WORKSPACEDOMAIN=udacity-student-workspaces.com;
//     sudo apt update
// sudo apt install -y libncurses5-dev libncursesw5-dev
//     cd /home/workspace && bash
// Now catch
string LinuxParser::Command(int pid) {
  string command;
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kCmdlineFilename);
  if (stream.is_open()) {
    std::getline(stream, command);
  }
  return command;
}

// TODO: Read and return the memory used by a process -> OK
// Get it by
//
// #cat /proc/2753/status for process with ID 2753
//
// This will return something like
// ..
// Uid:      0         0         0         0
// Gid:      0         0         0         0
// FDSize:   64
// Groups:    
// NStgid:   2753
// NSpid:    2753
// NSpgid:   435
// NSsid:    435
// VmPeak:      58300 kB
// VmSize:      58300 kB
// VmLck:           0 kB
// VmPin:           0 kB
// VmHWM:        4656 kB
// VmRSS:        4656 kB
// Now let us catch the VMSize's value
string LinuxParser::Ram(int pid) {
  string line, attribute, value;
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatusFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      while (linestream >> attribute) {
        if (attribute == "VmSize:") {
          linestream >> value;
          return std::to_string(std::stol(value) / 1024);
        }
      }
    }
  }
  return "0";
}

// TODO: Read and return the user ID associated with a process -> OK
// Get it by
//
// #cat /proc/2753/status for process with ID 2753
//
// This will return something like
// ..
// Uid:      0         0         0         0
// Gid:      0         0         0         0
// FDSize:   64
// Groups:    
// NStgid:   2753
// NSpid:    2753
// NSpgid:   435
// NSsid:    435
// VmPeak:      58300 kB
// VmSize:      58300 kB
// VmLck:           0 kB
// VmPin:           0 kB
// VmHWM:        4656 kB
// VmRSS:        4656 kB
// Now let us catch the Uid's value
string LinuxParser::Uid(int pid) {
  string line, attribute, value;
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatusFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      while (linestream >> attribute >> value) {
        if (attribute == "Uid:") {
          return value;
        }
      }
    }
  }
  return "0";
}

// TODO: Read and return the user associated with a process -> OK
// Get the user-id-to-user-mapping by
//
// #cat /etc/passwd
//
// This returns something like
// root:x:0:0:root:/root:/bin/bash
// daemon:x:1:1:daemon:/usr/sbin:/usr/sbin/nologin
// bin:x:2:2:bin:/bin:/usr/sbin/nologin
// sys:x:3:3:sys:/dev:/usr/sbin/nologin
// sync:x:4:65534:sync:/bin:/bin/sync
//
// We must now compare the integer after "x" to the desired LinuxParser::Uid of the process to identify which user launched the process
string LinuxParser::User(int pid) {
  string line, user, x, uid;
  std::ifstream stream(kPasswordPath);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> user >> x >> uid) {
        if (uid == LinuxParser::Uid(pid)) {
          return user;
        }
      }
    }
  }
  return "UNIDENTIFIED"; 
}

// TODO: Read and return the uptime of a process
// Get it by
//
// #  cat /proc/1/stat
//
// This will return something like
// 1 (sh) S 0 1 1 0 -1 4194560 809 69412 16 181 2 3 74 38 20 0 1 0 4072 4612096 181 18446744073709551615 94234772529152 94234772673052 140720401885216 0 0 0 0 0 65538 1 0 0 17 0 0 0 8 0 0 94234774773608 94234774778400 94234805583872 140720401886854 140720401886897 140720401886897 140720401887216 0
// The uptime of a process is "uptime of system -  the time the process started after system boot".
// In kernels before Linux 2.6, this value was expressed in jiffies.  Since Linux 2.6, the value is expressed in clock ticks (divide by sysconf(_SC_CLK_TCK)).
// See also https://man7.org/linux/man-pages/man5/proc.5.html and then /proc/[pid]/stat
long LinuxParser::UpTime(int pid) {
  string line, value;
  vector<string> values;
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    while (linestream >> value) {
      values.push_back(value);
    };
  }
  return LinuxParser::UpTime() - (std::stol(values[21]) / 100);
}
