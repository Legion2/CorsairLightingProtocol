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
#include "FanController.h"
#include "FanControllerStorage.h"
#include "PWMFan.h"
#include "TemperatureController.h"

#ifndef FAN_EXTERNAL_TEMPERATURE_TIMEOUT
/**
 * Milliseconds without a temperature update from iCUE after which a fan curve on an external temperature falls back to
 * the first local temperature sensor, or to full power if there is none. This keeps the radiator fans running when
 * iCUE is not running: during boot, in the BIOS or after a crash.
 */
#define FAN_EXTERNAL_TEMPERATURE_TIMEOUT 10000
#endif

namespace CLP {

/**
 * The default FanController. Fans are controlled with fixed power, fixed speed or a fan curve as set by iCUE; the fan
 * curve is evaluated on the board, so it keeps working without a USB connection when the state is persisted with a
 * FanControllerStorage. Fan curves on a local temperature sensor of the Commander PRO are the reliable choice, a curve
 * on a sensor from the PC only works while iCUE is running.
 */
class SimpleFanController : public FanController {
public:
	/**
	 * @param temperatureController the TemperatureController used to get the temperatures for the fan curves
	 * @param updateRate the time between fan speed updates in ms, also the measuring window of TachoFan
	 * @param storage the storage for the fan states, e.g. FanControllerStorageEEPROM, or nullptr for no persistence
	 */
	SimpleFanController(TemperatureController* temperatureController, uint16_t updateRate,
						FanControllerStorage* storage);
	/**
	 * Add a fan to the controller.
	 *
	 * @param index the index of the fan port as shown in iCUE, 0-5
	 * @param fan the fan object
	 */
	void addFan(uint8_t index, PWMFan* fan);
	/**
	 * Load the fan states from the storage and initialise the fans. Call it from setup() after all fans were added;
	 * updateFans() calls it on the first update if it was not called before.
	 */
	void begin();
	/**
	 * Update the fan speeds based on the temperatures and the commands received. This function must be called in
	 * loop.
	 *
	 * @return true if the fans were updated in this call
	 */
	virtual bool updateFans();
	/**
	 * Check that a FanData contains only values the controller can handle, e.g. after loading it from a storage.
	 *
	 * @param data the data to check
	 * @return true if the data is valid
	 */
	static bool isValidFanData(const FanData& data);

protected:
	virtual uint16_t getFanSpeed(uint8_t fan) override;
	virtual void setFanSpeed(uint8_t fan, uint16_t speed) override;
	virtual uint8_t getFanPower(uint8_t fan) override;
	virtual void setFanPower(uint8_t fan, uint8_t percentage) override;
	virtual void setFanCurve(uint8_t fan, uint8_t group, FanCurve& fanCurve) override;
	virtual void setFanExternalTemperature(uint8_t fan, uint16_t temp) override;
	virtual void setFanForce3PinMode(bool flag) override;
	virtual FanDetectionType getFanDetectionType(uint8_t fan) override;
	virtual void setFanDetectionType(uint8_t fan, FanDetectionType type) override;
	virtual FanMask detectFan(uint8_t fan) override;

	/**
	 * Calculate the speed of a fan from its curve and the temperature of its temperature group.
	 *
	 * @param fan the index of the fan
	 * @param speed receives the speed in RPM
	 * @return true if a temperature was available, false if the fan must run at full power
	 */
	bool speedFromCurve(uint8_t fan, uint16_t& speed);
	bool load();
	bool save();

	TemperatureController* const temperatureController;
	FanControllerStorage* const storage;
	PWMFan* fans[FAN_NUM] = {nullptr};
	bool force3PinMode = false;
	FanData fanData[FAN_NUM];
	uint16_t externalTemp[FAN_NUM] = {0};
	unsigned long externalTempUpdate = 0;
	const uint16_t updateRate;
	/**
	 * Indicates that the configuration of the fans has been changed and should be saved.
	 */
	bool triggerSave = false;
	bool started = false;
	unsigned long lastUpdate = 0;
};

}  // namespace CLP
