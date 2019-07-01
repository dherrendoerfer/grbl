/*
  virt_servo.h - a timer2 based servo driver

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

#ifndef virt_servo_h
#define virt_servo_h

volatile uint8_t servoVal;
volatile uint8_t servo_enabled;

#define SERVO_FRAME_LENGTH  120
#define SERVO_PIN           4
#define SERVO_DDR           DDRB
#define SERVO_PORT          PORTB

void servo_spindle_init();

#endif
