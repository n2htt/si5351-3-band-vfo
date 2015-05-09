#ifndef LCD2004_LCDLIB_VFODISPLAY_H
#define LCD2004_LCDLIB_VFODISPLAY_H

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
 * This file contains class definitions for LCD2004_LCDLib_VFODisplay. 
 * This class implements the methods defined in base class
 * VFODisplay, using the LiquidCrystal library, which may be found
 * here:
 * 
 * https://bitbucket.org/fmalpartida/new-liquidcrystal/downloads
 * 
 */
 
#include <Arduino.h> 
#include "VFODisplay.h"

/**
 * This class implements the methods defined in base class
 * VFODisplay, using the Adafruit SSD1306 I2C library.
 */
class LCD2004_LCDLib_VFODisplay : public VFODisplay {
protected:   
   /**
    * pointer to externally defined LCD display object
    */
   LiquidCrystal_I2C *mp_display;
   
   /**
    * show vfos display method 
    */
   void displayVFOScreen() {
      mp_display->setCursor(0, 0); // Start at character 0, line 0
      mp_display->print(HEADING_PREFIX);
      mp_display->print(ml_freq_delta, DEC);
      
      for (int ii = 0; ii<mi_number_of_vfos; ++ii) {
         mi_displayLine = ii;
         mb_enabled = mpp_vfos[mi_displayLine]->isEnabled();
         ml_freq = mpp_vfos[mi_displayLine]->getFrequency();
         
         mp_display->setCursor(11, ii+1); // Start at character 0, line ii+1

         formatIndicator();
         mp_display->write(ms_buffer[0]);
         
         formatFrequencyMHz();  
         mp_display->print(ms_buffer);
      }      
   }
   
   /**
    * show vfos display method 
    */
   void displayFrequencyDeltaScreen() {
      mp_display->setCursor(0, 1); // Start at character 0, line 1
      mp_display->write(mc_freqDelta);
      mp_display->print(" freq =");
      
      mp_display->setCursor(0, 2); // Start at character 0, line 2
      mp_display->print(ml_freq_delta, DEC);
   }
   
public:
   /**
    * Constructor 
    * 
    * @param  vfo list
    * @param  num_vfos number of vfos to show in display
    */
   LCD2004_LCDLib_VFODisplay(VFODefinition **vfos, int num_vfos)
   : VFODisplay(vfos, num_vfos)
   { 
      // override some of the default display characters
      mc_indicator = '~'; // this renders as a right arrow
      mc_disabled  = 219; // alt 165
      mc_freqDelta = 94;
       
      mp_display = new LiquidCrystal_I2C(0x20, 4, 5, 6, 0, 1, 2, 3, 7, NEGATIVE);  // Set the LCD I2C address     
       
      mp_display->begin(20,4);         // initialize the lcd for 20 chars 4 lines
      mp_display->clear();
      mp_display->backlight();
   }
   
   /**
    * Destructor 
    */
   virtual ~LCD2004_LCDLib_VFODisplay() 
   {
      if (mp_display != 0) {
         delete mp_display;
         mp_display = 0;
      }
   }

   /**
    * display heading line and all vfos 
    * @param  f_delta current frequency change increment
    * @param  currentVFO subscript of selected vfo in list (from 0)
    */
   void showVFOs(unsigned long f_delta, short currentVFO) {
      ml_freq_delta = f_delta;
      mi_currentVFO = currentVFO;
      
      mp_display->clear();
      displayVFOScreen();
   }
     
   /**
    * display new value of frequency change increment
    * @param  f_delta new frequency change increment
    */
   virtual void showFreqDeltaDisplay(unsigned long f_delta) {
      ml_freq_delta = f_delta;
      
      mp_display->clear();      
      displayFrequencyDeltaScreen();
   }
};   
   
#endif // LCD2004_LCDLIB_VFODISPLAY_H
