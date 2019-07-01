/*
  virt_stepper.c - virtual stepper motor driver: Emulates the grbl driver
                   scheme, so it can easily drive unipolar steppers or servos
  Part of Grbl

  Copyright (c) 2019 Dirk Herrendoerfer <d.herrendoerfer@herrendoerfer.name>

  Grbl is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Grbl is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with Grbl.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "grbl.h"

static volatile uint8_t stepX = 0;
static volatile uint8_t stepY = 0;
static volatile uint8_t stepZ = 0;

static uint8_t steplist_Unipolar[] = { 0B1000,0B1100,0B0100,0B0110,0B0010,0B0011,0B0001,0B1001 };

void step_unipolar()
{
  // Hardcoded ports

  //Stepper 1(X)
  PORTB = (PORTB & 0B11111000) | (steplist_Unipolar[stepX & 0x07] >> 1);
  PORTD = (PORTD & 0B01111111) | (steplist_Unipolar[stepX & 0x07] << 7);
  //Stepper 2(Y)
  PORTD = (PORTD & 0B10000111) | (steplist_Unipolar[stepY & 0x07] << 3);
  //Stepper 1(Z)
  #ifdef SERVO_Z_PROPORTIONAL
    servoVal = stepZ;
  #else
    PORTC = (PORTC & 0B11110000) | steplist_Unipolar[stepZ & 0x07];
  #endif
}

void step_unipolar_init()
{
  DDRB |= 0B00000111;
  DDRD |= 0B11111000;
  DDRC |= 0B00001111;

  #ifdef SERVO_Z_PROPORTIONAL
    servo_init();
    if (1<<Z_DIRECTION_BIT)
      stepZ = 180;
    else
      stepZ = 0;
    servoVal = stepZ;
  #endif
}

void step_unipolar_power()
{
  if (STEP_PORT & (1<<STEPPERS_DISABLE_BIT)) {
    //Stepper 1(X)
    PORTB = PORTB & ~0B00000111;
    PORTD = PORTD & ~0B10000000;
    //Stepper 2(Y)
    PORTD = PORTD & ~0B01111000;
    //Stepper 3(Z)
    PORTC = PORTC & ~0B00001111;
  }
  else {
    step_unipolar(); //put the last step on the steppers
  }
}

void step_virtual_unipolar()
{
  if (STEP_PORT & (1<<X_STEP_BIT)) {
    if (DIRECTION_PORT & (1<<X_DIRECTION_BIT))
      stepX++;
    else
      stepX--;
  }
  if (STEP_PORT & (1<<Y_STEP_BIT)) {
    if (DIRECTION_PORT & (1<<Y_DIRECTION_BIT))
      stepY++;
    else
      stepY--;
  }
  if (STEP_PORT & (1<<Z_STEP_BIT)) {
    if (DIRECTION_PORT & (1<<Z_DIRECTION_BIT)) {
      #ifdef SERVO_Z_PROPORTIONAL
        if (stepZ > 0)
          stepZ--;
      #else
        stepZ--;
      #endif
    }
    else {
      #ifdef SERVO_Z_PROPORTIONAL
        if (stepZ < 180)
          stepZ++;
      #else
        stepZ++;
      #endif
    }
  }

  step_unipolar();
}
