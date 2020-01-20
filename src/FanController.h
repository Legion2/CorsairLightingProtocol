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
 * Defines types and constants of the fan protocol
 */

#include "Arduino.h"
#include "IFanController.h"

#define FAN_NUM 6

/**
 * The actual state of a fan port.
 *
 * @see FanDetectionType
 */
enum class FanMask : byte {
	/** No fan connected */
	Disconnected = 0x00,
	/** A three pin fan is connected */
	ThreePin = 0x01,
	/** A four pin fan is connected */
	FourPin = 0x02
};

bool isValidFanMask(const FanMask fanMask);

/**
 * The virtual state of a fan port.
 *
 * @see FanMask
 */
enum class FanDetectionType : byte {
	/** Auto detect the type of fan which is connected */
	Auto = 0x00,
	/** A three pin fan is connected */
	ThreePin = 0x01,
	/** A four pin fan is connected */
	FourPin = 0x02,
	/** No fan connected */
	Disconnected = 0x03
};

bool isValidFanDetectionType(const FanDetectionType type);

#define FAN_FORCE_THREE_PIN_MODE_ON 0x01
#define FAN_FORCE_THREE_PIN_MODE_OFF 0x00

#define FAN_CURVE_POINTS_NUM 6

#define FAN_CURVE_TEMP_GROUP_EXTERNAL 255

/**
 * The definition of a fan curve to control fan speeds depending on the temperature.
 * The fan curve is a mapping of temperature values to fan speed.
 */
struct FanCurve {
	/**
	 * The temperatures in hundredths of a degree Celsius.
	 */
	uint16_t temperatures[FAN_CURVE_POINTS_NUM];
	/**
	 * The fan speeds in RPM.
	 */
	uint16_t rpms[FAN_CURVE_POINTS_NUM];
};

/**
 * The abstract implementation of IFanController. This implementation handles the parsing and interpretation of incoming
 * commands.
 */
class FanController : public IFanController {
public:
	virtual void handleFanControl(const Command& command, const CorsairLightingProtocolResponse* response) override;

protected:
	/**
	 * Get the fan speed.
	 *
	 * @param fan index of the fan
	 * @return fan speed in RPM.
	 */
	virtual uint16_t getFanSpeed(uint8_t fan) = 0;
	/**
	 * Set the fan speed to a fixed value.
	 *
	 * @param fan index of the fan
	 * @param speed fan speed in RPM.
	 */
	virtual void setFanSpeed(uint8_t fan, uint16_t speed) = 0;
	/**
	 * Get the power percentage of a fan.
	 *
	 * @param fan index of the fan
	 * @return power percentage, in range 0-255 with 255 mean 100%.
	 */
	virtual uint8_t getFanPower(uint8_t fan) = 0;
	/**
	 * @param fan index of the fan
	 * @param percentage 255 mean 100%.
	 */
	virtual void setFanPower(uint8_t fan, uint8_t percentage) = 0;
	/**
	 * Set a FanCure for a fan.
	 *
	 * @param fan index of the fan
	 * @param group the temperature group used for getting the temperature for this fan
	 * @param fanCurve the fan curve data
	 */
	virtual void setFanCurve(uint8_t fan, uint8_t group, FanCurve& fanCurve) = 0;
	/**
	 * Set the external temperature for a fan. The external temperature is provied by the iCUE.
	 *
	 * @param fan index of the fan
	 * @param temp The temperature in hundredths of a degree Celsius.
	 */
	virtual void setFanExternalTemperature(uint8_t fan, uint16_t temp) = 0;
	/**
	 * TODO not sure what this does, currently unused
	 *
	 * @param flag the value
	 */
	virtual void setFanForce3PinMode(bool flag) = 0;
	/**
	 * Get the fan DetectionType for a fan.
	 *
	 * @param fan index of the fan
	 * @return the FanDetectionType
	 */
	virtual FanDetectionType getFanDetectionType(uint8_t fan) = 0;
	/**
	 * Set the fan DetectionType for a fan.
	 *
	 * @param fan index of the fan
	 * @param type the FanDetectionType
	 */
	virtual void setFanDetectionType(uint8_t fan, FanDetectionType type) = 0;
};
