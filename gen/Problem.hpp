#pragma once

#include <stdint.h>
#include "IProblem.hpp"
#include "ButtonHelper.hpp"

template <uint8_t N>
class Problem : public IProblem
{
public:
	Problem(uint16_t tPeriod);

	void generateProblem() override;

	ShowSequenceResult showSequence() override;

  WaitForSolveResult waitForSolve() override;
  
  String getPrintableSequence() override;
  
private:
	uint16_t mPeriod;
	Button mSequence[N];
	uint32_t mShowSequenceStartTime;
  uint8_t mWaitForSolveSequenceIndex;
};

//Implementation
template <uint8_t N>
Problem<N>::Problem(uint16_t tPeriod):
	mPeriod(tPeriod),
	mSequence{},
	mShowSequenceStartTime(0),
  mWaitForSolveSequenceIndex(0)
{
}

template <uint8_t N>
void Problem<N>::generateProblem()
{
	Button lastButton = static_cast<Button>(255);

	for (uint16_t i = 0; i < N; ++i)
	{
		Button button;
		do
		{
			button = static_cast<Button>(random(4));
		} while (button == lastButton);

		mSequence[i] = button;

		lastButton = button;
	}

  mShowSequenceStartTime = 0;
  mWaitForSolveSequenceIndex = 0;
}

template <uint8_t N>
ShowSequenceResult Problem<N>::showSequence()
{
	if (0 == mShowSequenceStartTime)
	{
		mShowSequenceStartTime = millis();
	}

	uint32_t currentTime = millis();
	uint32_t elapsedTime = currentTime - mShowSequenceStartTime;

	uint8_t indexInSequence = elapsedTime / mPeriod;

	if (indexInSequence >= N)
	{
    illuminateNoButton();
		return ShowSequenceResult::Done;
	}
	else
	{
		Button button = static_cast<Button>(mSequence[indexInSequence]);

		illuminateOneButton(button);
	}
	return ShowSequenceResult::NotDone;
}


template <uint8_t N>
WaitForSolveResult Problem<N>::waitForSolve()
{
  // Check that this problem was not already solved
  if (mWaitForSolveSequenceIndex >= N)
  {
    return WaitForSolveResult::Success;
  }

  Button pressedButton = getPressedButton();

  // If no button is pressed, the problem is not solved.
  if (Button::None == pressedButton)
  {
    return WaitForSolveResult::NotDone;
  }
  // If multiple buttons are pressed, the problem is failed.
  else if (Button::Multiple == pressedButton)
  {
    return WaitForSolveResult::Failure;
  }
  // If the right button is pressed, the problem advances to the next button.
  else if (mSequence[mWaitForSolveSequenceIndex] == pressedButton)
  {
    ++mWaitForSolveSequenceIndex;
    
    // If the sequence is done, the problem is solved. Else, it is not solved.
    return (mWaitForSolveSequenceIndex) >= N ? WaitForSolveResult::Success :
                                               WaitForSolveResult::NotDone;
  }
  // If the previous right button is pressed, the problem is not solved (but not failed).
  else if (mWaitForSolveSequenceIndex > 0 &&
           mSequence[mWaitForSolveSequenceIndex - 1] == pressedButton)
  {
    return WaitForSolveResult::NotDone;
  }
  // Else, the wrong button was pressed. The problem is failed.
  else
  {
    return WaitForSolveResult::Failure;
  }
}

template <uint8_t N>
String Problem<N>::getPrintableSequence()
{
  String output = "";
  for (uint8_t i = 0; i < N; ++i)
  {
    switch(mSequence[i])
    {
      case(Button::Red):
        output += "R";
        break;
      case(Button::Yellow):
        output += "Y";
        break;
      case(Button::Green):
        output += "G";
        break;
      case(Button::White):
        output += "W";
        break;
    }
  }
  return output;
}
