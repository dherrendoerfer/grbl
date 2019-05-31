/*
  spindle_control.c - spindle control methods
  Part of Grbl

  Copyright (c) 2012-2015 Sungeun K. Jeon
  Copyright (c) 2009-2011 Simen Svale Skogsrud

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

volatile uint8_t servoVal = 0;
volatile uint8_t servo_enabled = 1;

#define SERVO_FRAME_LENGTH  120
#define SERVO_PIN           4

void servo_spindle_init()
{
  // Enable Pin output
  DDRB |= (1<<SERVO_PIN);

  servoIsrCounter = 0; // clear the value of the ISR counter;

  TCCR2A = 0;
  TCCR2B = 0;

  /* Configure timer2 in normal mode (pure counting, no PWM etc.) */
  TCCR2A &= ~((1<<WGM21) | (1<<WGM20));
  TCCR2B &= ~(1<<WGM22);

  /* Disable Compare Match A interrupt enable (only want overflow) */
  TIMSK2 &= ~(1<<OCIE2A);

  TCNT2 = 140;            // preload timer 65536-16MHz/256/2Hz
  TCCR2B |= (1 << CS22);    // prescaler
  TCCR2B |= (1 << CS20);    // prescaler
  //TCCR2B &= ~(1 << CS20);
  TCCR2B &= ~(1 << CS21);
  TIMSK2 |= (1 << TOIE2);   // enable timer overflow interrupt
}

ISR (TIMER2_OVF_vect)
{
  TCNT2 = 236; // preload timer value *experimented
  servoIsrCounter++;

  if (servoIsrCounter == SERVO_FRAME_LENGTH) {
    servoIsrCounter = 0;
    return;
  }

  if (servo_enabled && servoIsrCounter == 1) {
    TCNT2 = 200 - (servoVal>>1);
    PORTB = PORTB | (1<<SERVO_PIN);  //Pin12
    return;
  }
  if (servoIsrCounter == 2) {
    TCNT2 = 200 - (servoVal>>1);
    return;
  }
  if (servoIsrCounter == 3) {
    PORTB = PORTB & ~(1<<SERVO_PIN);  //Pin12)
  }
}

void spindle_init()
{
  // Configure variable spindle PWM and enable pin, if requried. On the Uno, PWM and enable are
  // combined unless configured otherwise.
  #ifdef VARIABLE_SPINDLE
    SPINDLE_PWM_DDR |= (1<<SPINDLE_PWM_BIT); // Configure as PWM output pin.
    #if defined(CPU_MAP_ATMEGA2560) || defined(USE_SPINDLE_DIR_AS_ENABLE_PIN)
      SPINDLE_ENABLE_DDR |= (1<<SPINDLE_ENABLE_BIT); // Configure as output pin.
    #endif
  // Configure no variable spindle and only enable pin.
  #else
    SPINDLE_ENABLE_DDR |= (1<<SPINDLE_ENABLE_BIT); // Configure as output pin.
  #endif

  #ifndef USE_SPINDLE_DIR_AS_ENABLE_PIN
    SPINDLE_DIRECTION_DDR |= (1<<SPINDLE_DIRECTION_BIT); // Configure as output pin.
  #endif
  servo_spindle_init();
  spindle_stop();
}


void spindle_stop()
{
  // On the Uno, spindle enable and PWM are shared. Other CPUs have seperate enable pin.
  #ifdef VARIABLE_SPINDLE
    TCCRA_REGISTER &= ~(1<<COMB_BIT); // Disable PWM. Output voltage is zero.
    #if defined(CPU_MAP_ATMEGA2560) || defined(USE_SPINDLE_DIR_AS_ENABLE_PIN)
      #ifdef INVERT_SPINDLE_ENABLE_PIN
        SPINDLE_ENABLE_PORT |= (1<<SPINDLE_ENABLE_BIT);  // Set pin to high
      #else
        SPINDLE_ENABLE_PORT &= ~(1<<SPINDLE_ENABLE_BIT); // Set pin to low
      #endif
    #endif
  #else
    #ifdef INVERT_SPINDLE_ENABLE_PIN
      SPINDLE_ENABLE_PORT |= (1<<SPINDLE_ENABLE_BIT);  // Set pin to high
    #else
      SPINDLE_ENABLE_PORT &= ~(1<<SPINDLE_ENABLE_BIT); // Set pin to low
    #endif
  #endif

  servoVal = 90;
}


