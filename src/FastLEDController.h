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

#include <FastLED.h>

#include "Arduino.h"
#include "LEDController.h"
#include "TemperatureController.h"

#ifndef EEPROM_ADDRESS
#define EEPROM_ADDRESS 4
#endif

#ifndef LED_CONTROLLER_TIMEOUT
#define LED_CONTROLLER_TIMEOUT 30000
#endif

/**
 * The default LEDController. This controller uses the FastLED library to implement the Hardware Lighting effects. Also
 * all RGB values of the LEDs are stored into CRGB arrays which can be used by the FastLED library to show them on the
 * real LED strips. This controller can stores the internal state to EEPROM and support HW playback without USB
 * connection.
 *
 * @see FastLED
 */
class FastLEDController : public LEDController {
	/**
	 * Internal data stored for each Channel. These data are not persistent.
	 */
	struct ChannelData {
		uint8_t ledCount = 0;
		CRGB* leds = nullptr;
		/**
		 * store an array for each color, used for software playback
		 */
		uint8_t* valuesBuffer[3] = {nullptr};
		/**
		 * External temperature value used by this channel for temperature based lighting.
		 */
		uint16_t temp;
		void (*onUpdateCallback)(void) = nullptr;
	};

public:
	/**
	 * Create a new FastLEDController and specify if the EEPROM of the Arduino should be used. See the other contructor
	 * for more details.
	 *
	 * @param useEEPROM specify if the EEPROM should be used
	 */
	FastLEDController(bool useEEPROM);
	/**
	 * Create a new FastLEDController and specify if the EEPROM of the Arduino should be used to store persistent
	 * information like the Hardware Lighting. If enabled, the hardware lighting configured in iCUE works without a USB
	 * connection and even after a restart of the Arduino. Also the the TemperatureController used for temperature
	 * related lighting can be passed here.
	 *
	 * @param temperatureController used for temperature based lighting
	 * @param useEEPROM specify if the EEPROM should be used
	 */
	FastLEDController(TemperatureController* temperatureController, bool useEEPROM);
	~FastLEDController();
	/**
	 * Add a LED array on a channel with a given length. The length define how many LEDs iCUE can control. The actual
	 * length of the array can be longer, but iCUE only writes up to the specified length.
	 *
	 * @param channel the index of the channel
	 * @param leds the array to store the LED data in
	 * @param length the length of the array used by iCUE to write LED data
	 */
	virtual void addLEDs(uint8_t channel, CRGB* leds, uint8_t length);
	/**
	 * Get the LED data array for a channel.
	 *
	 * @param channel the index of the channel
	 * @return the pointer to the LED array or nullptr if there is no array
	 * @see getLEDCount()
	 */
	CRGB* getLEDs(uint8_t channel);
	/**
	 * Get the length of the LED data array.
	 *
	 * @param channel the index of the channel
	 * @return the length of the array
	 * @see getLEDs()
	 */
	uint8_t getLEDCount(uint8_t channel);
	/**
	 * Update the displayed RGB values for the LEDs. This will write to the LED data array of each Channel.
	 * This method does not call {@code FastLED.show()}. This function must be called in loop.
	 *
	 * @return true if the LED data of a channel was updated, false otherwise
	 */
	virtual bool updateLEDs();
	/**
	 * Get the total size of all data stored in EEPROM by this LEDController.
	 *
	 * @return the size in bytes
	 */
	virtual size_t getEEPROMSize();
	/**
	 * Register an update hook, which is executed after a channel has been updated. This can be used to apply
	 * transforamtions to the channel before the data is displayed by FastLED.
	 *
	 * @param channel the channel for which the hook is registered
	 * @param callback the callback, which is executed after the update
	 */
	void onUpdateHook(uint8_t channel, void (*callback)(void));

protected:
	TemperatureController* const temperatureController;

	bool trigger_update = false;

	ChannelData channelData[CHANNEL_NUM];

	unsigned long lastUpdate = 0;
	unsigned long currentUpdate = 0;

	int applySpeed(int duration, const GroupSpeed speed);
	/**
	 * Calculates the index of the current step of the animation.
	 *
	 * @param duration the duration on the animation
	 * @param steps the number of steps of the animation
	 * @return the current step of the animation
	 */
	int animation_step(int duration, int steps);
	/**
	 * Calculates the number of steps of the animation, since the last update of the animation.
	 *
	 * @param duration the duration on the animation
	 * @param steps the number of steps of the animation
	 * @return the number of steps since the last update
	 */
	int animation_step_count(int duration, int steps);

	/**
	 * Generate a rainbow random color from predefined color palette.
	 *
	 * @return a random color
	 */
	CRGB randomColor();

	bool renderRainbowWave(ChannelData& channelData, LEDGroup& group, int groupLedCount);
	bool renderColorShift(ChannelData& channelData, LEDGroup& group, int groupLedCount);
	bool renderColorPulse(ChannelData& channelData, LEDGroup& group, int groupLedCount);
	bool renderColorWave(ChannelData& channelData, LEDGroup& group, int groupLedCount);
	bool renderStatic(ChannelData& channelData, LEDGroup& group, int groupLedCount);
	bool renderTemperature(ChannelData& channelData, LEDGroup& group, int groupLedCount);
	bool renderVisor(ChannelData& channelData, LEDGroup& group, int groupLedCount);
	bool renderMarquee(ChannelData& channelData, LEDGroup& group, int groupLedCount);
	bool renderBlink(ChannelData& channelData, LEDGroup& group, int groupLedCount);
	bool renderSequential(ChannelData& channelData, LEDGroup& group, int groupLedCount);
	bool renderRainbow(ChannelData& channelData, LEDGroup& group, int groupLedCount);

	const bool useEEPROM;
	bool load() override;
	bool save() override;

	virtual void triggerLEDUpdate() override;
	virtual void setLEDExternalTemperature(uint8_t channel, uint16_t temp) override;
	virtual void setLEDColorValues(uint8_t channel, uint8_t color, uint8_t offset, const uint8_t* values,
								   size_t len) override;
	virtual void clearLEDColorValues(uint8_t channel) override;
	virtual uint8_t getLEDAutodetectionResult(uint8_t channel) override;
	/**
	 * This function is called when a timeout occurs.
	 */
	virtual void timeoutAction();
};
