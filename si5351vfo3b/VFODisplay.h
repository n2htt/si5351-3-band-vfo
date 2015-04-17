#ifndef VFODISPLAY_H
#define VFODISPLAY_H

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
 * This file contains the definition of base class VFODisplay, 
 * which defines the methods needed by the three band VFO to 
 * display VFO frequency and status.
 */
 
#include <Arduino.h> 

/**
 * display character constants
 */
#define INDICATOR_CHARACTER              16   // right pointing triangle
#define DISABLED_CHARACTER               126  // tilde
#define FREQ_DELTA_CHARACTER             127  // up pointing triangle
#define LBL_SEP_CHAR                     ":" 
#define HEADING_PREFIX                   "SI5351 N2HTT "


/**
 * This class defines the methods needed by the three band VFO to 
 * display VFO frequency and status.
 */
class VFODisplay {
protected: 

   int mi_number_of_vfos;

   
   /**
    * Constructor 
    * 
    * @param  num_vfos number of vfos to show in display
    */
   VFODisplay(int num_vfos)
   : mi_number_of_vfos(num_vfos)
   {}
   
public:
   
   /**
    * abstract method 
    * show indicator for selected vfo
    * @param  currentVFO subscript of selected vfo in list (from 0)
    * @param  displayLine subscript of display line being constructed
    * @param  isEnabled enabled/disabled status of current vfo
    */
   virtual void displayIndicator(int currentVFO, int displayLine, boolean isEnabled) = 0;
    
   /**
    * abstract method 
    * display a frequency formatted as nnn.nnnnn 
    * @param  freq frequency in Hz
    */
   virtual void displayFrequencyMHz(unsigned long freq) = 0;
    
   /**
    * abstract method 
    * display heading line and all vfos 
    * @param  f_delta current frequency change increment
    * @param  currentVFO subscript of selected vfo in list (from 0)
    * @param  vfoList pointer to array holding vfo list
    */
   virtual void showVFOs(long f_delta, int currentVFO, VFODefinition **vfoList) = 0;
    
   /**
    * abstract method 
    * display new value of frequency change increment
    * @param  f_delta new frequency change increment
    */
   virtual void showFreqDeltaDisplay(long f_delta) = 0;
};   
   
#endif // VFODISPLAY_H
