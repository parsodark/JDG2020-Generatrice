#pragma once

#include "LedControl.h"

class LedMatrixPrinter
{
public:
  LedMatrixPrinter(LedControl& tLedControl);
  void addChar(char tChar,
               uint8_t tPosition);
  void print();
  void clear();

private:
  static const uint8_t FONT[11][8];
  LedControl& mLedControl;
  uint32_t mMatrix[8];
};
