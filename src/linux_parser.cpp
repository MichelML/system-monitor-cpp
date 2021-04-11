#include "linux_parser.h"

#include <dirent.h>
#include <unistd.h>

#include <string>
#include <vector>

#include "iostream"

using std::stof;
using std::string;
using std::to_string;
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

float LinuxParser::MemoryUtilization() {
  string line;
  std::ifstream filestream(kProcDirectory + kMeminfoFilename);
  float total_mem = -1;
  float free_mem = -1;

  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      if (line.find("MemTotal:") != std::string::npos) {
        string total_mem_str = std::regex_replace(line, nonDigitsRegexp, "");
        total_mem = std::stof(total_mem_str);
      } else if (line.find("MemFree:") != std::string::npos) {
        string free_mem_str = std::regex_replace(line, nonDigitsRegexp, "");
        free_mem = std::stof(free_mem_str);
      }

      if (total_mem != -1 && free_mem != -1) {
        break;
      }
    }
  }

  // equation based on https://stackoverflow.com/a/41251290
  return (total_mem - free_mem) / total_mem;
}

long LinuxParser::UpTime() {
  string line;
  std::ifstream filestream(kProcDirectory + kUptimeFilename);
  long uptime = -1;

  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      string uptime_str = line.substr(0, line.find(" "));
      uptime = std::stol(uptime_str);
      if (uptime != -1) {
        break;
      }
    }
  }

  return uptime;
}

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() { return 0; }

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid [[maybe_unused]]) { return 0; }

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() { return 0; }

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() { return 0; }

// TODO: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() { return {}; }

int LinuxParser::TotalProcesses() {
  string line;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  int total_processes = -1;

  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      if (line.find("processes ") != std::string::npos) {
        string total_processes_str =
            std::regex_replace(line, nonDigitsRegexp, "");
        total_processes = std::stoi(total_processes_str);
      }

      if (total_processes != -1) {
        break;
      }
    }
  }
  return total_processes;
}

int LinuxParser::RunningProcesses() {
  string line;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  int running_processes = -1;

  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      if (line.find("procs_running ") != std::string::npos) {
        string running_processes_str =
            std::regex_replace(line, nonDigitsRegexp, "");
        running_processes = std::stoi(running_processes_str);
      }

      if (running_processes != -1) {
        break;
      }
    }
  }
  return running_processes;
}

// TODO: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid [[maybe_unused]]) { return string(); }

// TODO: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid [[maybe_unused]]) { return string(); }

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid [[maybe_unused]]) { return string(); }

// TODO: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid [[maybe_unused]]) { return string(); }

// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid [[maybe_unused]]) { return 0; }
