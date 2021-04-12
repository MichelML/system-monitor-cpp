#ifndef SYSTEM_PARSER_H
#define SYSTEM_PARSER_H

#include <fstream>
#include <regex>
#include <string>

namespace LinuxParser {
// Paths
const std::string kProcDirectory{"/proc/"};
const std::string kCmdlineFilename{"/cmdline"};
const std::string kCpuinfoFilename{"/cpuinfo"};
const std::string kStatusFilename{"/status"};
const std::string kStatFilename{"/stat"};
const std::string kUptimeFilename{"/uptime"};
const std::string kMeminfoFilename{"/meminfo"};
const std::string kVersionFilename{"/version"};
const std::string kOSPath{"/etc/os-release"};
const std::string kPasswordPath{"/etc/passwd"};
const auto nonDigitsRegexp = std::regex(R"([\D])");

// System
float MemoryUtilization();
long UpTime();
std::vector<int> Pids();
int TotalProcesses();
int RunningProcesses();
std::string OperatingSystem();
std::string Kernel();

// CPU
long int prevusertime = 0;
long int prevnicetime = 0;
long int prevsystemtime = 0;
long int previdletime = 0;
long int previoWait = 0;
long int previrq = 0;
long int prevsoftIrq = 0;
long int prevsteal = 0;
long int prevguest = 0;
long int prevguestnice = 0;

long int usertime = 0;
long int nicetime = 0;
long int systemtime = 0;
long int idletime = 0;
long int ioWait = 0;
long int irq = 0;
long int softIrq = 0;
long int steal = 0;
long int guest = 0;
long int guestnice = 0;

float CpuUtilization();
long Jiffies();
long ActiveJiffies();
long ActiveJiffies(int pid);
long IdleJiffies();

// Processes
std::string Command(int pid);
std::string Ram(int pid);
std::string Uid(int pid);
std::string User(int pid);
long int UpTime(int pid);
float CpuUtilization(int pid);
};  // namespace LinuxParser

#endif