#pragma once

#include <stdint.h>

enum class ShowSequenceResult
{
  NotDone,
  Done,
};

enum class WaitForSolveResult
{
  NotDone,
  Success,
  Failure,
};

class IProblem
{
public:
  virtual void generateProblem() = 0;

  virtual ShowSequenceResult showSequence() = 0;

  virtual WaitForSolveResult waitForSolve() = 0;

  virtual String getPrintableSequence() = 0;
};
