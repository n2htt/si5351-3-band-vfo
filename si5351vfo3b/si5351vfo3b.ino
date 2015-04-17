/*! \mainpage si5351 3-band VFO
 *
 * This sketch implements a three band VFO using the SI5351a 
 * breakout board from Etherkit. 
 *  Features include:
 *  - Operation on a single frequency at a time, push button selected
 *  - Frequency adjustment by rotary encoder. Rate of adjustment is 
 *    push button adjustable in 10 to 10000 Hz increments.
 *  - Individual VFOs can be toggled on/off line by a long press of
 *    the selection button.
 *  - All VFOs can be toggled off at once by a long press on the 
 *    the frequency increment button.
 *  - OLED display shows currently selected VFO, VFO status, VFO 
 *    frequency. The display header is customizable, and shows 
 *    current frequency increment.
 *  - Choose your favorite band and frequencies as default start up
 *    values.
 */
 
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
 * This sketch implements a three band VFO using the SI5351a 
 * breakout board from Etherkit. It should run on any AT328 
 * Arduino. Required resources are:
 *  - 32k program memory
 *  - 1k variable space
 *  - 2 pins supporting external hardware interrupts
 *  - 2 digital input pins
 *  - I2C bus support for SI5351 and an OLED 128x64 pixel display (0.96")
 *
 * The prototype was tested on:
 *  - Arduino Mega 2560
 *  - Etherkit SI5351a breakout board
 *  - Bournes mechanical encoder, 24 ppr with momentary contact switch
 *  - SPST momentary pushbutton switch
 *  - SDD1306 I2C OLED display
 *
 * The macros immediately following this description configure all the 
 * hardware options for the VFO. Configurable items include:
 *  - Hardware pin assignments
 *  - VFO frequency settings
 *  - Number of VFOs
 *  - Range of frequency increments
 *  - Other timing parameters
 *
 * See the include files for other configurable parameters.
 *
 * Libraries used in this sketch are:
 *  - DigitalPin       - object representation of a digital pin (supplied)
 *  - DigitalPulse     - object that manages short/long press 
 *                       determination (supplied) 
 *  - Adafruit SSD1306 - interface for the SDD1306 I2C OLED display. This is 
 *                       not supplied, but is available here:
 *
 *  - Etherkit SI5351  - interface for the SI5351a clock chip, 
 *                       not supplied but available here:
 *
 * Two object wrappers are supplied for the SSD1306 display, and 
 * the SI5351. Each wrapper is constructed with a generic 
 * abstract base class that defines the methods used by the sketch, 
 * and then a hardware specific class that uses the appropriate third 
 * party library to implement the methods.
 * This architecture will make it easier to adapt the sketch to use
 * other harware or libraries in the future. The wrappers classes are:
 *
 * Base class       Implementation class
 * VFODefinition    si5351_VFODefinition
 * VFODisplay       SSD1306_VFODisplay
 *
 * Some notes about program structure:
 * 1. I would have normally included the hardware specific library
 *    includes in the include files containing the hardware specific
 *    classes, but the Arduino IDE did not find them when I did. For
 *    that reason you will see them below in this file, above the local 
 *    includes for hardware specific classes.
 * 2. The include files FrequencySelection.h and DeviceInitializations.h
 *    contain sections of in-line c++ code that I moved to separate
 *    files for better readability. Using the include mechanism allows
 *    inserting this code in the main sketch in the proper sequence.
 * 3. The embedded comment blocks in all of the files of this sketch
 *    are compatible with documentation generators like javadoc
 *    and doxygen.
 */
 
#include <Wire.h>
#include <SPI.h>
#include <si5351.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include <DigitalPin.h>
#include <DigitalPulse.h>

#include "si5351_VFODefinition.h"
#include "SSD1306_VFODisplay.h"

/**
 * hardware pin definitions
 */
#define ENCODER_PIN_A                     2
#define ENCODER_PIN_B                     3
#define VFO_SELECTOR_PIN                  4
#define FRQ_DELTA_SELECTOR_PIN            5

/**
 * frequency change constants
 */
#define FREQ_DELTA_DEFAULT              100
#define FREQ_DELTA_MAX                10000
#define FREQ_DELTA_MIN                   10
#define FREQ_DELTA_MULT                  10
#define NUMBER_OF_VFOS                    3
#define STARTING_VFO                      1

/**
 * vfo frequency limits
 */
#define FREQ_VFO_A_DEFAULT          3560000
#define FREQ_VFO_A_MIN              3500000
#define FREQ_VFO_A_MAX              4000000

#define FREQ_VFO_B_DEFAULT          7055000
#define FREQ_VFO_B_MIN              7000000
#define FREQ_VFO_B_MAX              7300000

#define FREQ_VFO_C_DEFAULT         10120000
#define FREQ_VFO_C_MIN             10100000
#define FREQ_VFO_C_MAX             10150000

/**
 * delay times
 */
#define BUTTON_DEBOUNCE_WAIT_MILS        10
#define SETUP_DELAY_MILS                  5
#define LOOP_DELAY_MILS                  50
#define FREQ_DELTA_LATENCY_MILS         900

/**
 * Si5351 clock board object
 */
Si5351 si5351;

/**
 * vfo list and variables controlling frequency
 */
VFODefinition *vfoList[NUMBER_OF_VFOS];

int currVFO;
VFODefinition *pCurrentVFO;
long frequency_delta;
int freq_delta_display_time;

/**
 * display object
 */
VFODisplay *pDisplay;

/**
 * digital pins (reading button presses)
 */
DigitalInputPin VFOSelectPin( VFO_SELECTOR_PIN
                                , INPUT_PULLUP
                                , BUTTON_DEBOUNCE_WAIT_MILS
                                , DIGITAL_PIN_INIT_STATE_HIGH
                                , DIGITAL_PIN_INVERTING);

DigitalInputPin FrequencyDeltaSelectPin( FRQ_DELTA_SELECTOR_PIN
                                       , INPUT_PULLUP
                                       , BUTTON_DEBOUNCE_WAIT_MILS
                                       , DIGITAL_PIN_INIT_STATE_HIGH
                                       , DIGITAL_PIN_INVERTING);


/**
 * handler code for frequency selection via rotary encoder
 */
#include "FrequencySelection.h"

/**
 * initialization code for hardware devices
 */
#include "DeviceInitializations.h"

/**
 * sketch setup
 */
void setup()   { 
   // initialize serial port at a relatively languid rate               
   Serial.begin(9600);
   
   // initialize digital input pins for button presses               
   setupInputPins();
   
   // setup encoder for frequency adjustments               
   setupEncoder();
   
   // initialize vfo list                
   setupVFOs();
   
   // light up display               
   setupDisplay();   
   
   // turn on vfos               
   setupSI5351();   

   delay(SETUP_DELAY_MILS);
}

/**
 * operating loop
 */
void loop() {
   // selecting output vfo is first priority
   if (VFOSelectPin.readInputPulseMode()) {
      int mode = VFOSelectPin.getCurrentPinMode();
      switch (mode) {
         case PIN_MODE_SHORT_PULSE:
            // short pulse - switch to next vfoList
            // - turn off current clock
            pCurrentVFO->stop();
            
            // increment the selected vfoList
            currVFO = (++currVFO) % NUMBER_OF_VFOS; 
            pCurrentVFO = vfoList[currVFO]; 
            
            // turn on new clock if not disabled
            pCurrentVFO->start();
 
            // repaint the vfoList display
            pDisplay->showVFOs(frequency_delta, currVFO, vfoList);

            // reset pin
            VFOSelectPin.setCurrentPinMode(PIN_MODE_IDLE);
            break;

         case PIN_MODE_LONG_PULSE:
            // long pulse - toggle vfoList enable flag
            pCurrentVFO->toggleEnabled(); 
            
            // turn on new clock if not disabled
            pCurrentVFO->start();

            // repaint the vfoList display
            pDisplay->showVFOs(frequency_delta, currVFO, vfoList);

            // reset pin
            VFOSelectPin.setCurrentPinMode(PIN_MODE_IDLE);
            break;
      }
   }

   // then we can check to see if frequency delta needs to change
   if (FrequencyDeltaSelectPin.readInputPulseMode()) {
      int mode = FrequencyDeltaSelectPin.getCurrentPinMode();
      switch (mode) {
         case PIN_MODE_SHORT_PULSE:
            // short pulse - change frequency delta
            frequency_delta *= FREQ_DELTA_MULT;
            if (frequency_delta > FREQ_DELTA_MAX) {
               frequency_delta = FREQ_DELTA_MIN;
            }

            // take over display to show new frequency delta
            pDisplay->showFreqDeltaDisplay(frequency_delta);
            freq_delta_display_time = FREQ_DELTA_LATENCY_MILS;

            // reset pin
            FrequencyDeltaSelectPin.setCurrentPinMode(PIN_MODE_IDLE);
            break;

         case PIN_MODE_LONG_PULSE:
            // long pulse - disable all VFOs
            
            for (int ii=0; ii<NUMBER_OF_VFOS; ++ii) {
               // set band enable flag off
               vfoList[ii]->setEnabled(false); 
            
               // - turn off current clock
               vfoList[ii]->stop();
            }

            // repaint the vfoList display
            pDisplay->showVFOs(frequency_delta, currVFO, vfoList);
            
            // reset pin
            FrequencyDeltaSelectPin.setCurrentPinMode(PIN_MODE_IDLE);
            break;
      }
   }
  
   // check to see if freq delta display needs to be overwritten
   if (freq_delta_display_time > 0) {
      // update the display time for the vfoList select pin
      freq_delta_display_time = (freq_delta_display_time > LOOP_DELAY_MILS)
                              ? (freq_delta_display_time -= LOOP_DELAY_MILS)
                              : 0;

      // repaint the vfoList display if freq display time exhausted
      if (freq_delta_display_time <= 0) {
         pDisplay->showVFOs(frequency_delta, currVFO, vfoList);
      }
   }
  
   // re-display frequencies if encoder has made changes
   if (updateSelectedFrequencyValue()) {
      pDisplay->showVFOs(frequency_delta, currVFO, vfoList);
   }

   delay(LOOP_DELAY_MILS);
}
