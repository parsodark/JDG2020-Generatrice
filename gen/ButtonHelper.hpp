#pragma once

#include "Button.hpp"

void illuminateOneButton(Button button);

void illuminateAllButtons();

void illuminateNoButton();

void setButtonIllumination(Button button, bool illuminated);

void illuminatePressedButton();

Button getPressedButton();
