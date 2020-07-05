#ifndef PROCESSOR_H
#define PROCESSOR_H

class Processor {
 public:
  float Utilization();  
  float prev_active{0}, prev_idle{0}, prev_total{0};
  
 private:
  
};

#endif