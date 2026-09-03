/*
   Copyright 2026 Thomas Neumann

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
#include "FanControllerStorageEEPROM.h"

#if defined(ARDUINO_ARCH_AVR)

#include <EEPROM.h>

#define HEADER_SIZE 2

CLP::FanControllerStorageEEPROM::FanControllerStorageEEPROM(uint16_t address) : address(address) {}

bool CLP::FanControllerStorageEEPROM::isHeaderValid() const {
	return EEPROM.read(address) == MAGIC && EEPROM.read(address + 1) == VERSION;
}

void CLP::FanControllerStorageEEPROM::writeHeader() const {
	EEPROM.update(address, MAGIC);
	EEPROM.update(address + 1, VERSION);
}

bool CLP::FanControllerStorageEEPROM::load(const uint8_t index, FanData& fan) {
	if (index >= FAN_NUM || !isHeaderValid()) {
		return false;
	}
	EEPROM.get(address + HEADER_SIZE + index * sizeof(FanData), fan);
	return true;
}

bool CLP::FanControllerStorageEEPROM::save(const uint8_t index, const FanData& fan) {
	if (index >= FAN_NUM) {
		return false;
	}
	if (!isHeaderValid()) {
		// a blank EEPROM contains 0xFF: initialise every port before the header makes them valid
		for (uint8_t i = 0; i < FAN_NUM; i++) {
			EEPROM.put(address + HEADER_SIZE + i * sizeof(FanData), FanData());
		}
		writeHeader();
	}
	EEPROM.put(address + HEADER_SIZE + index * sizeof(FanData), fan);
	return true;
}

size_t CLP::FanControllerStorageEEPROM::getEEPROMSize() const { return HEADER_SIZE + sizeof(FanData) * FAN_NUM; }

#endif
