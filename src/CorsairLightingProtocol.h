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

/**
 * @file
 * The central include file for CorsairLightingProtocol.
 */
#include "CLPAdditionalFeatures.h"
#include "CLPUtils.h"
#include "CorsairLightingFirmware.h"
#include "CorsairLightingFirmwareStorageEEPROM.h"
#include "CorsairLightingFirmwareStorageStatic.h"
#include "CorsairLightingProtocolConstants.h"
#include "CorsairLightingProtocolController.h"
#include "CorsairLightingProtocolHID.h"
#include "CorsairLightingProtocolResponse.h"
#include "CorsairLightingProtocolSerial.h"
#include "CorsairLightingProtocolTinyUSBHID.h"
#include "FanController.h"
#include "FastLEDController.h"
#include "FastLEDControllerStorage.h"
#include "FastLEDControllerStorageEEPROM.h"
#include "FastLEDControllerUtils.h"
#include "IFanController.h"
#include "ILEDController.h"
#include "ITemperatureController.h"
#include "LEDController.h"
#include "TemperatureController.h"
