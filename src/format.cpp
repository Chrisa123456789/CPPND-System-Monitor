#include <string>

#include "format.h"

using std::string;


string Format::FillZeros (string time) {
  while (time.length()<2) {
    time = "0" + time;
  }
  return time;
} 
 
    

string Format::ElapsedTime(long input_seconds) {
  
  long hours, minutes, seconds, remaining_after_hours, remaining_after_minutes;
  
  hours = input_seconds / 3600;
  remaining_after_hours = input_seconds % 3600;
  minutes = remaining_after_hours / 60;
  remaining_after_minutes = remaining_after_hours % 60;
  seconds = remaining_after_minutes;
  
  string formatted = FillZeros(std::to_string(hours)) + ":" + FillZeros(std::to_string(minutes)) + ":" + FillZeros(std::to_string(seconds)); 
  
  return  formatted;


}