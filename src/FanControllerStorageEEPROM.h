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
#pragma once

#if defined(ARDUINO_ARCH_AVR)

#include "FanControllerStorage.h"
#include "FastLEDControllerStorageEEPROM.h"
#include "LEDController.h"

#ifndef EEPROM_ADDRESS_FAN_CONTROLLER
/**
 * The default location follows the DeviceID and the LED channels of FastLEDControllerStorageEEPROM.
 */
#define EEPROM_ADDRESS_FAN_CONTROLLER (EEPROM_ADDRESS + sizeof(LEDChannel) * CHANNEL_NUM)
#endif

namespace CLP {

/**
 * Stores the fan port states in the EEPROM behind a magic byte and a format version. Reading a blank EEPROM or the
 * data of another format version fails, so the fan controller falls back to safe defaults instead of running the
 * fans with garbage settings.
 */
class FanControllerStorageEEPROM : public FanControllerStorage {
public:
	/**
	 * @param address the EEPROM address of the first byte
	 */
	FanControllerStorageEEPROM(uint16_t address = EEPROM_ADDRESS_FAN_CONTROLLER);
	virtual bool load(const uint8_t index, FanData& fan) override;
	virtual bool save(const uint8_t index, const FanData& fan) override;
	/**
	 * Get the total size of all data stored in EEPROM by this storage.
	 *
	 * @return the size in bytes
	 */
	size_t getEEPROMSize() const;

protected:
	const uint16_t address;
	static const uint8_t MAGIC = 0xCF;
	static const uint8_t VERSION = 1;

	bool isHeaderValid() const;
	void writeHeader() const;
};

}  // namespace CLP

#endif
