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
      "([0-9]+)\\s([0-9]+)\\s([0-9]+)\\s([0-9]+)\\s([0-9]+)\\s([0-9]+)\\s([0-9]"
      "+)\\s([0-9]+)"
      "\\s([0-9]+)\\s([0-9]+)");

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
        guest = std::stol(match[9]);
        guestnice = std::stol(match[10]);
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

  // // differentiate: actual value minus the previous one
  float totald = Total - PrevTotal;
  float idled = Idle - PrevIdle;

  float CPU_Percentage = (totald - idled) / totald;
  return CPU_Percentage;
}