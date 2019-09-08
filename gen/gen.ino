#include "Defines.hpp"
#include "Button.hpp"
#include "PinHelper.hpp"
#include "InitSequence.hpp"
#include "Problem.hpp"
#include "Time.hpp"
#include "LedControl.h"
#include "LedMatrixPrinter.hpp"

LedControl ledControl = LedControl(LED_MATRIX_DIN_PIN,
                                   LED_MATRIX_CLK_PIN,
                                   LED_MATRIX_CS_PIN,
                                   LED_MATRIX_NUMBER_OF_DISPLAYS);
LedMatrixPrinter ledMatrixPrinter(ledControl);

struct ProblemEntry
{
  IProblem* problem;
  uint32_t startTime;
  uint32_t endTime;

  ProblemEntry(IProblem* tProblem,
               uint32_t tStartTime,
               uint32_t tEndTime):
    problem(tProblem),
    startTime(tStartTime),
    endTime(tEndTime) 
    {}
};

InitSequence initSequence(100);
Problem<6> p1(450);
Problem<10> p2(300);
Problem<15> p3(150);

ProblemEntry problems [] = 
{
  {&p1,convertMinutesAndSecondsToMilliseconds(5,0),convertMinutesAndSecondsToMilliseconds(6,0)},
  {&p2,convertMinutesAndSecondsToMilliseconds(8,0),convertMinutesAndSecondsToMilliseconds(9,0)},
  {&p3,convertMinutesAndSecondsToMilliseconds(10,0),convertMinutesAndSecondsToMilliseconds(11,0)},
};

const uint8_t NUMBER_OF_PROBLEMS = sizeof(problems) / sizeof(problems[0]);

const uint32_t GRACE_PERIOD_MS = convertMinutesAndSecondsToMilliseconds(1,0);
const uint32_t END_TIME_SUCCESS = convertMinutesAndSecondsToMilliseconds(13,0);

bool initSequenceDone = false;
uint8_t currentProblemIndex = 0;
bool currentProblemSequenceShown = false;
bool shouldDoubleFlash = false;
uint32_t doubleFlashStartTime = 0;
bool failed = false;
uint32_t endTime = 0;

uint32_t uartAndLedMatrixUpdateTime = 0;

enum class Status
{
  InitSequence,
  WaitingForProblem,
  ShowingSequence,
  SolvingSequence,
  Failed,
  Success,
};
Status status = Status::InitSequence;

void setup()
{
	Serial.begin(115200);

	pinMode(RED_LED_PIN, OUTPUT);
	pinMode(YELLOW_LED_PIN, OUTPUT);
	pinMode(GREEN_LED_PIN, OUTPUT);
	pinMode(WHITE_LED_PIN, OUTPUT);

	pinMode(RED_SWITCH_PIN, INPUT_PULLUP);
	pinMode(YELLOW_SWITCH_PIN, INPUT_PULLUP);
	pinMode(GREEN_SWITCH_PIN, INPUT_PULLUP);
	pinMode(WHITE_SWITCH_PIN, INPUT_PULLUP);

  for (uint8_t i = 0; i < LED_MATRIX_NUMBER_OF_DISPLAYS; ++i)
  {
    // Wake up displays
    ledControl.shutdown(i,false);
    // Set intensity levels
    ledControl.setIntensity(i,LED_MATRIX_INTENSITY);
    // Clear Displays
    ledControl.clearDisplay(i);
  }
}

