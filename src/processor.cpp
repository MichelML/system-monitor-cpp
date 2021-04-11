#include "processor.h"

#include <regex>
#include <string>
#include <vector>

#include "iostream"
#include "linux_parser.h"

float Processor::Utilization() {
  std::ifstream filestream(LinuxParser::kProcDirectory +
                           LinuxParser::kStatFilename);
  std::string line;
  int lineN = 0;
  std::smatch match;
  std::regex rule(
      "cpu  "
      "(\\d+)\\s(\\d+)\\s(\\d+)\\s(\\d+)\\s(\\d+)\\s(\\d+)\\s(\\d+)\\s(\\d+)"
      "\\s(\\d+)\\s(\\d+)");

  prevusertime = usertime;
  prevnicetime = nicetime;
  prevsystemtime = systemtime;
  previdletime = idletime;
  previoWait = ioWait;
  previrq = irq;
  prevsoftIrq = softIrq;
  prevsteal = steal;
  prevguest = guest;

  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      if (regex_match(line, match, rule)) {
        usertime = std::stol(match[0]);
        nicetime = std::stol(match[1]);
        systemtime = std::stol(match[2]);
        idletime = std::stol(match[3]);
        ioWait = std::stol(match[4]);
        irq = std::stol(match[5]);
        softIrq = std::stol(match[6]);
        steal = std::stol(match[7]);
        guest = std::stol(match[8]);
        guestnice = std::stol(match[9]);
      }
      if (lineN == 0) {
        break;
      }
    }
  }

  // from https://stackoverflow.com/a/23376195
  long int PrevIdle = previdletime + previoWait;
  long int Idle = idletime + ioWait;

  long int PrevNonIdle = prevusertime + prevnicetime + prevsystemtime +
                         previrq + prevsoftIrq + prevsteal;
  long int NonIdle = usertime + nicetime + systemtime + irq + softIrq + steal;

  long int PrevTotal = PrevIdle + PrevNonIdle;
  long int Total = Idle + NonIdle;

  // differentiate: actual value minus the previous one
  long int totald = Total - PrevTotal;
  long int idled = Idle - PrevIdle;

  long int CPU_Percentage = (totald - idled) / totald;
  return CPU_Percentage;
}