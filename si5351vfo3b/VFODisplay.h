#ifndef VFODISPLAY_H
#define VFODISPLAY_H

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
 * This file contains the definition of base class VFODisplay, 
 * which defines the methods needed by the three band VFO to 
 * display VFO frequency and status.
 */
 
#include <Arduino.h> 

/**
 * display character constants
 */
#define DISPLAY_BUFFER_MAX               16    // buffer for intermediate strings
#define INDICATOR_CHARACTER              0xb6  // right pointing triangle
#define DISABLED_CHARACTER               0xa1  // empty square
#define NOT_SELECTED                     ' '   // space
#define FREQ_DELTA_CHARACTER             0xb3  // up pointing triangle
#define HEADING_PREFIX                   "SI5351 N2HTT "


/**
 * This class defines the methods needed by the three band VFO to 
 * display VFO frequency and status.
 */
class VFODisplay {
protected: 
   char            ms_buffer[DISPLAY_BUFFER_MAX];
   unsigned long   ml_freq;
   unsigned long   ml_freq_delta;
   
   VFODefinition **mpp_vfos;
   int             mi_displayFunc;     
   int             mi_number_of_vfos;
   int             mi_currentVFO;
   int             mi_displayLine;
   boolean         mb_enabled;
    
   /**
    * Constructor     
    * 
    * @param  num_vfos number of vfos to show in display
    */
   VFODisplay(VFODefinition **vfos, int num_vfos)
   : mpp_vfos(vfos)
   , mi_number_of_vfos(num_vfos)
   , ml_freq(0)
   , ml_freq_delta(0)
   , mi_currentVFO(0)
   , mi_displayLine(0)
   , mi_displayFunc(0)
   , mb_enabled(false)
   {ms_buffer[0]=0;}
   
public:
    
   /**
    * abstract method 
    * display heading line and all vfos 
    * @param  f_delta current frequency change increment
    * @param  currentVFO subscript of selected vfo in list (from 0)
    * @param  vfoList pointer to array holding vfo list
    */
   virtual void showVFOs(unsigned long f_delta, short currentVFO) = 0;
    
   /**
    * abstract method 
    * display new value of frequency change increment
    * @param  f_delta new frequency change increment
    */
   virtual void showFreqDeltaDisplay(unsigned long f_delta) = 0;
};   
   
#endif // VFODISPLAY_H
