#pragma once

#include <stdint.h>

enum class InitSequenceResult
{
  NotDone,
  Done,
};

class InitSequence
{
public:
  InitSequence(uint32_t tPeriod);
  InitSequenceResult run();
private:
  uint32_t mPeriod;
  uint32_t mStartTime;

  enum class InitSequenceState
  {
    FlashingButtons,
    WaitingForWhite,
    WaitingForGreen,
    WaitingForYellow,
    WaitingForRed,
    Done,
  };
  InitSequenceState mState;
  
};
