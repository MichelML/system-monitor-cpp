#ifndef PROCESSOR_H
#define PROCESSOR_H

class Processor {
 public:
  float Utilization();

 private:
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
};

#endif