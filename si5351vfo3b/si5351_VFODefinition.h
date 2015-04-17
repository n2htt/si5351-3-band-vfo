#ifndef SI5351_VFODEFINITION_H
#define SI5351_VFODEFINITION_H

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
 * This file contains class definitions for si5351_VFODefinition. 
 * This class implements the methods defined in base class
 * VFODefinition, using the Etherkit SI5351 library.
 */
 
#include <Arduino.h> 
#include <si5351.h>

#include "VFODefinition.h"

/**
 * This class mplements the methods defined in base class
 * VFODefinition, using the Etherkit SI5351 library.
 */
class si5351_VFODefinition : public VFODefinition {
protected:   
   
   /**
    * Stores PLL selection for this vfo
    */
   unsigned long long si5351PLL;
   
   /**
    * Stores clock selection for this vfo
    */
   si5351_clock       si5351Clock;
   
   /**
    * reference to extenally defined SI5351 object
    * which runs this vfo
    */
   Si5351             &si5351;
   
   
public:
   
   /**
    * Constructor 
    * 
    * @param  f     starting frequency of VFO in Hz
    * @param  minf  minimum frequency of VFO in Hz
    * @param  maxf  maximum frequency of VFO in Hz
    * @param  pll   SI5351 PLL selection. See SI5351 library documentation.
    * @param  clock SI5351 clock selection. See SI5351 library documentation.
    * @param  flag  enabled/disabled state of clock. Default enabled.
    */
   si5351_VFODefinition(Si5351  &device
                      , unsigned long f
                      , unsigned long minf
                      , unsigned long maxf
                      , unsigned long long pll
                      , si5351_clock clock
                      , boolean flag= true)
   : si5351(device)
   , VFODefinition(f,minf,maxf,flag)
   , si5351PLL(pll)
   , si5351Clock(clock)
  {}
   
   /**
    * Start clock running
    * Respects the vfo enabled flag, will not start clock
    * if vfo is marked disabled.
    */
   virtual void start() {
      si5351.output_enable(si5351Clock, (enabled)?1:0);   
   }
   
   /**
    * Stop clock
    */
   virtual void stop()  {
      si5351.output_enable(si5351Clock, 0);   
   }
   
   /**
    * loads vfo current frequency into the clock
    * takes effect immediately
    */
   virtual void loadFrequency()  {
      si5351.set_freq(((unsigned long long)frequency)*SI5351_FREQ_MULT, si5351PLL, si5351Clock);
   }
};

#endif // SI5351_VFODEFINITION_H

