#ifndef DEVICEINITIALIZATIONS_H
#define DEVICEINITIALIZATIONS_H

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
 * This file contains functions used to initialize hardware values.
 * 
 * Note that the SI5351 output current is hardcoded below at 2ma. You
 * can set it to higher values up to 8ma, see the documentation 
 * accompanying the Etherkit library.
 */
 
 
#include <Arduino.h> 

/**
 * initialize vfo list values
 */
void setupVFOs()   {     
   vfoList[0] = new si5351_VFODefinition(si5351
                                   , FREQ_VFO_A_DEFAULT
                                   , FREQ_VFO_A_MIN
                                   , FREQ_VFO_A_MAX
                                   , SI5351_PLL_FIXED
                                   , SI5351_CLK0 );
   
   vfoList[1] = new si5351_VFODefinition(si5351
                                   , FREQ_VFO_B_DEFAULT
                                   , FREQ_VFO_B_MIN
                                   , FREQ_VFO_B_MAX
                                   , SI5351_PLL_FIXED
                                   , SI5351_CLK1 );
   
   vfoList[2] = new si5351_VFODefinition(si5351
                                   , FREQ_VFO_C_DEFAULT
                                   , FREQ_VFO_C_MIN
                                   , FREQ_VFO_C_MAX
                                   , SI5351_PLL_FIXED
                                   , SI5351_CLK2 );

   frequency_delta          = FREQ_DELTA_DEFAULT;
   currVFO                  = STARTING_VFO;
   pCurrentVFO              = vfoList[currVFO]; 
   freq_delta_display_time  = 0;
}

/**
 * setup for digital pin input (button presses)
 */
void setupInputPins()   {  
   VFOSelectPin.initialize();  
   FrequencyDeltaSelectPin.initialize();
}

/**
 * setup for rotary encoder (change frequencies)
 */
void setupEncoder()   {                
   // set up encoder pins for interrupts
   pinMode(ENCODER_PIN_A, INPUT); 
   pinMode(ENCODER_PIN_B, INPUT); 
   digitalWrite(ENCODER_PIN_A, HIGH);  // turn on pullup resistor
   digitalWrite(ENCODER_PIN_B, HIGH);  // turn on pullup resistor

   // encoder pin on interrupt 0 (pin 2)
   attachInterrupt(0, encoderPinA_ISR, CHANGE);

   // encoder pin on interrupt 1 (pin 3)
   attachInterrupt(1, encoderPinB_ISR, CHANGE);
}

/**
 * setup for OLED display
 */
void setupDisplay()   {   
   pDisplay = new SSD1306_VFODisplay(vfoList, NUMBER_OF_VFOS);
   pDisplay->showVFOs(frequency_delta, currVFO);
}

/**
 * setup for Si5351 clock board
 */
void setupSI5351()   {                
   si5351.init(SI5351_CRYSTAL_LOAD_8PF, 0);

   // initially all oscillators are off
   si5351.output_enable(SI5351_CLK0, 0);
   si5351.output_enable(SI5351_CLK1, 0);
   si5351.output_enable(SI5351_CLK2, 0);

   // set output to minimum value
   si5351.drive_strength(SI5351_CLK0, SI5351_DRIVE_2MA);
   si5351.drive_strength(SI5351_CLK1, SI5351_DRIVE_2MA);
   si5351.drive_strength(SI5351_CLK2, SI5351_DRIVE_2MA);

   // set  a fixed PLL frequency
   si5351.set_pll(SI5351_PLL_FIXED, SI5351_PLLA);

   // set clocks to initial frequencies
   for (int ii=0; ii<NUMBER_OF_VFOS; ++ii) {
      vfoList[ii]->loadFrequency();
   }

   // enable output on selected vfoList
   vfoList[currVFO]->start();
}

#endif // DEVICEINITIALIZATIONS_H