uint32_t initDoneTimestamp = 0;
void loop()
{
  uint32_t currentTime = 0;

  // Whether buttons pressed by the user should be illuminated
  bool userControl = true;

  if (false == initSequenceDone)
  {
    // Currently doing init sequence
    if (InitSequenceResult::Done == initSequence.run())
    {
      initSequenceDone = true;
      initDoneTimestamp = millis();
      
      // Reset uart and Led Matrix timing
      uartAndLedMatrixUpdateTime = 0;

      randomSeed(micros());

      for (uint8_t i = 0; i < NUMBER_OF_PROBLEMS; ++i)
      {
        problems[i].problem->generateProblem();
      }
    }
    userControl = false;
    status = Status::InitSequence;
  }
  else
  {
    //Init sequence done
    currentTime = millis() - initDoneTimestamp;

    // Check for failure
    if (true == failed)
    {
      userControl = false;
      status = Status::Failed;
      if (currentTime > endTime)
      {
        // End of run
        illuminateAllButtons();
        for(;;);
      }
      else
      {
        illuminateNoButton();
      }
    }

    // Check if all problems are completed
    else if (NUMBER_OF_PROBLEMS <= currentProblemIndex)
    {
      status = Status::Success;
      endTime = END_TIME_SUCCESS;
      if (END_TIME_SUCCESS < currentTime)
      {
        // End of run
        illuminateAllButtons();
        for(;;);
      }
      else
      {
        illuminateNoButton();
      }
    }
    else
    {
      // One or more problems remaining
      ProblemEntry& currentProblemEntry = problems[currentProblemIndex];
  
      if (currentProblemEntry.startTime < currentTime)
      {
        // Problem started
        // Check for timeout
        if (currentProblemEntry.endTime <= currentTime)
        {
          failed = true;
          endTime = currentProblemEntry.endTime + GRACE_PERIOD_MS;
        }
  
        // Check if sequence must be shown
        if (false == currentProblemSequenceShown)
        {
          // Sequence has not finished showing
          ShowSequenceResult result = currentProblemEntry.problem->showSequence();
          if (ShowSequenceResult::Done == result)
          {
            currentProblemSequenceShown = true;
          }
          userControl = false;
          status = Status::ShowingSequence;
        }
        else if (false == shouldDoubleFlash)
        {
          // Sequence shown, time to solve the problem
          WaitForSolveResult result = currentProblemEntry.problem->waitForSolve();
          if (WaitForSolveResult::Success == result)
          {
            ++currentProblemIndex;
            currentProblemSequenceShown = false;
          }
          else if (WaitForSolveResult::Failure == result)
          {
            shouldDoubleFlash = true;
            doubleFlashStartTime = currentTime;
          }
          status = Status::SolvingSequence;
        }
        else // true == shouldDoubleFlash
        {
          // After a problem is failed, we show a double flash and a new 
          // problem is shown (as long as there is time remaining).
          uint32_t timeInDoubleFlash = currentTime - doubleFlashStartTime;
          uint32_t indexInDoubleFlash = timeInDoubleFlash / 100;
          switch(indexInDoubleFlash)
          {
          case 0:
          case 2:
            illuminateAllButtons();
            break;
          case 1:
          case 3:
          case 4:
          case 5:
          case 6:
          case 7:
            illuminateNoButton();
            break;
          default:
            // Stop double flash
            shouldDoubleFlash = false;
            doubleFlashStartTime = 0;
            // Reshow new sequence
            currentProblemEntry.problem->generateProblem();
            currentProblemSequenceShown = false;
            break;
          }
          userControl = false;
        }
      }
      else
      {
        // No active problem
        status = Status::WaitingForProblem;
      }
    }
  }

  // When user has control, any pressed button will be illuminated
  if (true == userControl)
  {
    illuminatePressedButton();  
  }

  // Update Uart and Led matrix
  if ( (false == initSequenceDone && millis() > uartAndLedMatrixUpdateTime) ||
       (true == initSequenceDone && currentTime > uartAndLedMatrixUpdateTime) )
  {
    uint32_t timeUntilNextProblem = 0;
    uint32_t timeUntilEndOfProblem = 0;
    uint32_t timeUntilEnd = 0;

    uint32_t timeToPrintToLedMatrix = 0;

    if (NUMBER_OF_PROBLEMS >= currentProblemIndex)
    {
      timeUntilNextProblem = problems[currentProblemIndex].startTime - currentTime;
      timeUntilEndOfProblem = problems[currentProblemIndex].endTime - currentTime;
    }
    timeUntilEnd = endTime - currentTime;
    
    switch(status)
    {
      case Status::InitSequence:
        Serial.print(convertMillisecondsToPrintableTime(timeUntilNextProblem));
        Serial.println(" Init sequence");
        timeToPrintToLedMatrix = timeUntilNextProblem;
        break;
      case Status::WaitingForProblem:
        Serial.print(convertMillisecondsToPrintableTime(timeUntilNextProblem));
        Serial.print(" Waiting for problem ");
        Serial.println(currentProblemIndex);
        timeToPrintToLedMatrix = timeUntilNextProblem;
        break;
      case Status::ShowingSequence:
        Serial.print(convertMillisecondsToPrintableTime(timeUntilEndOfProblem));
        Serial.print(" Showing sequence ");
        Serial.print(currentProblemIndex);
        Serial.print(" ");
        Serial.println(problems[currentProblemIndex].problem->getPrintableSequence());
        timeToPrintToLedMatrix = timeUntilEndOfProblem;
        break;
      case Status::SolvingSequence:
        Serial.print(convertMillisecondsToPrintableTime(timeUntilEndOfProblem));
        Serial.print(" Solving sequence ");
        Serial.print(currentProblemIndex);
        Serial.print(" ");
        Serial.println(problems[currentProblemIndex].problem->getPrintableSequence());
        timeToPrintToLedMatrix = timeUntilEndOfProblem;
        break;
      case Status::Failed:
        Serial.print(convertMillisecondsToPrintableTime(timeUntilEnd));
        Serial.println(" Failed");
        timeToPrintToLedMatrix = timeUntilEnd;
        break;
      case Status::Success:
        Serial.print(convertMillisecondsToPrintableTime(timeUntilEnd));
        Serial.println(" Success");
        timeToPrintToLedMatrix = timeUntilEnd;
        break;
      default:
        Serial.println("Unknown status");
        break;
    }

    // Next time we should update uart and led matrix
    uartAndLedMatrixUpdateTime += 125;


    // Led time logic
    bool showTime = false;
    switch(status)
    {
      case Status::InitSequence:
      case Status::WaitingForProblem:
        // Not blinking
        showTime = true;
        break;
      case Status::ShowingSequence:
      case Status::SolvingSequence:
        // Slow blinking
        showTime = ((currentTime / 500) % 2) == 0;
        break;
      case Status::Failed:
        // Fast blinking
        showTime = ((currentTime / 250) % 2) == 0;
        break;
      case Status::Success:
        // Fast blinking at the end
        if (currentTime > END_TIME_SUCCESS - GRACE_PERIOD_MS)
        {
          showTime = ((currentTime / 250) % 2) == 0;
        }
        else
        {
          showTime = true;
        }
        break;
      default:
        showTime = false;
        break;
    }

    ledMatrixPrinter.clear();
    if (true == showTime)
    {
      ledMatrixPrinter.addChar(timeToPrintToLedMatrix / 1000 / 60 + '0', 2);
      ledMatrixPrinter.addChar(':', 10);
      ledMatrixPrinter.addChar(((timeToPrintToLedMatrix / 1000) % 60) / 10 + '0', 16);
      ledMatrixPrinter.addChar((timeToPrintToLedMatrix / 1000) % 10 + '0', 24);
    }
    ledMatrixPrinter.print();
  }
}
