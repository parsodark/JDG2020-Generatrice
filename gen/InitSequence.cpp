#include "InitSequence.hpp"
#include <Arduino.h>
#include "Button.hpp"
#include "ButtonHelper.hpp"

InitSequence::InitSequence(uint32_t tPeriod):
  mPeriod(tPeriod),
  mStartTime(0),
  mState(InitSequenceState::FlashingButtons)
{ 
}

InitSequenceResult InitSequence::run()
{
  if (0 == mStartTime)
  {
    mStartTime = millis();
  }

  switch(mState)
  {
  case InitSequenceState::FlashingButtons:
  {
    uint32_t currentTime = millis();
    uint32_t elapsedTime = currentTime - mStartTime;
  
    uint8_t indexInSequence = elapsedTime / mPeriod;
  
    switch(indexInSequence)
    {
    case 0:
      illuminateOneButton(Button::White);
      break;
    case 1:
      illuminateOneButton(Button::Green);
      break;
    case 2:
      illuminateOneButton(Button::Yellow);
      break;
    case 3:
      illuminateOneButton(Button::Red);
      break;
    case 4:
    case 5:
      illuminateNoButton();
      break;
    case 6:
      illuminateOneButton(Button::Red);
      break;
    case 7:
      illuminateOneButton(Button::Yellow);
      break;
    case 8:
      illuminateOneButton(Button::Green);
      break;
    case 9:
      illuminateOneButton(Button::White);
      break;
    case 10:
    case 11:
      illuminateNoButton();
      break;
    default:
      illuminateNoButton();
      mState = InitSequenceState::WaitingForWhite;
      break;
    }
    break;
  }
  case InitSequenceState::WaitingForWhite:
    illuminateNoButton();
    if (Button::White == getPressedButton())
    {
      mState = InitSequenceState::WaitingForGreen;
    }
    break;
  case InitSequenceState::WaitingForGreen:
    illuminateOneButton(Button::White);
    if (Button::Green == getPressedButton())
    {
      mState = InitSequenceState::WaitingForYellow;
    }
    break;
  case InitSequenceState::WaitingForYellow:
    setButtonIllumination(Button::White, true);
    setButtonIllumination(Button::Green, true);
    setButtonIllumination(Button::Yellow, false);
    setButtonIllumination(Button::Red, false);
    if (Button::Yellow == getPressedButton())
    {
      mState = InitSequenceState::WaitingForRed;
    }
    break;
  case InitSequenceState::WaitingForRed:
    setButtonIllumination(Button::White, true);
    setButtonIllumination(Button::Green, true);
    setButtonIllumination(Button::Yellow, true);
    setButtonIllumination(Button::Red, false);
    if (Button::Red == getPressedButton())
    {
      mState = InitSequenceState::Done;
      return InitSequenceResult::Done;
    }
    break;
  case InitSequenceState::Done:
  default:
    illuminateAllButtons();
    return InitSequenceResult::Done;
  }
  
  return InitSequenceResult::NotDone;
}
