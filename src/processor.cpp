#include "processor.h"

#include <regex>
#include <string>
#include <vector>

#include "iostream"
#include "linux_parser.h"

float Processor::Utilization() { return LinuxParser::CpuUtilization(); }