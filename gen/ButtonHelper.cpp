#include "ButtonHelper.hpp"
#include <Arduino.h>
#include "Defines.hpp"
#include "PinHelper.hpp"

void illuminateOneButton(Button button)
{
  digitalWrite(RED_LED_PIN, button == Button::Red ? LOW : HIGH);
  digitalWrite(YELLOW_LED_PIN, button == Button::Yellow ? LOW : HIGH);
  digitalWrite(GREEN_LED_PIN, button == Button::Green ? LOW : HIGH);
  digitalWrite(WHITE_LED_PIN, button == Button::White ? LOW : HIGH);
}

void illuminateAllButtons()
{
  digitalWrite(RED_LED_PIN, LOW);
  digitalWrite(YELLOW_LED_PIN, LOW);
  digitalWrite(GREEN_LED_PIN, LOW);
  digitalWrite(WHITE_LED_PIN, LOW);
}

void illuminateNoButton()
{
  digitalWrite(RED_LED_PIN, HIGH);
  digitalWrite(YELLOW_LED_PIN, HIGH);
  digitalWrite(GREEN_LED_PIN, HIGH);
  digitalWrite(WHITE_LED_PIN, HIGH);
}

void setButtonIllumination(Button button, bool illuminated)
{
  if (Button::None != button &&
      Button::Multiple != button)
  {
    digitalWrite(getLedPin(button), illuminated ? LOW : HIGH);  
  }
}
void illuminatePressedButton()
{
  setButtonIllumination(Button::Red, !digitalRead(RED_SWITCH_PIN));
  setButtonIllumination(Button::Yellow, !digitalRead(YELLOW_SWITCH_PIN));
  setButtonIllumination(Button::Green, !digitalRead(GREEN_SWITCH_PIN));
  setButtonIllumination(Button::White, !digitalRead(WHITE_SWITCH_PIN));
}

Button getPressedButton()
{
  Button returnValue = Button::None;
  uint8_t numberOfPressedButtons = 0;
  if (false == digitalRead(RED_SWITCH_PIN))
  {
    returnValue = Button::Red;
    ++numberOfPressedButtons;
  }
  if (false == digitalRead(YELLOW_SWITCH_PIN))
  {
    returnValue = Button::Yellow;
    ++numberOfPressedButtons;
  }
  if (false == digitalRead(GREEN_SWITCH_PIN))
  {
    returnValue = Button::Green;
    ++numberOfPressedButtons;
  }
  if (false == digitalRead(WHITE_SWITCH_PIN))
  {
    returnValue = Button::White;
    ++numberOfPressedButtons;
  }

  if (1 < numberOfPressedButtons)
  {
    returnValue = Button::Multiple;
  }

  return returnValue;
}
