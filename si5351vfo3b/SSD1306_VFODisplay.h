#ifndef SSD1306_VFODISPLAY_H
#define SSD1306_VFODISPLAY_H

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
 * This file contains class definitions for SSD1306_VFODisplay. 
 * This class implements the methods defined in base class
 * VFODisplay, using the Adafruit SSD1306 I2C library.
 */
 
#include <Arduino.h> 
#include "VFODisplay.h"

/**
 * OLED display constants
 */
#define OLED_RESET 4
#define OLED_DISPLAY_ADDR 0x3c


/**
 * This class implements the methods defined in base class
 * VFODisplay, using the Adafruit SSD1306 I2C library.
 */
class SSD1306_VFODisplay : public VFODisplay {
protected:   
   /**
    * pointer to externally defined Adafruit_SSD1306 object
    */
   Adafruit_SSD1306 *mp_display;

public:

   /**
    * Constructor 
    * 
    * @param  num_vfos number of vfos to show in display
    */
   SSD1306_VFODisplay(int num_vfos) 
   : VFODisplay(num_vfos), mp_display(0)
   {
      mp_display = new Adafruit_SSD1306(OLED_RESET);
      
      // initialize with the I2C addr (for the 128x64)
      mp_display->begin(SSD1306_SWITCHCAPVCC, OLED_DISPLAY_ADDR);  
   }
   /**
    * Destructor 
    */
   virtual ~SSD1306_VFODisplay() 
   {
      if (mp_display != 0) {
         delete mp_display;
         mp_display = 0;
      }
   }
   
   /**
    * show indicator for selected vfo
    * @param  currentVFO subscript of selected vfo in list (from 0)
    * @param  displayLine subscript of display line being constructed
    * @param  isEnabled enabled/disabled status of current vfo
    */
   virtual void displayIndicator(int currentVFO, int displayLine, boolean isEnabled) {
      if (currentVFO == displayLine ) {
         mp_display->write((isEnabled)?INDICATOR_CHARACTER:DISABLED_CHARACTER);
      }
      else {
         mp_display->print(" ");  
      }
   }
    
   /**
    * display a frequency formatted as nnn.nnnnn 
    * @param  freq frequency in Hz
    */
   virtual void displayFrequencyMHz(unsigned long freq) {
      long mant = freq / 1000000L;
      long dec =  (freq % 1000000L) / 10L;
      char buf[9] = "";

      sprintf(buf, "%2lu", mant);
      mp_display->print(buf);
        
      mp_display->print(".");
        
      sprintf(buf, "%05lu", dec);
      mp_display->println(buf);  
   }
    
   /**
    * display heading line and all vfos 
    * @param  f_delta current frequency change increment
    * @param  currentVFO subscript of selected vfo in list (from 0)
    * @param  vfoList pointer to array holding vfo list
    */
   void showVFOs(long f_delta, int currentVFO, VFODefinition **vfoList) {
      // clear the buffer.
      mp_display->clearDisplay();

      // display heading line
      mp_display->setTextSize(1);
      mp_display->setTextColor(WHITE);
      mp_display->setCursor(0,0);
      mp_display->print(HEADING_PREFIX);
      mp_display->print(f_delta, DEC);
      mp_display->println("\n");
      mp_display->setTextSize(2);

      for (int ii = 0; ii<mi_number_of_vfos; ++ii) {
         displayIndicator(currentVFO, ii, vfoList[ii]->isEnabled());
         displayFrequencyMHz(vfoList[ii]->getFrequency());  
      }

      mp_display->display();
   }
     
   /**
    * display new value of frequency change increment
    * @param  f_delta new frequency change increment
    */
   virtual void showFreqDeltaDisplay(long f_delta) {
      // clear the buffer.
      mp_display->clearDisplay();

      mp_display->setTextSize(2);
      mp_display->setTextColor(WHITE);
      mp_display->setCursor(0,0);

      mp_display->print("\n");
      mp_display->write(FREQ_DELTA_CHARACTER);
      mp_display->print(" freq=\n");
      mp_display->print(f_delta);

      mp_display->display();
   }
};   
   
#endif // SSD1306_VFODISPLAY_H
