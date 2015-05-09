
/**
 * @file
 * @author  Mike Aiello N2HTT <n2htt@arrl.net>
 * @version 1.1
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
 * This file contains class implementations for SimpleDigitalPin, and its
 * child class, SimpleDigitalInputPin. These classes
 * encapsulate Arduino hardware input pin behavior
 */
#include <Arduino.h> 
#include "SimpleDigitalPulse.h"
#include "SimpleDigitalInputPin.h"
   
/**
 * returns complement of input state
 * from the set of values {LOW,HIGH}
 */
int SimpleDigitalPin::invertState(int st) {
    return (LOW == st) ? HIGH : LOW;
}

/**
 * performs hardware pin initializations
 */
void SimpleDigitalPin::initialize() {
  // initialize hardware pin mode
  pinMode(pinNumber, mode); 
}

/**
 * sets pulse mode on pin
 * 
 * @param  pm  the pulse mode to set in {IDLE, SHORT_PULSE, LONG_PULSE}
 */
void SimpleDigitalInputPin::setCurrentPinMode(int pm) {
    currentPinMode = pm;
    pulse.reset();
    setLogicalState(LOW);
 }
   
/**
 * updates stored pulse on a state change
 */
void SimpleDigitalInputPin::processPinState(long tm, int priorState) {

   // flag change in state
   stateChanged = state != priorState;

   // capture pulse
   if (stateChanged) {
      if (logicalState) {
         pulse.setStart(tm);
         }
      else {
         pulse.setEnd(tm);
      }
   }
}

/**
 * reads physical pin state and applies debounce logic
 */
void SimpleDigitalInputPin::determinePinState() {
   // save current pin state
   int priorState = state;
   
   // get curren system time
   long tm = millis();

   // read the input pin:
   int reading = digitalRead(pinNumber);

   if (reading != lastReading) {
      // pin input has changed
      // - start debounce timing
      lastReadTime = tm;
   }
   else {
      // pin input has not changed
      // - compare state age to threshold
      if ((tm - lastReadTime) > debounceThreshold) {
         // age exceeds debounce threshold
         // - accept new  state
         setState(reading);
      }
   } 

   // save the current pin reading
   lastReading = reading;

   // process pin state
   processPinState(tm, priorState);
}
 
/**
 * An input pin is always in one of three modes, depending on 
 * the last pulse read on the pin: IDLE, SHORT_PULSE and LONG_PULSE
 * 
 * The pin starts out in IDLE mode. A pulse detected
 * longer than the short pulse threshold but shorter than the 
 * long pulse threshold will move the pin to the SHORT_PULSE mode.
 * 
 * From the IDLE mode, a pulse detected that is longer than the
 * LONG_PULSE threshold will move the pin to LONG_PULSE mode.
 * 
 * From any mode other then IDLE, any pulse longer than the 
 * SHORT_PULSE threshold will move the mode back to IDLE. 
 * 
 * @return  true if state of pin has changed
 */
bool SimpleDigitalInputPin::readInputPulseMode() {
   bool rtn = false;
   
   // check state of input pin
   determinePinState();
   if (hasChanged()) {
      rtn = true; // signal state change to caller
      
      unsigned int pulseDur  = getLastPulse().duration;
      
      switch (currentPinMode) {
         case PIN_MODE_SHORT_PULSE:
            if (pulseDur >= SHORT_PRESS_MILS)  {
               currentPinMode = PIN_MODE_IDLE;
            }
            break;
            
         case PIN_MODE_LONG_PULSE:
           if (pulseDur >= SHORT_PRESS_MILS)  {
               currentPinMode = PIN_MODE_IDLE;
            }
            break;
            
         case PIN_MODE_IDLE:
           if (     (pulseDur >= SHORT_PRESS_MILS) 
               && !(pulseDur >= LONG_PRESS_MILS))  {
               currentPinMode = PIN_MODE_SHORT_PULSE;
            }
            else if (    (pulseDur >= SHORT_PRESS_MILS) 
                      && (pulseDur >= LONG_PRESS_MILS))  {
               currentPinMode = PIN_MODE_LONG_PULSE;
            }
            else {
               // pulse not wide enough
               // stay in PIN_MODE_IDLE
            }
            break;
            
         default:
            if (pulseDur >= SHORT_PRESS_MILS)  {
               currentPinMode = PIN_MODE_IDLE;
            }
            break;
      };
   }

   return rtn;
}




