#include "Time.hpp"

uint32_t convertMinutesAndSecondsToMilliseconds(uint32_t tMinutes, uint32_t tSeconds)
{
  return ((tMinutes * 60) + tSeconds) * 1000;
}

String convertMillisecondsToPrintableTime(uint32_t tMilliseconds)
{
  uint32_t minutes = tMilliseconds / 1000 / 60;
  uint32_t seconds = tMilliseconds / 1000 % 60;

  return String(minutes) + 
         ":" + 
         (seconds < 10 ? "0" : "") +
         String(seconds);
}
