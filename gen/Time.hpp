#pragma once

#include <stdint.h>
#include <Arduino.h>

uint32_t convertMinutesAndSecondsToMilliseconds(uint32_t tMinutes, uint32_t tSeconds);

String convertMillisecondsToPrintableTime(uint32_t tMilliseconds);
