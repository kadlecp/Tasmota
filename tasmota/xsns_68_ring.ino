/*
  xdrv_24_Buzzer.ino - buzzer support for Tasmota

  Copyright (C) 2020  Theo Arends

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifdef USE_RING
/*********************************************************************************************\
 * Ring support
\*********************************************************************************************/

#define XSNS_68            68

/*********************************************************************************************/

#include "Tone.h"

uint8_t ring_initialized = 1;

void RingOff(void)
{
  DigitalWrite(pin[GPIO_BUZZER], LOW);  // Buzzer Off
}

bool RingBeep(uint32_t count, uint32_t delayInMilis)
{
  for (uint32_t i=1; i<=count; i++) {
    for (uint32_t j=1; j<=3; j++){
      for (uint32_t k=1; k<=100; k++) {
        uint32_t freq = 2000;
        if (k%2 == 0) {
          freq = 2500;
        }

        tone(pin[GPIO_BUZZER], freq);
        delay(20);
        noTone(pin[GPIO_BUZZER]);
        digitalWrite(pin[GPIO_BUZZER], HIGH);
      }
      delay(1000);
    }
    delay(delayInMilis);
  }
  digitalWrite(pin[GPIO_BUZZER], LOW);

  AddLog_P2(LOG_LEVEL_DEBUG, PSTR("RNG: Ring triggered with following params: %d(%d)"), count, delayInMilis);

  if (count == 0) {
    RingOff();
  }

  return true;
}

bool RingBeep(uint32_t count)
{
  return RingBeep(count, 0);
}

/*********************************************************************************************/

void RingInit(void)
{
  ring_initialized = 0;
  if (pin[GPIO_BUZZER] < 99) {
    pinMode(pin[GPIO_BUZZER], OUTPUT);
    RingOff();
    ring_initialized = 1;
    AddLog_P2(LOG_LEVEL_DEBUG, PSTR("RNG: Ring initialization successful"));
  } else {
    AddLog_P2(LOG_LEVEL_DEBUG, PSTR("RNG: Initialization failed"));
  }
}

/*********************************************************************************************\
 * Commands
\*********************************************************************************************/

const char kRingCommands[] PROGMEM = "|"  // No prefix
  "Ring" ;

void (* const RingCommand[])(void) PROGMEM = {
  &CmndRing };

void CmndRing(void)
{
  // Ring <number of repetitions>,<duration of beep in mS>
  // All parameters are optional
  //
  // Ring               = Ring 1,0 = Ring sequence once
  // Ring 1             = Ring sequence once
  // Ring 2,2000        = Ring sequence twice with delay in between of 2000ms
  if (XdrvMailbox.data_len > 0) {
    if (XdrvMailbox.payload != 0) {
      uint32_t parm[4] = { 0 };
      uint32_t mode = 0;
      ParseParameters(4, parm);
      RingBeep(parm[0], parm[1]);
    } else {
      RingBeep(0);
    }
  } else {
    RingBeep(1);
  }
  ResponseCmndDone();
}

/*********************************************************************************************\
 * Interface
\*********************************************************************************************/

bool Xsns68(uint8_t function)
{
  bool result = false;
  if (ring_initialized) {
    switch (function) {
      case FUNC_COMMAND_SENSOR:
        if (XSNS_68 == XdrvMailbox.index) {
          result = RingBeep(1);
        }
        break;
      case FUNC_INIT:
        RingInit();
        break;
    }
  }
  return result;
}

#endif  // USE_RING
