#ifndef _DIGITALPULSE_H_
#define _DIGITALPULSE_H_

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
 * This file contains the definition of DigitalPulse. This struct
 * holds information describing a pulse read from an input pin.
 */
 
#include <Arduino.h> 

/**
* constants for pulse description
*/
#define PULSE_DESCRIPTION_MAX              32
#define PULSE_VALUE_BUFFER_MAX             16
#define PULSE_DESCRIPTION_VALUE_DELIMITER  '|'

/**
* constants for pulse threshold definitions 
*/
#define SHORT_PRESS_MILS    100
#define LONG_PRESS_MILS    1000

/**
* struct containg digital pulse information 
*/
struct DigitalPulse {
   /**
   * pulse start time, milliseconds since reset
   */
   long startTime;
      
  /**
   * pulse end time, milliseconds since reset
   */
   long endTime;
      
  /**
   * computed duration of pulse in milliseconds
   */
   int duration;
      
  /**
   * flag is true if pulse is ready (has valid start/end times)
   */
   bool isValid;
      
  /**
   * character buffer used to store pulse description
   */
   char pulseDescription[PULSE_DESCRIPTION_MAX];

   
  /**
   * DigitalPulse constructor
   * creates empty pulse object
   */
   DigitalPulse()
   : startTime(0), endTime(0), duration(0), isValid(false)
   {}
      
  /**
   * character used to delimit pulse start/end time
   */
   static const char VALUE_DELIMITER;
   
  /**
   * reset pulse to cleared condition
   */
   void reset() {
      startTime = 0;
      endTime = 0;
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
      endTime = startTime = tm;
   }
   
  /**
   * set pulse end time
   *
   * @param  tm    pulse end time, milliseconds since reset
   */
   void setEnd(long tm) {
      endTime = tm;
      duration = endTime - startTime;
      isValid = (duration > 0);
   }
   
  /**
   * returns pulse description text
   *
   * @return pointer to pulse description text
   */
   const char * getDescription();  
};

#endif // _DIGITALPULSE_H_
