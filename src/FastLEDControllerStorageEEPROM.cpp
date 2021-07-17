/*
   Copyright 2021 Leon Kiefer

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

	   http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/
#include "FastLEDControllerStorageEEPROM.h"

#if defined(ARDUINO_ARCH_AVR)

#include <EEPROM.h>

bool FastLEDControllerStorageEEPROM::load(const int index, LEDChannel& channel) {
	EEPROM.get(EEPROM_ADDRESS + (sizeof(LEDChannel) * index), channel);
	return true;
}

bool FastLEDControllerStorageEEPROM::save(const int index, const LEDChannel& channel) {
#ifdef DEBUG
	Serial.println(F("Save to EEPROM."));
#endif
	EEPROM.put(EEPROM_ADDRESS + (sizeof(LEDChannel) * index), channel);
	return true;
}

size_t FastLEDControllerStorageEEPROM::getEEPROMSize() { return sizeof(LEDChannel) * CHANNEL_NUM; }

#endif
