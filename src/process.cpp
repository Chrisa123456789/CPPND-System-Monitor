#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "process.h"
#include "linux_parser.h"
#include "system.h"
#include "processor.h"

using std::string;
using std::to_string;
using std::vector;

Process::Process(const int pid) : pid_(pid), cpu_utilization_(CpuUtilization()) {}

//Return this process's ID
int Process::Pid() { return pid_;}

//Return this process's CPU utilization
float Process::CpuUtilization() {
 unsigned long int active_jiffies = LinuxParser::ActiveJiffies(Pid());
 unsigned long int process_uptime = LinuxParser::UpTime(Pid());
 unsigned long int system_uptime = LinuxParser::UpTime();
    auto seconds = system_uptime - process_uptime;
    if(seconds <= 0)
    {return 0.0;}
  
 return (float) (1.0*(active_jiffies/sysconf(_SC_CLK_TCK)) / seconds);  


}

//Return the command that generated this process
string Process::Command() { return LinuxParser::Command(Pid()); }

//Return this process's memory utilization
string Process::Ram() {return LinuxParser::Ram(Pid());} 

//Return the user (name) that generated this process
string Process::User() {return LinuxParser::User(Pid()); }

//Return the age of this process (in seconds)
long int Process::UpTime() { return LinuxParser::UpTime(Pid()); }

//Overload the "less than" comparison operator for Process objects

bool Process::operator<(Process const& a) const {
	return cpu_utilization_>a.cpu_utilization_;
} 
