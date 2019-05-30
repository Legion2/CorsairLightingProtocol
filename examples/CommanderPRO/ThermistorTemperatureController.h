// ThermistorTemperatureContorller.h

#ifndef _ThermistorTemperatureController_h
#define _ThermistorTemperatureController_h

#include "TemperatureController.h"

class ThermistorTemperatureController : public TemperatureController {
public:
	void addSensor(uint8_t index, uint8_t pin);
protected:
	virtual uint16_t getTemperatureValue(uint8_t temperatureSensor) override;
	virtual bool isTemperatureSensorConnected(uint8_t temperatureSensor) override;
	virtual uint16_t getVoltageRail12V() override;
	virtual uint16_t getVoltageRail5V() override;
	virtual uint16_t getVoltageRail3V3() override;

	uint8_t sensorPins[TEMPERATURE_NUM] = { 0 };
};

#endif

