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

// CLP_DEBUG:      0=Off, 1=Error, 2=Warning, 3=Info, 4=Data (Only for TinyUSB)
// CLP_DEBUG_BAUD: Setting too low will severely affect performance depending on debug level;
//                 2000000 needed when CLP_DEBUG is 4
#define CLP_DEBUG 0
#define CLP_DEBUG_PORT Serial1
#define CLP_DEBUG_BAUD 115200

#define COMMAND_SIZE 64
#define RESPONSE_SIZE 16

#define READ_STATUS 0x01
#define READ_FIRMWARE_VERSION 0x02
#define READ_DEVICE_ID 0x03
#define WRITE_DEVICE_ID 0x04
#define START_FIRMWARE_UPDATE 0x05
#define READ_BOOTLOADER_VERSION 0x06
#define WRITE_TEST_FLAG 0x07

#define READ_TEMPERATURE_MASK 0x10
#define READ_TEMPERATURE_VALUE 0x11
#define READ_VOLTAGE_VALUE 0x12

#define READ_FAN_MASK 0x20
#define READ_FAN_SPEED 0x21
#define READ_FAN_POWER 0x22
#define WRITE_FAN_POWER 0x23
#define WRITE_FAN_SPEED 0x24
#define WRITE_FAN_CURVE 0x25
#define WRITE_FAN_EXTERNAL_TEMP 0x26
#define WRITE_FAN_FORCE_THREE_PIN_MODE 0x27
#define WRITE_FAN_DETECTION_TYPE 0x28
#define READ_FAN_DETECTION_TYPE 0x29

#define READ_LED_STRIP_MASK 0x30
#define WRITE_LED_RGB_VALUE 0x31
#define WRITE_LED_COLOR_VALUES 0x32
#define WRITE_LED_TRIGGER 0x33
#define WRITE_LED_CLEAR 0x34
#define WRITE_LED_GROUP_SET 0x35
#define WRITE_LED_EXTERNAL_TEMP 0x36
#define WRITE_LED_GROUPS_CLEAR 0x37
#define WRITE_LED_MODE 0x38
#define WRITE_LED_BRIGHTNESS 0x39
#define WRITE_LED_COUNT 0x3A
#define WRITE_LED_PORT_TYPE 0x3B
#define WRITE_LED_START_AUTODETECTION 0x3C
#define READ_LED_AUTODETECTION_RESULTS 0x3D

#define PROTOCOL_RESPONSE_OK 0x00
#define PROTOCOL_RESPONSE_ERROR 0x01
#define PROTOCOL_STATUS_OK 0x00
#define PROTOCOL_STATUS_ERROR 0xFF

#ifndef SERIAL_NUMBER
#define SERIAL_NUMBER "FB66DF55421900F5"
#endif

#define CORSAIR_MANUFACTURER "Corsair"
#define CORSAIR_VID 0x1B1C
#define CORSAIR_LNP_PRODUCT "Lighting Node PRO"
#define CORSAIR_LNP_PID 0x0C0B
#define CORSAIR_CP_PRODUCT "Commander PRO"
#define CORSAIR_CP_PID 0x0C10
#define CORSAIR_LNC_PRODUCT "Lighting Node CORE"
#define CORSAIR_LNC_PID 0x0C1A
#define CORSAIR_SLC_PRODUCT "Smart Lighting Controller"
#define CORSAIR_SLC_PID 0x0C1E
#define CORSAIR_SLT_PRODUCT "Smart Lighting Towers"
#define CORSAIR_SLT_PID 0x0C23
#define CORSAIR_CC_PRODUCT "CORSAIR iCUE Commander CORE"
#define CORSAIR_CC_PID 0x0C1C

typedef enum {
	CORSAIR_LIGHTING_NODE_PRO = 0,
	CORSAIR_COMMANDER_PRO,
	CORSAIR_LIGHTING_NODE_CORE,
	CORSAIR_SMART_LIGHTING_CONTROLLER,
	CORSAIR_SMART_LIGHTING_TOWERS,
	CORSAIR_COMMANDER_CORE  // Currently not functional
} corsair_product_enum_t;

struct Command {
	union {
		struct {
			uint8_t command;
			uint8_t data[COMMAND_SIZE - 1];
		};
		uint8_t raw[COMMAND_SIZE];
	};
};
