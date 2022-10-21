#include <string>

#include "format.h"

using std::string;

// TODO: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
string Format::ElapsedTime(long seconds) { 
    int HH = seconds / 3600;
    seconds = seconds - (HH * 3600);
    int MM = seconds / 60;
    int SS = seconds - (MM * 60);
    string HOURS{"00"}, MINS{"00"}, SECONDS{"00"}; 
    if(HH < 10) {HOURS = "0" + std::to_string(HH);} else {HOURS = std::to_string(HH);}
    if(MM < 10) {MINS = "0" + std::to_string(MM);} else {MINS = std::to_string(MM);}
    if(SS < 10) {SECONDS = "0" + std::to_string(SS);} else {SECONDS = std::to_string(SS);}
    return HOURS + ":" + MINS + ":" + SECONDS; 
}