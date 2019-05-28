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
#include "FanController.h"

#define bigEndian(a) highByte(a), lowByte(a)
#define convert100To255(a) (a * 2.5546875f)

void FanController::handleFanControl(const Command& command, const CorsairLightingProtocolResponse* response)
{
	switch (command.command)
	{
	case READ_FAN_MASK:
	{
		uint8_t mask[FAN_NUM];
		//TODO
		response->send(mask, sizeof(mask));
		break;
	}
	case READ_FAN_SPEED:
	{
		const uint8_t& fan = command.data[0];
		if (fan >= FAN_NUM) {
			response->sendError();
			return;
		}
		uint16_t speed;
		//TODO
		uint8_t fanData[] = { bigEndian(speed) };
		response->send(fanData, sizeof(fanData));
		break;
	}
	case READ_FAN_POWER:
	{
		//TODO
		break;
	}
	case WRITE_FAN_POWER:
	{
		const uint8_t& fan = command.data[0];
		if (fan >= FAN_NUM) {
			response->sendError();
			return;
		}
		uint8_t percentage = convert100To255(command.data[1]);
		//TODO
		break;
	}
	case WRITE_FAN_SPEED:
	{
		break;
	}
	case WRITE_FAN_CURVE:
	{
		break;
	}
	case WRITE_FAN_EXTERNAL_TEMP:
	{
		break;
	}
	case WRITE_FAN_FORCE_THREE_PIN_MODE:
	{
		break;
	}
	case WRITE_FAN_DETECTION_TYPE:
	{
		break;
	}
	case READ_FAN_DETECTION_TYPE:
	{
		break;
	}
	default:
		response->sendError();
	}
}
