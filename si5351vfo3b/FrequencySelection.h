#ifndef FREQUENCYSELECTION_H
#define FREQUENCYSELECTION_H
/**
 * @file
 * @author  Mike Aiello N2HTT <n2htt@arrl.net>
 * @version 1.0
 *
 * @section LICENSE
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details at
 * http://www.gnu.org/copyleft/gpl.html
 *
 * @section DESCRIPTION
 *
 * This file contains functions used implement the rotary encoder based
 * frequency selection logic.
 * 
 * Hardware and software debouncing of the encoder is accomplished with 
 * the macros defined below. Use the smallest values that give smooth
 * operation of your encoder without jitter in the frequency values.
 * Note that the movement threshold value is a divisor of the encoder's
 * ppr; the default value of 2 essentially halves the ppr of your. The
 * minimum value for this parameter is 1 - a value of 1 turns off
 * the software debounce feature.
 */
 
#include <Arduino.h> 

/**
 * encoder constants
 */
#define ENCODER_MOVEMENT_THRESHOLD        2
#define ENCODER_DEBOUNCE_WAIT_MILS        1

/**
 * variables tracking encoder movement
 */
volatile long encoder_movement;
boolean A_set = false;
boolean B_set = false;

/**
 * service interrupt on encoder pin A changing state
 */
void encoderPinA_ISR() {
   delay(ENCODER_DEBOUNCE_WAIT_MILS);
   if( digitalRead(ENCODER_PIN_A) != A_set ) {  
      A_set = !A_set;

      // decrement counter if A leads B
      if ( A_set && !B_set ) {
         --encoder_movement;
      }
   }
}

/**
 * service interrupt on encoder pin B changing state
 */
void encoderPinB_ISR(){
   delay(ENCODER_DEBOUNCE_WAIT_MILS);
   if( digitalRead(ENCODER_PIN_B) != B_set ) {
      B_set = !B_set;

      // increment counter - if B leads A
      if( B_set && !A_set ) {
         ++encoder_movement;
      }    
   }
}

/**
 * updates current VFO frequency as needed
 * @return true if frequency has changed
 */
boolean updateSelectedFrequencyValue() {
   boolean rtn = false;
   
   if (encoder_movement != 0 ) {
      // updating has to be done as a critical section
      noInterrupts();

      if (encoder_movement >= ENCODER_MOVEMENT_THRESHOLD) {
         // adjust frequency positive
         vfoList[currVFO]->increaseFrequency(frequency_delta);

         // reset movement and tell caller to redisplay the frequencies
         encoder_movement = 0;
         rtn = true;
      }  
      else if (encoder_movement <= -(ENCODER_MOVEMENT_THRESHOLD)) {
         // adjust frequency negative
         vfoList[currVFO]->decreaseFrequency(frequency_delta);

         // reset movement and tell caller to redisplay the frequencies
         encoder_movement = 0;
         rtn = true;
      }

      // end critical section
      interrupts();

      // outside of critical section
      // install new frequency in clock if needed
      if (rtn) {
         vfoList[currVFO]->loadFrequency();
      }
   }

   return rtn;
}
   
#endif // FREQUENCYSELECTION_H
