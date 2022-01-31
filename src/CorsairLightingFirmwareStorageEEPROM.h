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
#pragma once

#if defined(ARDUINO_ARCH_AVR)

#include "CorsairLightingFirmware.h"

#ifndef EEPROM_ADDRESS_DEVICE_ID
#define EEPROM_ADDRESS_DEVICE_ID 0
#endif

class CorsairLightingFirmwareStorageEEPROM : public CorsairLightingFirmwareStorage {
public:
	virtual void loadDeviceID(DeviceID& deviceID) const override;
	virtual void saveDeviceID(const DeviceID& deviceID) override;
};

#endif
