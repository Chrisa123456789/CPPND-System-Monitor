#include "processor.h"
#include "linux_parser.h"


//Return the aggregate CPU utilization 
float Processor::Utilization() { 
  float aggregate, total, active, delta_active, delta_total;
  
  total = LinuxParser::Jiffies();
  active = LinuxParser::ActiveJiffies();
  
  
 
  delta_active = active - prev_active;
  delta_total = total - prev_total;
  aggregate = delta_active / delta_total;
  
  prev_total = total;
  prev_active = active;
 
  if (std::isnan(aggregate)) {aggregate = 0.0;}
  return aggregate;
  
}