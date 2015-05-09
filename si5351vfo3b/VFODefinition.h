#ifndef VFODEFINITION_H
#define VFODEFINITION_H

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
 * This file contains the definition of base class VFODefinition, 
 * which defines the methods needed by the three band VFO.
 */
  
#include <Arduino.h> 

/**
 * This class defines the methods needed by the three band VFO to operate
 * the SI5351 clock chip.
 */
class VFODefinition {
protected:   
   unsigned long frequency;
   unsigned long minFrequency;
   unsigned long maxFrequency;
   boolean       enabled;
   
public:
   /**
    * Constructor 
    * 
    * @param  f     starting frequency of VFO in Hz
    * @param  minf  minimum frequency of VFO in Hz
    * @param  maxf  maximum frequency of VFO in Hz
    * @param  flag  enabled/disabled state of clock. Default enabled.
    */
   VFODefinition(unsigned long f
               , unsigned long minf
               , unsigned long maxf
               , boolean flag= true)
   : frequency(f)
   , minFrequency(minf)
   , maxFrequency(maxf)
   , enabled(flag)
   {}
   
   /**
    * gets vfo current frequency
    * @return vfo current frequency
    */   
   virtual unsigned long getFrequency() {
      return frequency;
   }
   
   /**
    * flips vfo enabled/disabled state 
    */   
   virtual void toggleEnabled() {
      enabled  = !enabled;
   }
   
   /**
    * flips sets enabled/disabled state 
    * @param  flag  new enabled/disabled state of vfo
    */   
   virtual void setEnabled(boolean flag) {
      enabled  = flag;
   }
   
   /**
    * checks vfo enabled/disabled state
    * @return enabled/disabled state of vfo
    */   
   virtual boolean isEnabled() {
      return enabled;
   }
   
   /**
    * increase vfo frequency by a specified amount (in Hz)
    * if increment would cause vfo frequency to exceed the vfo 
    * maximum, sets the vfo to its maximum frequency 
    * @param  freq_delta  amount to increase vfo frequency
    */   
   virtual void increaseFrequency(unsigned long freq_delta) {
      frequency += freq_delta;
      if (frequency > maxFrequency) {
         frequency = maxFrequency;
      }
   }
   
   /**
    * decrease vfo frequency by a specified amount (in Hz)
    * if increment would cause vfo frequency to fall below the vfo 
    * minimum, sets the vfo to its minimum frequency 
    * @param  freq_delta  amount to decrease vfo frequency
    */   
   virtual void decreaseFrequency(unsigned long freq_delta) {
      frequency -= freq_delta;
      if (frequency < minFrequency) {
         frequency = minFrequency;
      }
   }

   // abstract methods
   
   /**
    * abstract method
    * Start vfo running
    */
   virtual void start()                = 0;
   
   /**
    * Stop vfo
    */
   virtual void stop()                 = 0;
   
   /**
    * loads vfo current frequency into the hardware
    */
   virtual void loadFrequency()        = 0;
   
};   
   
#endif // VFODEFINITION_H