void spindle_set_state(uint8_t state, float rpm)
{
  // Halt or set spindle direction and rpm.
  if (state == SPINDLE_DISABLE) {

    spindle_stop();

  } else {
    uint8_t servo_level = 90;
    if (rpm <= 90)
      servo_level = rpm;

    #ifndef USE_SPINDLE_DIR_AS_ENABLE_PIN
      if (state == SPINDLE_ENABLE_CW) {
        SPINDLE_DIRECTION_PORT &= ~(1<<SPINDLE_DIRECTION_BIT);
        servoVal = 90+servo_level;
      } else {
        SPINDLE_DIRECTION_PORT |= (1<<SPINDLE_DIRECTION_BIT);
        servoVal = 90-servo_level;
      }
    #endif

    #ifdef VARIABLE_SPINDLE
      // TODO: Install the optional capability for frequency-based output for servos.
      #ifdef CPU_MAP_ATMEGA2560
      	TCCRA_REGISTER = (1<<COMB_BIT) | (1<<WAVE1_REGISTER) | (1<<WAVE0_REGISTER);
        TCCRB_REGISTER = (TCCRB_REGISTER & 0b11111000) | 0x02 | (1<<WAVE2_REGISTER) | (1<<WAVE3_REGISTER); // set to 1/8 Prescaler
        OCR4A = 0xFFFF; // set the top 16bit value
        uint16_t current_pwm;
      #else
        TCCRA_REGISTER = (1<<COMB_BIT) | (1<<WAVE1_REGISTER) | (1<<WAVE0_REGISTER);
        TCCRB_REGISTER = (TCCRB_REGISTER & 0b11111000) | 0x02; // set to 1/8 Prescaler
        uint8_t current_pwm;
      #endif

      if (rpm <= 0.0) { spindle_stop(); } // RPM should never be negative, but check anyway.
      else {
        #define SPINDLE_RPM_RANGE (SPINDLE_MAX_RPM-SPINDLE_MIN_RPM)
        if ( rpm < SPINDLE_MIN_RPM ) { rpm = 0; }
        else {
          rpm -= SPINDLE_MIN_RPM;
          if ( rpm > SPINDLE_RPM_RANGE ) { rpm = SPINDLE_RPM_RANGE; } // Prevent integer overflow
        }
        current_pwm = floor( rpm*(PWM_MAX_VALUE/SPINDLE_RPM_RANGE) + 0.5);
        #ifdef MINIMUM_SPINDLE_PWM
          if (current_pwm < MINIMUM_SPINDLE_PWM) { current_pwm = MINIMUM_SPINDLE_PWM; }
        #endif
        OCR_REGISTER = current_pwm; // Set PWM pin output

        // On the Uno, spindle enable and PWM are shared, unless otherwise specified.
        #if defined(CPU_MAP_ATMEGA2560) || defined(USE_SPINDLE_DIR_AS_ENABLE_PIN)
          #ifdef INVERT_SPINDLE_ENABLE_PIN
            SPINDLE_ENABLE_PORT &= ~(1<<SPINDLE_ENABLE_BIT);
          #else
            SPINDLE_ENABLE_PORT |= (1<<SPINDLE_ENABLE_BIT);
          #endif
        #endif
      }

    #else
      // NOTE: Without variable spindle, the enable bit should just turn on or off, regardless
      // if the spindle speed value is zero, as its ignored anyhow.
      #ifdef INVERT_SPINDLE_ENABLE_PIN
        SPINDLE_ENABLE_PORT &= ~(1<<SPINDLE_ENABLE_BIT);
      #else
        SPINDLE_ENABLE_PORT |= (1<<SPINDLE_ENABLE_BIT);
      #endif
    #endif

  }
}

void spindle_run(uint8_t state, float rpm)
{
  if (sys.state == STATE_CHECK_MODE) { return; }
  protocol_buffer_synchronize(); // Empty planner buffer to ensure spindle is set when programmed.
  spindle_set_state(state, rpm);
}
