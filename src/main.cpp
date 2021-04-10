#include "iostream"
#include "ncurses_display.h"
#include "system.h"

int main() {
  System system;
  // NCursesDisplay::Display(system);
  std::cout << system.MemoryUtilization() << std::endl;
}