#include <dirent.h>
#include <unistd.h>
#include <sstream>
#include <string>
#include <vector>
#include <iostream>

#include "linux_parser.h"

using std::stof;
using std::stol;
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
  string os, kernel, version;
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

// TODO: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() {
  long MemTotal{0};
  long MemFree{0};
  long MemAvailable{0};
  long Buffers{0};

  string line, tmp;
  std::ifstream meminfoStream(kProcDirectory + kMeminfoFilename);

  if(meminfoStream.is_open())
  {
    std::getline(meminfoStream, line);
    std::istringstream linestream1(line);
    linestream1 >> tmp >> MemTotal;

    std::getline(meminfoStream, line);
    std::istringstream linestream2(line);
    linestream2 >> tmp >> MemFree;

    std::getline(meminfoStream, line);
    std::istringstream linestream3(line);
    linestream3 >> tmp >> MemAvailable;

    std::getline(meminfoStream, line);
    std::istringstream linestream4(line);
    linestream4 >> tmp >> Buffers;

    return ((float)MemTotal - (float)MemFree)/(float)MemTotal;
  }

  return 0.0; 
}

// TODO: Read and return the system uptime
long LinuxParser::UpTime() { 
  long uptime{0};

  string line;
  std::ifstream uptimeStream(kProcDirectory + kUptimeFilename);

  if (uptimeStream.is_open()){
    std::getline(uptimeStream, line);
    std::istringstream linestream(line);
    linestream >> uptime;
  }
  return uptime; 
}

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() {
  return ActiveJiffies() + IdleJiffies(); 
}

// Parse process file
vector<string> LinuxParser::ParseProcess(int pid) {
  string line, value;
  vector<string> process_stats;
  int process_stat_no = 21;
  std::ifstream statStream(kProcDirectory + to_string(pid) + kStatFilename);

  if (statStream.is_open()){
    std::getline(statStream, line);
    std::istringstream linestream(line);

    for(int i = 0; i <= process_stat_no; i++)
    {
      linestream >> value;
      process_stats.emplace_back(value);
    }
  }
  return process_stats;
}

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid) {
  vector<string> process_stats = ParseProcess(pid);
  return stol(process_stats[utime]) + stol(process_stats[stime]);
}

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() {
  vector<string> cpu_values = CpuUtilization();
  return stol(cpu_values[kUser_]) + stol(cpu_values[kNice_]) + stol(cpu_values[kSystem_])
        + stol(cpu_values[kIRQ_])+ stol(cpu_values[kSoftIRQ_]) + stol(cpu_values[kSteal_]);
}

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() {
  vector<string> cpu_values = CpuUtilization();
  return stol(cpu_values[kIdle_]) + stol(cpu_values[kIOwait_]);
}

// TODO: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() { 
  string line, key, value;
  vector<string> cpu_values;
  std::ifstream statStream(kProcDirectory + kStatFilename);

  if (statStream.is_open()){
    std::getline(statStream, line);
    std::istringstream linestream(line);
    linestream >> key;
    while (linestream >> value)
    {
      cpu_values.emplace_back(value);
    }
  }
  return cpu_values;
}

// TODO: Read and return the total number of processes
int LinuxParser::TotalProcesses() {
  string line, key, value;

  std::ifstream statStream(kProcDirectory + kStatFilename);

  if (statStream.is_open()){
    while(std::getline(statStream, line)){
      std::istringstream linestream(line);
      linestream >> key;
      if(key == "processes"){
        linestream >> value;
        return std::stoi(value);
      }
    }
  }
  return 0;
}

// TODO: Read and return the number of running processes
int LinuxParser::RunningProcesses() {
  string line, key, value;

  std::ifstream statStream(kProcDirectory + kStatFilename);

  if (statStream.is_open()){
    while(std::getline(statStream, line)){
      std::istringstream linestream(line);
      linestream >> key;
      if(key == "procs_running"){
        linestream >> value;
        return std::stoi(value);
      }
    }
  }
  return 0;
}

// TODO: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid) {
  string cmd;
  std::ifstream CmdlineStream(kProcDirectory + to_string(pid) + kCmdlineFilename);
  if (CmdlineStream.is_open()){
    std::getline(CmdlineStream, cmd);
    return cmd;
  }
  return "Not Found";
}

// TODO: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid) {
  string line, key, value, unit;
  std::ifstream procstatStream(kProcDirectory + to_string(pid) + kStatusFilename);
  if (procstatStream.is_open()){
    while(std::getline(procstatStream, line)){
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      linestream >> key;
      if(key == "VmSize"){
        linestream >> value >> unit;
        return value + " " + unit;
      }
    }
  }
  return "Not Found";
}

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid){
  string line, key, value;
  std::ifstream procstatStream(kProcDirectory + to_string(pid) + kStatusFilename);
  if (procstatStream.is_open()){
    while(std::getline(procstatStream, line)){
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      linestream >> key;
      if(key == "Uid"){
        linestream >> value;
        return value;
      }
    }
  }
  return "Not Found";
}


// TODO: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid) {
  string userID = Uid(pid);
  string line, user, str, id;
  std::ifstream passwdStream(kPasswordPath);
  if (passwdStream.is_open()){
    while(std::getline(passwdStream, line)){
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      linestream >> user >> str >> id;
      if(userID == id){
        return user;
      }
    }
  }
  return "Not Found";
}

// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid) {
  vector<string> process_stats = ParseProcess(pid);
  return stol(process_stats[starttime]);
}
