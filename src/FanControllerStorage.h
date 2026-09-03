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

#include "Arduino.h"
#include "FanController.h"

namespace CLP {

/**
 * How the fan controller decides the power of a fan.
 */
enum class FanControlMode : uint8_t {
	/** Run at a fixed power set by iCUE. */
	FixedPower = 0,
	/** Run at a fixed speed set by iCUE, mapped to power by the fan. */
	FixedRpm = 1,
	/** Follow the fan curve set by iCUE using the temperature of the fan's temperature group. */
	Curve = 2
};

bool isValidFanControlMode(const FanControlMode mode);

/**
 * The persistent state of one fan port. This is the format stored by FanControllerStorage implementations.
 */
struct FanData {
	FanControlMode mode = FanControlMode::FixedPower;
	/**
	 * Power in the range 0-255. Until iCUE configures the fan it runs at 50%, which is safe for a radiator.
	 */
	uint8_t power = 128;
	uint16_t speed = 0;
	FanDetectionType detectionType = FanDetectionType::Auto;
	/**
	 * The temperature sensor the fan curve uses, or FAN_CURVE_TEMP_GROUP_EXTERNAL for a temperature sent by iCUE.
	 */
	uint8_t tempGroup = FAN_CURVE_TEMP_GROUP_EXTERNAL;
	FanCurve fanCurve = {};
};

/**
 * Persistent storage for the state of the fan ports.
 */
class FanControllerStorage {
public:
	/**
	 * Load the state of a fan port.
	 *
	 * @param index the index of the fan port
	 * @param fan receives the state
	 * @return true if valid data was loaded, false if the storage is empty or from another version
	 */
	virtual bool load(const uint8_t index, FanData& fan) = 0;
	/**
	 * Save the state of a fan port.
	 *
	 * @param index the index of the fan port
	 * @param fan the state to save
	 * @return true if the data was saved
	 */
	virtual bool save(const uint8_t index, const FanData& fan) = 0;
};

}  // namespace CLP
