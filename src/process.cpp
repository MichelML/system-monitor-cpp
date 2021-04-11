#include "process.h"

#include <linux_parser.h>
#include <unistd.h>

#include <cctype>
#include <iostream>
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
string Process::User() {
  std::ifstream filestream(LinuxParser::kProcDirectory + std::to_string(pid_) +
                           LinuxParser::kStatusFilename);
  std::ifstream passwdstream(LinuxParser::kPasswordPath);
  std::string line;
  std::smatch match;
  std::regex rule("Uid:\\s+([0-9]+)");
  int uid;
  std::string username;

  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      if (regex_search(line, match, rule)) {
        // ignore first match as it's full string
        uid = std::stoi(match[1]);
        break;
      }
    }
  }

  std::regex usernameLine("([^:]+):[^:]+:" + to_string(uid) + ":");
  if (passwdstream.is_open()) {
    while (std::getline(passwdstream, line)) {
      if (regex_search(line, match, usernameLine)) {
        // ignore first match as it's full string
        username = match[1];
        break;
      }
    }
  }
  return username;
}

// TODO: Return the age of this process (in seconds)
long int Process::UpTime() { return 0; }

// TODO: Overload the "less than" comparison operator for Process objects
// REMOVE: [[maybe_unused]] once you define the function
bool Process::operator<(Process const& a [[maybe_unused]]) const {
  return true;
}