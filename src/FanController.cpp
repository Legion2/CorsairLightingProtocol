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
#include "CLPUtils.h"

void FanController::handleFanControl(const Command& command, const CorsairLightingProtocolResponse* response)
{
	switch (command.command)
	{
	case READ_FAN_MASK:
	{
		uint8_t mask[FAN_NUM];
		for (uint8_t i = 0; i < FAN_NUM; i++) {
			switch (getFanDetectionType(i))
			{
			case FAN_DETECTION_TYPE_AUTO:
				mask[i] = FAN_MASK_DISCONNECTED;//TODO
				break;
			case FAN_DETECTION_TYPE_3PIN:
				mask[i] = FAN_MASK_3PIN;
				break;
			case FAN_DETECTION_TYPE_4PIN:
				mask[i] = FAN_MASK_4PIN;
				break;
			case FAN_DETECTION_TYPE_DISCONNECTED:
				mask[i] = FAN_MASK_DISCONNECTED;
			}
		}
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
		uint16_t speed = getFanSpeed(fan);
		uint8_t fanData[] = { toBigEndian(speed) };
		response->send(fanData, sizeof(fanData));
		break;
	}
	case READ_FAN_POWER:
	{
		const uint8_t& fan = command.data[0];
		if (fan >= FAN_NUM) {
			response->sendError();
			return;
		}
		uint8_t power = getFanPower(fan);
		uint8_t powerData[] = { convert255To100(power) };
		response->send(powerData, sizeof(powerData));
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
		setFanPower(fan, percentage);
		response->send(NULL, 0);
		break;
	}
	case WRITE_FAN_SPEED:
	{
		const uint8_t& fan = command.data[0];
		if (fan >= FAN_NUM) {
			response->sendError();
			return;
		}
		uint16_t speed = fromBigEndian(command.data[1], command.data[2]);
		setFanSpeed(fan, speed);
		response->send(NULL, 0);
		break;
	}
	case WRITE_FAN_CURVE:
	{
		const uint8_t& fan = command.data[0];
		if (fan >= FAN_NUM) {
			response->sendError();
			return;
		}
		const uint8_t& group = command.data[1];
		FanCurve fanCurve;
		for (uint8_t i = 0; i < FAN_CURVE_POINTS_NUM; i++) {
			fanCurve.temperatures[i] = fromBigEndian(command.data[2 + i * 2], command.data[3 + i * 2]);
			fanCurve.rpms[i] = fromBigEndian(command.data[14 + i * 2], command.data[15 + i * 2]);
		}
		setFanCurve(fan, group, fanCurve);
		response->send(NULL, 0);
		break;
	}
	case WRITE_FAN_EXTERNAL_TEMP:
	{
		const uint8_t& fan = command.data[0];
		if (fan >= FAN_NUM) {
			response->sendError();
			return;
		}
		uint16_t temp = fromBigEndian(command.data[1], command.data[2]);
		setFanExternalTemperature(fan, temp);
		response->send(NULL, 0);
		break;
	}
	case WRITE_FAN_FORCE_THREE_PIN_MODE:
	{
		if (command.data[0] == FAN_FORCE_THREE_PIN_MODE_ON) {
			setFanForce3PinMode(true);
		}
		else if (command.data[0] == FAN_FORCE_THREE_PIN_MODE_OFF) {
			setFanForce3PinMode(false);
		}
		else {
			response->sendError();
			return;
		}
		
		response->send(NULL, 0);
		break;
	}
	case WRITE_FAN_DETECTION_TYPE:
	{
		if (command.data[0] != 0x02) {
			response->sendError();
			return;
		}
		const uint8_t& fan = command.data[1];
		if (fan >= FAN_NUM) {
			response->sendError();
			return;
		}
		const uint8_t& type = command.data[2];
		if (type > 0x03) {
			response->sendError();
			return;
		}
		setFanDetectionType(fan, type);
		response->send(NULL, 0);
		break;
	}
	case READ_FAN_DETECTION_TYPE:
	{
		if (command.data[0] != 0x01) {
			response->sendError();
			return;
		}
		const uint8_t& fan = command.data[1];
		if (fan >= FAN_NUM) {
			response->sendError();
			return;
		}
		uint8_t type = getFanDetectionType(fan);
		uint8_t typeData[] = { type };
		response->send(typeData, sizeof(typeData));
		break;
	}
	default:
		response->sendError();
	}
}
