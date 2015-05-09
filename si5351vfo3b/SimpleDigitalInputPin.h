#ifndef _DIGITALPININPUT_H_
#define _DIGITALPININPUT_H_

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
 * This file contains class definitions for SimpleDigitalPin, and its
 * child class SimpleDigitalInputPin. These classes
 * encapsulate Arduino hardware pin behavior for input.
 */
 
#include <Arduino.h> 
#include "SimpleDigitalPulse.h"

/**
 * defined constants used by SimpleDigitalPin classes
*/
#define DIGITAL_PIN_INIT_STATE_LOW   LOW
#define DIGITAL_PIN_INIT_STATE_HIGH  HIGH
#define DIGITAL_PIN_NO_DEBOUNCE      0L
#define DIGITAL_PIN_INVERTING        true
#define DIGITAL_PIN_NON_INVERTING    false
#define DIGITAL_PIN_UNINITIALIZED    -1
#define DIGITAL_PIN_WRITE_TO_SERIAL  true
#define DIGITAL_PIN_SUPPRESS_SERIAL  false

/**
 * input pin pulse modes
 * 
 * pin can be IDLE
 * 
 * or have just seen a SHORT_PULSE
 * (short threshold <= pulse < long threshold)
 * 
 * or have just seen a LONG_PULSE 
 * (long threshold <= pulse)
 * 
 *  or just be confused. (UNKNOWN) 
*/
#define PIN_MODE_UNKNOWN      0
#define PIN_MODE_IDLE         1
#define PIN_MODE_SHORT_PULSE  2
#define PIN_MODE_LONG_PULSE   3

/**
* This is the base class for input hardware pins
*/
class SimpleDigitalPin {
protected:   
   
   /**
   * pin number 
   */
	int  pinNumber;
   
   /**
   * pin hardware mode 
   */
	int  mode;
   
   /**
   * physical pin state
   */
	int  state;
   
   /**
   * logical pin state
   */
	int  logicalState;
   
   /**
   * records initial state of the pin
   * used when the pin is forced to idle mode
   */
   int  initialState;
   
   /**
   * this flag when asserted sets the logical state 
   * to the complement of the physical state
   */
	bool invertSense;
    
public:   
  /**
   * returns complement of input state
   * from the set of values {LOW,HIGH}
   */
   static int invertState(int st);

  /**
   * performs hardware pin initializations
   */
   virtual void initialize();
   
  /**
   * returns physical state of pin
   *
   * @return a physical state value from {LOW,HIGH}
   */
   int getState()  {
      return state; 
   }
   
  /**
   * returns logical state of pin
   *
   * @return a logical state value from {LOW,HIGH}
   */
   int getLogicalState()  {
      return logicalState; 
   }
   
  /**
   * sets physical state of pin, and logical pin state considering
   * the inversion flag
   *
   * @param  st      physical state set
   */
   void setState(int st) {
      state = (LOW == st) ? LOW : HIGH;
      logicalState = (!invertSense) ? state : invertState(state); 
   }
   
  /**
   * sets logical state of pin, and actual pin state considering
   * the inversion flag
   *
   * @param  lst      logical state set
   */
   void setLogicalState(int lst) {
      logicalState = (LOW == lst) ? LOW : HIGH;
      state = (!invertSense) ? logicalState : invertState(logicalState); 
   }
   
  /**
   * SimpleDigitalPin Constructor
   *
   * @param  pn       pin number
   * @param  md       pin mode
   * @param  ini_st   initial pin state
   * @param  invert   is pin sense inverted?
   */
   SimpleDigitalPin (int pn, int md, int ini_st, bool invert)
   : pinNumber(pn)
   , mode(md)
   , state(ini_st)
   , initialState(ini_st)
   , invertSense(invert)
   {}
};

/**
* This class represents input hardware pins and only allows
* input operations on the pin.
*/
class SimpleDigitalInputPin : public SimpleDigitalPin {
protected:
   
  /**
   * records last time pin state was interrogated
   */
	long lastReadTime;
   
  /**
   * minimum pulse width to register as an input pulse
   */
	long debounceThreshold;
   
  /**
   * stores the last pulse registered on the pin
   */
   SimpleDigitalPulse pulse;
   
  /**
   * stores current pulse mode of pin {IDLE, SHORT_PULSE, LONG_PULSE}
   */
	int  currentPinMode;
	
  /**
   * physical state of pin at last read
   */
	int  lastReading;
	
  /**
   * flag is true if physical state of pin has changed since last read
   */
	bool stateChanged;
   
  /**
   * updates stored pulse on a state change
   */
   void processPinState(long tm, int priorState);
   
public:   
  /**
   * reads physical pin state and applies debounce logic
   */
   void determinePinState();
    
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
   * @return  true if mode of pin has changed
   */
   bool readInputPulseMode();
   
  /**
   * returns last pulse read on pin
   * 
   * @return  the current stored pulse
   */
   SimpleDigitalPulse getLastPulse() const {
      return pulse;
   }
   
  /**
   * returns current pulse mode
   * 
   * @return  pulse mode, in {IDLE, SHORT_PULSE, LONG_PULSE}
   */
   int getCurrentPinMode() const {
       return currentPinMode;
    }
    
  /**
   * sets pulse mode on pin
   * 
   * @param  pm  the pulse mode to set in {IDLE, SHORT_PULSE, LONG_PULSE}
   */
   void setCurrentPinMode(int pm);
   
  /**
   * return flag indicating if pin state has changed
   * 
   * @return  true if pin state has changed with last read
   */
   bool hasChanged()
   { return stateChanged;}
   
  /**
   * SimpleDigitalPin Constructor
   *
   * @param  pn           pin number
   * @param  md           pin mode
   * @param  dbt          debounce threshold, milliseconds
   * @param  ini_st       initial pin state
   * @param  invert       is pin sense inverted?
   */
   SimpleDigitalInputPin (int pn
                  , int md
                  , int dbt
                  , int ini_st
                  , bool invert = DIGITAL_PIN_NON_INVERTING)
   : SimpleDigitalPin (pn, md, ini_st, invert)
   , lastReadTime(0L)
   , debounceThreshold(dbt)
   , lastReading(ini_st)
   , stateChanged(false)
   , currentPinMode(PIN_MODE_IDLE)
   { 
      setState(state); 
   }
};

#endif // _DIGITALPININPUT_H_
