#include "linux_parser.h"

#include <dirent.h>
#include <unistd.h>

#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

using std::stof;
using std::string;
using std::to_string;
using std::vector;

long int usertime;
long int nicetime;
long int systemtime;
long int idletime;
long int ioWait;
long int irq;
long int softIrq;
long int steal;
long int guest;

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

  filestream.close();

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

  stream.close();

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

  filestream.close();

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

  filestream.close();

  return uptime;
}

float LinuxParser::CpuUtilization() {
  std::ifstream filestream(LinuxParser::kProcDirectory +
                           LinuxParser::kStatFilename);
  std::string line;
  int lineN = 0;
  std::smatch match;
  std::regex rule(
      "cpu  "
      "([0-9]+)\\s([0-9]+)\\s([0-9]+)\\s([0-9]+)\\s([0-9]+)\\s([0-9]+)\\s([0-9]"
      "+)\\s([0-9]+)"
      "\\s([0-9]+)\\s([0-9]+)");

  long int prevusertime = (usertime) ? usertime : 0;
  long int prevnicetime = (nicetime) ? nicetime : 0;
  long int prevsystemtime = (systemtime) ? systemtime : 0;
  long int previdletime = (idletime) ? idletime : 0;
  long int previoWait = (ioWait) ? ioWait : 0;
  long int previrq = (irq) ? irq : 0;
  long int prevsoftIrq = (softIrq) ? softIrq : 0;
  long int prevsteal = (steal) ? steal : 0;

  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      if (regex_search(line, match, rule)) {
        // ignore first match as it's full string
        usertime = std::stol(match[1]);
        nicetime = std::stol(match[2]);
        systemtime = std::stol(match[3]);
        idletime = std::stol(match[4]);
        ioWait = std::stol(match[5]);
        irq = std::stol(match[6]);
        softIrq = std::stol(match[7]);
        steal = std::stol(match[8]);
      }
      if (lineN == 0) {
        break;
      }
    }
  }

  filestream.close();

  // from https://stackoverflow.com/a/23376195
  long int PrevIdle = previdletime + previoWait;
  long int Idle = idletime + ioWait;

  long int PrevNonIdle = prevusertime + prevnicetime + prevsystemtime +
                         previrq + prevsoftIrq + prevsteal;
  long int NonIdle = usertime + nicetime + systemtime + irq + softIrq + steal;

  long int PrevTotal = PrevIdle + PrevNonIdle;
  long int Total = Idle + NonIdle;

  // differentiate: actual value minus the previous one
  float totald = Total - PrevTotal;
  float idled = Idle - PrevIdle;

  float CPU_Percentage = (totald - idled) / totald;
  return CPU_Percentage;
}

float LinuxParser::CpuUtilization(int pid) {
  // see https://stackoverflow.com/a/16736599
  // for calculation explanation
  std::ifstream filestream(LinuxParser::kProcDirectory + std::to_string(pid) +
                           LinuxParser::kStatFilename);
  std::string line;
  std::vector<std::string> vstrings;
  long utime;
  long stime;
  long cutime;
  long cstime;
  long starttime;

  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::stringstream ss(line);
    std::istream_iterator<std::string> begin(ss);
    std::istream_iterator<std::string> end;
    vstrings = std::vector<std::string>(begin, end);
  }

  filestream.close();

  utime = std::stol(vstrings[13]);
  stime = std::stol(vstrings[14]);
  cutime = std::stol(vstrings[15]);
  cstime = std::stol(vstrings[16]);
  starttime = std::stol(vstrings[21]);

  long total_time = utime + stime + cutime + cstime;
  long seconds = LinuxParser::UpTime() - (starttime / sysconf(_SC_CLK_TCK));
  float cpu_usage = 100 * ((total_time / sysconf(_SC_CLK_TCK)) / seconds);

  return cpu_usage;
}

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

  filestream.close();

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

  filestream.close();

  return running_processes;
}

string LinuxParser::Command(int pid) {
  string line;
  std::ifstream filestream(LinuxParser::kProcDirectory + std::to_string(pid) +
                           LinuxParser::kCmdlineFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
  }

  filestream.close();

  return line;
}

string LinuxParser::Ram(int pid) {
  std::ifstream filestream(LinuxParser::kProcDirectory + std::to_string(pid) +
                           LinuxParser::kStatusFilename);
  std::string line;
  std::smatch match;
  std::regex rule("VmSize:\\s+([0-9]+)");
  float memoryInMb;

  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      if (regex_search(line, match, rule)) {
        // ignore first match as it's full string
        memoryInMb = std::stof(match[1]) / 1024;
        break;
      }
    }
  }

  filestream.close();

  std::stringstream stream;
  stream << std::fixed << std::setprecision(2) << memoryInMb;

  return stream.str();
}

string LinuxParser::Uid(int pid) {
  std::ifstream filestream(LinuxParser::kProcDirectory + std::to_string(pid) +
                           LinuxParser::kStatusFilename);
  std::string line;
  std::smatch match;
  std::regex rule("Uid:\\s+([0-9]+)");
  string uid;

  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      if (regex_search(line, match, rule)) {
        // ignore first match as it's full string
        uid = match[1];
        break;
      }
    }
  }

  filestream.close();

  return uid;
}

string LinuxParser::User(int pid) {
  std::ifstream passwdstream(LinuxParser::kPasswordPath);
  std::string line;
  std::smatch match;
  std::string username;
  string uid = LinuxParser::Uid(pid);
  std::regex usernameLine("([^:]+):[^:]+:" + uid + ":");

  if (passwdstream.is_open()) {
    while (std::getline(passwdstream, line)) {
      if (regex_search(line, match, usernameLine)) {
        // ignore first match as it's full string
        username = match[1];
        break;
      }
    }
  }

  passwdstream.close();

  return username;
}

long LinuxParser::UpTime(int pid) {
  std::ifstream filestream(LinuxParser::kProcDirectory + std::to_string(pid) +
                           LinuxParser::kStatFilename);
  std::string line;
  std::vector<std::string> vstrings;
  long starttime;
  long seconds;

  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::stringstream ss(line);
    std::istream_iterator<std::string> begin(ss);
    std::istream_iterator<std::string> end;
    vstrings = std::vector<std::string>(begin, end);
  }

  filestream.close();

  // convert clock ticks to seconds
  // take #22 starttime %llu
  // http://man7.org/linux/man-pages/man5/proc.5.html
  starttime = std::stol(vstrings[21]);
  seconds = starttime / sysconf(_SC_CLK_TCK);
  return seconds;
}