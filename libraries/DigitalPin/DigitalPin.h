#ifndef _DIGITALPIN_H_
#define _DIGITALPIN_H_

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
 * This file contains class definitions for DigitalPin, and two
 * child classes, DigitalInputPin and DigitalOutputPin. These classes
 * encapsulate Arduino hardware pin behavior
 */
 
#include <Arduino.h> 
#include <DigitalPulse.h>

/**
 * defined constants used by DigitalPin classes
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
 * enum for defined input pin pulse modes
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
enum InputPinMode{
	    PIN_MODE_UNKNOWN
	   ,PIN_MODE_IDLE
	   ,PIN_MODE_SHORT_PULSE
	   ,PIN_MODE_LONG_PULSE
}; 

/**
* This is the base class for input and output hardware pins
*/
class DigitalPin {
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
   * records intial state of the pin
   * used when the pin is forced to idle mode
   */
   int  initialState;
   
   /**
   * this flag when asserted sets the logical state 
   * to the complement of the physical state
   */
	bool invertSense;
   
   /**
   * this flag enables/disables all pin behaviors
   */
   bool enabled;
    
public:   
  /**
   * enables/disables output of pulse start/end
   * to the serial port
   */
   static bool writePulsesToSerialEnabled;
   
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
   * disables all pin behaviors
   */
   virtual void suspend() {
      enabled = false;
   }
   
  /**
   * enables all pin behaviors
   */
   virtual void resume() {
      enabled = true;
   }
   
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
   * DigitalPin Constructor
   *
   * @param  pn       pin number
   * @param  md       pin mode
   * @param  ini_st   initial pin state
   * @param  invert   is pin sense inverted?
   */
   DigitalPin (int pn, int md, int ini_st, bool invert)
   : pinNumber(pn)
   , mode(md)
   , state(ini_st)
   , initialState(ini_st)
   , invertSense(invert)
   , enabled(true)
   {}
};

/**
* This class represents output hardware pins and only allows
* output operations on the pin.
*/
class DigitalOutputPin : public DigitalPin {
protected:
   
public:   

  /**
   * performs hardware pin initializations
   */
   virtual void initialize();
   
  /**
   * disables all pin behaviors
   */
   virtual void suspend();
   
  /**
   * converts input values to {LOW, HIGH}
   * stores input value to pin logical state
   * writes stored logical state to pin
   * 
   * @param  val      value to set/write
   */
   void writeLogicalValue(int val) {
      setLogicalState(val); 
      writeState();
   }
   
  /**
   * writes stored physical state to pin
   */
   void writeState() {
      digitalWrite(pinNumber, getState());
   }
   
  /**
   * converts input values to {LOW, HIGH}
   * stores input value to pin physical state
   * writes stored physical state to pin
   * 
   * @param  val      value to set/write
   */
   void writeValue(int val) {
      setState(val); 
      writeState();
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
   void outputPulse(unsigned int pulse_width_mils
                   ,unsigned int hang_time_mils = 0
                   ,unsigned int lead_time_mils = 0
                   ,int initial_state = LOW);
                                  
  /**
   * DigitalPin Constructor
   *
   * @param  pn       pin number
   * @param  ini_st   initial pin state
   */
   DigitalOutputPin (int pn
                   , int ini_st
                   , bool invert = DIGITAL_PIN_NON_INVERTING)
   : DigitalPin(pn, OUTPUT, ini_st, invert)
   {}
};

/**
* This class represents input hardware pins and only allows
* input operations on the pin.
*/
class DigitalInputPin : public DigitalPin {
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
   DigitalPulse pulse;
   
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
   * enable/disables writing pulse description to serial port
   */
   bool writeToSerial;
	
  /**
   * updates stored pulse on a state change
   */
   void processPinState(long tm, int priorState);
   
public:   
  /**
   * disables all pin behaviors
   */
   virtual void suspend();
   
  /**
   * reads physical pin state and applies debounce logic
   */
   void determinePinState();
   
  /**
   * forces pin state to value, and performs logic as though
   * the forced state had just been read
   * 
   * @param  lstate    the logical state to force on the pin
   */
   void forceLogicalPinState(int lstate);   
 
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
   * sets indication on the output pin to correspond to the
   * physical state of the input pin
   * 
   * @param  outputPin    the output pin to display state on
   */
   void indicate(DigitalOutputPin &outputPin);
   
  /**
   * sets indication on the output pin to correspond to the
   * inverse of the physical state of the input pin
   * 
   * @param  outputPin    the output pin to display state on
   */
   void indicateInverse(DigitalOutputPin &outputPin);
   
  /**
   * returns last pulse read on pin
   * 
   * @return  the current stored pulse
   */
   DigitalPulse getLastPulse() const {
      return pulse;
   }
   
  /**
   * writes pulse description to the serial port
   */
   void writePulseToSerial();
   
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
   * DigitalPin Constructor
   *
   * @param  pn           pin number
   * @param  md           pin mode
   * @param  dbt          debounce threshold, milliseconds
   * @param  ini_st       initial pin state
   * @param  invert       is pin sense inverted?
   * @param  write_to_ser should this pin write pulse description to serial
   */
   DigitalInputPin (int pn
                  , int md
                  , int dbt
                  , int ini_st
                  , bool invert = DIGITAL_PIN_NON_INVERTING
                  , bool write_to_ser = false)
   : DigitalPin (pn, md, ini_st, invert)
   , lastReadTime(0L)
   , debounceThreshold(dbt)
   , lastReading(ini_st)
   , stateChanged(false)
   , currentPinMode(PIN_MODE_IDLE)
   , writeToSerial(write_to_ser)
   { 
      setState(state); 
   }
};

   
#endif // _DIGITALPIN_H_
