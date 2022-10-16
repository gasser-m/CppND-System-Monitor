#include "processor.h"
#include "linux_parser.h"

// TODO: Return the aggregate CPU utilization
float Processor::Utilization() {
    long ActiveTime = LinuxParser::ActiveJiffies();
    long Total = LinuxParser::Jiffies();
    return (float)ActiveTime / (float)Total;
}