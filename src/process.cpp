#include "process.h"

#include <linux_parser.h>
#include <unistd.h>

#include <cctype>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using std::string;
using std::to_string;
using std::vector;

Process::Process(int pid) { pid_ = pid; }

int Process::Pid() { return pid_; }

float Process::CpuUtilization() { return LinuxParser::CpuUtilization(pid_); }

string Process::Command() { return LinuxParser::Command(pid_); }

string Process::Ram() { return LinuxParser::Ram(pid_); }

string Process::User() { return LinuxParser::User(pid_); }

long int Process::UpTime() { return LinuxParser::UpTime(pid_); }

bool Process::operator<(Process const& a) const {
  long currentRam = LinuxParser::CpuUtilization(pid_);
  long otherRam = LinuxParser::CpuUtilization(a.pid_);
  return currentRam < otherRam;
}