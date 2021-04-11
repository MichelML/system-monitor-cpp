#include "process.h"

#include <linux_parser.h>
#include <unistd.h>

#include <cctype>
#include <sstream>
#include <string>
#include <vector>

using std::string;
using std::to_string;
using std::vector;

Process::Process(int pid) { pid_ = pid; }

int Process::Pid() { return pid_; }

float Process::CpuUtilization() { return 0; }

string Process::Command() {
  string line;
  std::ifstream filestream(LinuxParser::kProcDirectory + std::to_string(pid_) +
                           LinuxParser::kCmdlineFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      return line;
    }
  }
}

// TODO: Return this process's memory utilization
string Process::Ram() { return string(); }

// TODO: Return the user (name) that generated this process
string Process::User() { return string(); }

// TODO: Return the age of this process (in seconds)
long int Process::UpTime() { return 0; }

// TODO: Overload the "less than" comparison operator for Process objects
// REMOVE: [[maybe_unused]] once you define the function
bool Process::operator<(Process const& a [[maybe_unused]]) const {
  return true;
}