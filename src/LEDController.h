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
 * Defines types and constants of the led part of the protocol
 */

#include "Arduino.h"
#include <FastLED.h>
#include "ILEDController.h"
#include "CLPUtils.h"

#define CHANNEL_NUM 2
#define GROUPS_NUM 6

/**
 * The mode of an LEDChannel. The mode describes how the led lighting is done.
 *
 * @see LEDController#setLEDMode
 */
enum class ChannelMode : byte {
	/** No lighting is active for the channel. The leds will not be updated. */
	Disabled = 0x00,
	/** The Hardware Playback uses lighting effects defined by LEDGroups and LEDController renders the effects themself. This mode works even without an USB connection.  */
	HardwarePlayback = 0x01,
	/** All lighting effects are rendered by iCUE and only the RGB values are transferred via USB to the device. This requires an USB connection. */
	SoftwarePlayback = 0x02
};

bool inline isValidChannelMode(const ChannelMode channelMode) {
	return channelMode == ChannelMode::Disabled || channelMode == ChannelMode::HardwarePlayback || channelMode == ChannelMode::SoftwarePlayback;
}

/**
 * The type of LED Chipset connected to a channel. These are the types implicitly defined by iCUE when doing the lighting setup in iCUE.
 * These type is ignored by the LEDController.
 */
enum class PortType : byte {
	/** WS2812B used by all new Corsair devices */
	WS2812B = 0x01,
	/** UCS1903 Only used for the SP fan */
	UCS1903 = 0x02
};

bool inline isValidPortType(const PortType portType) {
	return portType == PortType::WS2812B || portType == PortType::UCS1903;
}

//LED group mode
#define GROUP_MODE_Rainbow_Wave 0x00
#define GROUP_MODE_Color_Shift 0x01
#define GROUP_MODE_Color_Pulse 0x02
#define GROUP_MODE_Color_Wave 0x03
#define GROUP_MODE_Static 0x04
#define GROUP_MODE_Temperature 0x05
#define GROUP_MODE_Visor 0x06
#define GROUP_MODE_Marquee 0x07
#define GROUP_MODE_Blink 0x08
#define GROUP_MODE_Sequential 0x09
#define GROUP_MODE_Rainbow 0x0A

//LED group speed
#define GROUP_SPEED_HIGH   0x00
#define GROUP_SPEED_MEDIUM 0x01
#define GROUP_SPEED_LOW    0x02

//LED group direction
#define GROUP_DIRECTION_BACKWARD 0x00
#define GROUP_DIRECTION_FORWARD  0x01

//LED group extra
#define GROUP_EXTRA_ALTERNATING 0x00
#define GROUP_EXTRA_RANDOM  0x01

#define GROUP_TEMP_GROUP_EXTERNAL 255

/**
 * A LEDGroup is a contiguous range of leds on a strip. The LEDGroup defines the size, position and lighting effects of the led range.
 */
struct LEDGroup {
	/**
	 * start index of the leds of this group
	 */
	byte ledIndex = 0;
	/**
	 * number of leds in this group
	 */
	byte ledCount = 0;
	byte mode = GROUP_MODE_Rainbow_Wave;
	byte speed = GROUP_SPEED_HIGH;
	byte direction = GROUP_DIRECTION_FORWARD;
	byte extra = 0x00;
	byte tempGroup = GROUP_TEMP_GROUP_EXTERNAL;

	CRGB color1;
	CRGB color2;
	CRGB color3;

	uint16_t temp1;
	uint16_t temp2;
	uint16_t temp3;
};

/**
 * The definition of a Channel.
 */
struct LEDChannel {
	/**
	 * Brightness of the channel in range 0-255.
	 */
	uint8_t brightness = 255;
	ChannelMode mode = ChannelMode::HardwarePlayback;
	uint8_t ledCount = 0;
	PortType ledPortType = PortType::WS2812B;

	LEDGroup groups[GROUPS_NUM];
	uint8_t groupsSet = 0;
};

/**
 * The abstract implemenation of an LEDController. This implementation handles the parsing and interpretation of incoming commands.
 * It also defines the data model to store the all required data from the commands.
 */
class LEDController : public ILEDController {
public:
	virtual void handleLEDControl(const Command& command, const CorsairLightingProtocolResponse* response) override;
	/**
	 * Validates a LEDChannel by checking all constrains on the values. This function should be used after non type-safe operations on a LEDChannel.
	 * 
	 * @param ledChannel the LEDChannel to validate
	 * @return true if the LEDChannel is valid, false otherwise
	 */
	virtual bool isValidLEDChannel(const LEDChannel& ledChannel);
	/**
	 * Validates a LEDGroup by checking all constrains on the values. This function should be used after non type-safe operations on a LEDGroup.
	 * 
	 * @param ledGroup the LEDGroup to validate
	 * @return true if the LEDGroup is valid, false otherwise
	 */
	virtual bool isValidLEDGroup(const LEDGroup& ledGroup);
	/**
	 * Get the data of a Channel from this LEDController.
	 * 
	 * @param channelIndex the index of the channel
	 * @return a reference to the LEDChannel
	 */
	const LEDChannel& getChannel(uint8_t channelIndex);
	/**
	 * Reset all persistent data to default values of the LEDController.
	 */
	virtual void reset();
protected:
	LEDChannel channels[CHANNEL_NUM];
	/**
	 * Indicates that the configuration of the channels has been changed and should be saved
	 */
	bool trigger_save = false;

	/**
	 * Trigger update of the LEDs
	 */
	virtual void triggerLEDUpdate() = 0;
	/**
	 * The led count of the group
	 *
	 * @param channel the channel index
	 * @param group the group index
	 * @return the number of leds in the group
	 */
	virtual uint8_t getLEDStripMask(uint8_t channel, uint8_t group);
	/**
	 *  Set the external temperature for a channel.
	 *
	 * @param channel the channel index
	 * @param temp the temperature in hundredths of a degree Celsius.
	 */
	virtual void setLEDExternalTemperature(uint8_t channel, uint16_t temp) = 0;
	virtual bool setLEDGroup(uint8_t channel, uint8_t groupIndex, LEDGroup& group);
	virtual void setLEDColorValues(uint8_t channel, uint8_t color, uint8_t offset, const uint8_t* values, size_t len) = 0;
	virtual bool setLEDMode(uint8_t channel, ChannelMode mode);
	/**
	 * The brightness of the channel. This only applies to HW lighting.
	 *
	 * @param channel the channel index
	 * @param brightness the brightness in the range 0-255
	 */
	virtual bool setLEDBrightness(uint8_t channel, uint8_t brightness);
	/**
	 * The type of led controller: WS2812B or UCS1903
	 *
	 * @param channel the channel index
	 * @param ledPortType the port type
	 */
	virtual bool setLEDPortType(uint8_t channel, PortType ledPortType);
	virtual void clearLEDColorValues(uint8_t channel) = 0;
	virtual bool clearLEDGroups(uint8_t channel);
	virtual bool save() = 0;
	virtual bool load() = 0;
};
