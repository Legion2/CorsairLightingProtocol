/*
   Copyright 2019 Leon Kiefer

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

#include "Arduino.h"
#include "CorsairLightingFirmware.h"

namespace CLP {
/**
 * Check if the device should be reseted. The check is based on the DeviceID from the firmware.
 *
 * @param firmware the firmware used by this device
 * @return if the device should be reset
 */
bool shouldReset(const CorsairLightingFirmware* firmware);

/**
 * Reset the DeviceID of the firmware.
 *
 * @param firmware reset this firmware
 */
void reset(CorsairLightingFirmware* firmware);
}  // namespace CLP
