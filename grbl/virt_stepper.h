/*
  virt_stepper.h - virtual stepper motor driver: Emulates the grbl driver
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

#ifndef virt_stepper_h
#define virt_stepper_h

/* Note: The the Port and direction values are hardcoded to there
         supported board, since they can overlap the hardware masks
         
   Make sure that only one of the folowing two defines is set; they 
   are exclusive! 
   You can set these here, or in the grblUpload.ino file in examples. 
 */

/* This define will make the servo move when the spindle is turned on
   or off, the degree of movement from the middle position is defined
   by the spindle speed (Use M4 S90 to turn it 90 degrees)
   The spindle direction is honored: the servo will move in different 
   directions
 */
//#define SERVO_SPINDLE

/* This define will make the servo move according to the Z-axis position.
   You must use the Z-axis steps per millimeter setting to define the 
   degrees per milimeter.
   Upon reset or initialisation the servo will always move to the zero 
   position (all the way up), and must be reset to real zero before each
   use.
   This will also disable the Z-axis stepper !
 */
//#define SERVO_Z_PROPORTIONAL

// This is the emulated port
volatile uint8_t vPORTD;

void step_unipolar();

void step_unipolar_init();

void step_unipolar_power();

void step_virtual_unipolar();

#endif
