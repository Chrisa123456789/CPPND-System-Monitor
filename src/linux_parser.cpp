#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

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

//Read and return the system memory utilization 
float LinuxParser::MemoryUtilization() { 
  
  string line;
  string memory_type;
  string memory_value;
  float mem_total;
  float mem_free;
  float buffers;
  
  std::ifstream filestream(kProcDirectory + kMeminfoFilename);
   if (filestream.is_open()) {
     while (std::getline(filestream, line)) {
       std::istringstream linestream(line);
       while (linestream >> memory_type >> memory_value) {
         
       	if (memory_type == "MemTotal:")
        {
           mem_total = stof(memory_value);
        }
            
        else if (memory_type == "MemFree:")
        {
            mem_free = stof(memory_value);
        }
        else if (memory_type== "Buffers:")
        {
              buffers = stof(memory_value);
        }
       }
     }
   }
  
  return 1 - (mem_free/(mem_total - buffers)); 
}

//Read and return the system uptime 
long LinuxParser::UpTime() { 
  string up_time, idle;
  string line;
  long time_return;
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> up_time >> idle;
    time_return=std::stol(up_time);
  } 
 
  return time_return;
  
}

// Read and return the number of jiffies for the system
long LinuxParser::Jiffies() {
  return LinuxParser::ActiveJiffies() + LinuxParser::IdleJiffies();
  }

//Read and return the number of active jiffies for a PID
long LinuxParser::ActiveJiffies(int pid) {
    vector<string> pids_data = CpuUtilization(pid);
  	return std::stol(pids_data[13]) + std::stol(pids_data[14]) + 
    std::stol(pids_data[15]) + std::stol(pids_data[16]);
  
}
   

//Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() { 
  long active_jiffies;
  vector<string> util;
  vector<CPUStates> active_states{kUser_,kNice_,kSystem_,kIRQ_,kSoftIRQ_,kSteal_};
  
  util = LinuxParser::CpuUtilization();
  for (auto state: active_states) {
   
  active_jiffies += stol(util[state]);
  }  
  return active_jiffies;
  }

//Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() {
  long idle_jiffies;
  vector<string> util;
  vector<CPUStates> idle_states{kIdle_,kIOwait_};
  
  util = LinuxParser::CpuUtilization();
  for (auto state: idle_states) {
   
  idle_jiffies += stol(util[state]);
  }  
  return idle_jiffies;
  
}

//Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() { 
  string line, jiffies;
  vector<string> cpu_usage;
  
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    while (linestream >> jiffies) {
  	 if (jiffies != "cpu") {
       cpu_usage.push_back(jiffies);
      }
    }
  }
    
    
  return cpu_usage;
}

//Read and return the total number of processes 
int LinuxParser::TotalProcesses() { 

  string line, key, value;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
     while (std::getline(stream, line)) {
       std::istringstream linestream(line);
       linestream >> key >> value;
       if (key == "processes") {
         return std::stoi(value);
         }
     }
  }
  
  return 0;
}

//Read and return the number of running processes 
int LinuxParser::RunningProcesses() {
  string line, key, value;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
     while (std::getline(stream, line)) {
       std::istringstream linestream(line);
       linestream >> key >> value;
       if (key == "procs_running") {
         return std::stoi(value);
         }
     }
  }
   
  return 0;
}

//Read and return the command associated with a process
string LinuxParser::Command(int pid) {
  string line;
  
  std::ifstream filestream(kProcDirectory+to_string(pid)+kCmdlineFilename);
  if(filestream.is_open()){
    std::getline(filestream,line);
  }
  
  return line;  
}


//Read and return the memory used by a process
string LinuxParser::Ram(int pid) { 
  std::ifstream filestream(kProcDirectory + std::to_string(pid) + kStatusFilename);
  string line, key, value;
  long kb_ram, mb_ram;
  
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> value;
      if (key == "VmSize:") {
        kb_ram = stol(value);
        mb_ram = kb_ram / 1000;
        
      }
    }
  }
 return std::to_string(mb_ram);
} 



//Read and return the user ID associated with a process
string LinuxParser::Uid(int pid) { 
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatusFilename);
  string uid,line, key, value;
  if (filestream.is_open()) {
    while(getline(filestream, line)) {
      std::istringstream filestream(line);
      filestream >> key >> value;
      if (key == "Uid:") {
        uid = value;
      }
    }
  }
  return uid;
} 



//Read and return the user associated with a process
string LinuxParser::User(int pid) { 
  std::ifstream fileStream(kPasswordPath);
  string line, username, x, user_id, found_name;
  
  if (fileStream.is_open()){
    while(std::getline(fileStream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream lineStream(line);
      lineStream >> username >> x >> user_id; 
      if (user_id == LinuxParser::Uid(pid)) {
        found_name = username;
      }
    }
  }
  return found_name;
} 




//Read and return the uptime of a process
long LinuxParser::UpTime(int pid) { 
 vector<string> pids_data = CpuUtilization(pid);

 return (std::stol(pids_data[21]) / sysconf(_SC_CLK_TCK)) ;
}
  


vector<string> LinuxParser::CpuUtilization(int pid) {
  std::ifstream filestream(kProcDirectory + std::to_string(pid) + kStatFilename);
  string line, token;
  vector<string> tokens;
  	if (filestream.is_open()) {
      getline(filestream, line);
      std::istringstream filestream(line);
      
      
      while(filestream >> token) {
      	tokens.push_back(token);
      }
      return tokens;
    }
  	return {};
} 
