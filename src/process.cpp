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

// TODO
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

string Process::Ram() {
  std::ifstream filestream(LinuxParser::kProcDirectory + std::to_string(pid_) +
                           LinuxParser::kStatusFilename);
  std::string line;
  std::smatch match;
  std::regex rule("VmSize:\\s+([0-9]+)");
  float memoryInMb;

  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      if (regex_search(line, match, rule)) {
        // ignore first match as it's full string
        memoryInMb = std::stof(match[1]) / 1000;
        break;
      }
    }
  }

  std::stringstream stream;
  stream << std::fixed << std::setprecision(2) << memoryInMb;

  return stream.str() + "MB";
}

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

long int Process::UpTime() {
  std::ifstream filestream(LinuxParser::kProcDirectory + std::to_string(pid_) +
                           LinuxParser::kStatFilename);
  std::string line;
  std::vector<std::string> vstrings;
  long int uptimeInSeconds;

  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::stringstream ss(line);
      std::istream_iterator<std::string> begin(ss);
      std::istream_iterator<std::string> end;
      vstrings = std::vector<std::string>(begin, end);
      break;
    }
  }

  // convert clock ticks to seconds
  // take #22 starttime %llu
  // http://man7.org/linux/man-pages/man5/proc.5.html
  uptimeInSeconds = std::stol(vstrings[21]) / sysconf(_SC_CLK_TCK);

  return uptimeInSeconds;
}

// TODO: Overload the "less than" comparison operator for Process objects
// REMOVE: [[maybe_unused]] once you define the function
bool Process::operator<(Process const& a [[maybe_unused]]) const {
  return true;
}