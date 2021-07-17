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
#include "CorsairLightingFirmwareStorageStatic.h"

CorsairLightingFirmwareStorageStatic::CorsairLightingFirmwareStorageStatic(DeviceID& deviceID) : deviceID(deviceID) {}

void CorsairLightingFirmwareStorageStatic::loadDeviceID(DeviceID& deviceID) const { deviceID = this->deviceID; }
void CorsairLightingFirmwareStorageStatic::saveDeviceID(const DeviceID& deviceID) {
	// DeviceID can not be updated
}
