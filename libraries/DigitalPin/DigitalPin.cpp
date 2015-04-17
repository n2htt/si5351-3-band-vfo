
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
 * This file contains class implementations for DigitalPin, and two
 * child classes, DigitalInputPin and DigitalOutputPin. These classes
 * encapsulate Arduino hardware pin behavior
 */
 
#include <Arduino.h> 
#include <DigitalPin.h>
#include <DigitalPulse.h>
/**
 * enables/disables output of pulse start/end
 * to the serial port
 */
bool DigitalPin::writePulsesToSerialEnabled = false;
   
/**
 * returns complement of input state
 * from the set of values {LOW,HIGH}
 */
int DigitalPin::invertState(int st) {
    return (LOW == st) ? HIGH : LOW;
}

/**
 * performs hardware pin initializations
 */
void DigitalPin::initialize() {
  // initialize hardware pin mode
  pinMode(pinNumber, mode); 
}

/**
 * performs hardware pin initializations
 */
void DigitalOutputPin::initialize() {
  // initialize hardware pin mode
  DigitalPin::initialize(); 
  
  // set hardware state for output pins
  digitalWrite(pinNumber, state);
}
   
/**
 * disables all pin behaviors
 */
void DigitalOutputPin::suspend() {
   setState(initialState);
   DigitalPin::suspend(); 
}
   
/**
 * performs an output pulse on the pin
 * restoring the pin to the state it was in
 * before the pulse 
 * 
 * @param  pulse_width_mils  width of pulse in milliseconds
 * @param  hang_time_mils    time to wait in intial state
 *                           after pulse is performed, milliseconds
 * @param  lead_time_mils    time to wait in intial state
 *                           before pulse is performed, milliseconds
 * @param  initial_state     pin is set to this state before pulse
 *                           is performed; the pulse itself is 
 *                           performed by raising the pin to the
 *                           complementary state. Default value LOW.
 */
void DigitalOutputPin::outputPulse(unsigned int pulse_width_mils
                                  ,unsigned int hang_time_mils
                                  ,unsigned int lead_time_mils
                                  ,int initial_state) {

   if (enabled) {
      int init_state;
      int pulse_state;
      
      // save current state of pin for restoration later
      int saved_state = state;
      
      // set up initial and pulsed state
      switch (initial_state) {
         default:
               init_state  = LOW;
         case  LOW:
               pulse_state = HIGH;
               break;
               
         case  HIGH:
               pulse_state = LOW;
               break;
      };
      
      // set pin to initial state
      writeValue(initial_state);
      
      // lead time if any
      if (lead_time_mils > 0) {
         delay(lead_time_mils);
      }
         
      // set pin to pulsed state
      writeValue(pulse_state);
      
      // pulse time
      if (pulse_width_mils > 0) {
         delay(pulse_width_mils);
      }

      // return pin to init state
      writeValue(initial_state);
      
      // hang time if any
      if (hang_time_mils > 0) {
         delay(hang_time_mils);
      }

      // leave it the way we found it
      writeValue(saved_state);
   }
}
/**
 * sets pulse mode on pin
 * 
 * @param  pm  the pulse mode to set in {IDLE, SHORT_PULSE, LONG_PULSE}
 */
void DigitalInputPin::setCurrentPinMode(int pm) {
    switch (pm) {
      case  PIN_MODE_SHORT_PULSE:
            currentPinMode = PIN_MODE_SHORT_PULSE;
            break;
         
      case  PIN_MODE_LONG_PULSE:
            currentPinMode = PIN_MODE_LONG_PULSE;
            break;
         
      default:
      case  PIN_MODE_IDLE:
            currentPinMode = PIN_MODE_IDLE;
            break;
    };

    pulse.reset();
    setLogicalState(LOW);
 }
   
/**
 * sets indication on the output pin to correspond to the
 * physical state of the input pin
 * 
 * @param  outputPin    the output pin to display state on
 */
void DigitalInputPin::indicate(DigitalOutputPin &outputPin) {
   if (enabled && stateChanged) {
      outputPin.setLogicalState(logicalState);
      outputPin.writeState();
   }
}
   
/**
 * sets indication on the output pin to correspond to the
 * inverse of the physical state of the input pin
 * 
 * @param  outputPin    the output pin to display state on
 */
void DigitalInputPin::indicateInverse(DigitalOutputPin &outputPin) {
   if (enabled && stateChanged) {
      outputPin.setLogicalState(invertState(logicalState));
      outputPin.writeState();
   }
}
	
/**
 * updates stored pulse on a state change
 */
void DigitalInputPin::processPinState(long tm, int priorState) {

   // flag change in state
   stateChanged = state != priorState;

   // capture pulse
   if (stateChanged) {
      if (logicalState) {
         pulse.setStart(tm);
         }
      else {
         pulse.setEnd(tm);

         // write pulse string to serial if enabled
         writePulseToSerial();
      }
   }
}

/**
 * reads physical pin state and applies debounce logic
 */
void DigitalInputPin::determinePinState() {
   if (enabled) {
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
}
   
/**
 * forces pin state to value, and performs logic as though
 * the forced state had just been read
 * 
 * @param  lstate    the logical state to force on the pin
 */
void DigitalInputPin::forceLogicalPinState(int lstate) {
   if (enabled) {
      // save current pin state
      int priorState = state;
      
      // get curren system time
      long tm = millis();
      
      // force state as if read from pin
      setLogicalState(lstate);
      
      // process as if read from pin
      processPinState(tm, priorState);
   }
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
bool DigitalInputPin::readInputPulseMode() {
   bool rtn = false;
   
   if (enabled) {
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
   }
      
   return rtn;
}

/**
 * disables all pin behaviors
 */
void DigitalInputPin::suspend() {
   setState(initialState);
   setCurrentPinMode(PIN_MODE_IDLE);
   DigitalPin::suspend();
}

/**
 * writes pulse description to the serial port
 */
void DigitalInputPin::writePulseToSerial() {
   if (writePulsesToSerialEnabled && writeToSerial) {
      if (pulse.isValid) {
         Serial.println(pulse.getDescription());
      }
   }
}



