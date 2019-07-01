/*
  virt_servo.c - a timer2 based servo driver

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

static unsigned int servoIsrCounter = 0;

void servo_init()
{
  // Enable Pin output
  SERVO_DDR |= (1<<SERVO_PIN);

  servoIsrCounter = 0; // clear the value of the ISR counter;

  TCCR2A = 0;
  TCCR2B = 0;

  /* Configure timer2 in normal mode (pure counting, no PWM etc.) */
  TCCR2A &= ~((1<<WGM21) | (1<<WGM20));
  TCCR2B &= ~(1<<WGM22);

  /* Disable Compare Match A interrupt enable (only want overflow) */
  TIMSK2 &= ~(1<<OCIE2A);

  TCNT2 = 140;              // preload timer
  TCCR2B |= (1 << CS22);    // prescaler
  TCCR2B |= (1 << CS20);    // prescaler

  TCCR2B &= ~(1 << CS21);
  TIMSK2 |= (1 << TOIE2);   // enable timer overflow interrupt

  // Finally, turn stuff on
  servo_enabled = 1;
  servoVal = 0;
}


ISR(TIMER2_OVF_vect)
{
  TCNT2 = 236; // preload timer value *experimented
  servoIsrCounter++;

  if (servoIsrCounter == SERVO_FRAME_LENGTH) {
    servoIsrCounter = 0;
    return;
  }

  if (servoIsrCounter == 1) {
    TCNT2 = 200 - (servoVal>>1);
    SERVO_PORT = SERVO_PORT | (1<<SERVO_PIN);
    return;
  }
  if (servoIsrCounter == 2) {
    TCNT2 = 200 - (servoVal>>1);
    return;
  }
  if (servoIsrCounter == 3) {
    SERVO_PORT = SERVO_PORT & ~(1<<SERVO_PIN);
  }
}
