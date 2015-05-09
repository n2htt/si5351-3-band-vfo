#ifndef _SIMPLEDIGITALPULSE_H_
#define _SIMPLEDIGITALPULSE_H_

/**
 * @file
 * @author  Mike Aiello N2HTT <n2htt@arrl.net>
 * @version 1.3
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
 * This file contains the definition of SimpleDigitalPulse. This struct
 * holds information describing a pulse read from an input pin.
 */
 
#include <Arduino.h> 

/**
* constants for pulse threshold definitions 
*/
#define SHORT_PRESS_MILS     50
#define LONG_PRESS_MILS    1000

/**
* struct containg digital pulse information 
*/
struct SimpleDigitalPulse {
   /**
   * pulse start time, milliseconds since reset
   */
   long startTime;
      
  /**
   * computed duration of pulse in milliseconds
   */
   int duration;
      
  /**
   * flag is true if pulse is ready (has valid start/end times)
   */
   bool isValid;
   
  /**
   * SimpleDigitalPulse constructor
   * creates empty pulse object
   */
   SimpleDigitalPulse()
   : startTime(0), duration(0), isValid(false)
   {}
   
  /**
   * reset pulse to cleared condition
   */
   void reset() {
      startTime = 0;
      duration = 0; 
      isValid = false;
   }
   
  /**
   * set pulse start time
   *
   * @param  tm    pulse start time, milliseconds since reset
   */
   void setStart(long tm) {
      isValid = false;
      duration = 0;
      startTime = tm;
   }
   
  /**
   * set pulse end time
   *
   * @param  tm    pulse end time, milliseconds since reset
   */
   void setEnd(long tm) {
      duration = tm - startTime;
      isValid = (duration > 0);
   }
};

#endif // _SIMPLEDIGITALPULSE_H_
