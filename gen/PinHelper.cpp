#include "PinHelper.hpp"
#include "Button.hpp"
#include "Defines.hpp"

uint8_t getLedPin(Button button)
{
	switch (button)
	{
	case Button::Red:
		return RED_LED_PIN;
	case Button::Yellow:
		return YELLOW_LED_PIN;
	case Button::Green:
		return GREEN_LED_PIN;
	case Button::White:
		return WHITE_LED_PIN;
	}
}
