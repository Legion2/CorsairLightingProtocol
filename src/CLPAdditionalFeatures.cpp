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
#include "CLPAdditionalFeatures.h"

#include "CLPUtils.h"

bool CLP::shouldReset(const CorsairLightingFirmware* firmware) {
	DeviceID deviceId;
	firmware->getDeviceID(deviceId);
	return CLP::isResetID(deviceId);
}

void CLP::reset(CorsairLightingFirmware* firmware) {
	DeviceID deviceId = {0x00};
	firmware->setDeviceID(deviceId);
}
