Add Support of the Lighting Node PRO Protocol to Arduino.
Controll LEDs connected to the Arduino with the Corsair Link or the iCUE Software.


# Requirements
This library uses the USB HID interface of the Arduino Micro or Leonardo.
The library is compatible with all boards using the MCU ATmega32U4.

You need two USB devices one for the "Lighting Node PRO" and one "Lighting Loader".

Visual Studio with the extension [Visual Micro](https://marketplace.visualstudio.com/items?itemName=VisualMicro.ArduinoIDEforVisualStudio).

# DISCLAIMERS
This is a DO IT YOURSELF project, use at your own risk.

THIS IS NOT PLUG AND PLAY.
You MAY need to use a custom Bootloader for the Arduino and/or custom Arduino core.

Changing the VID and PID of your Arduino forces you to use a custom boards configuration, otherwise the Arduino is lost(can not be detected by the IDE).

# Credits
- [HoodLoader2](https://github.com/NicoHood/HoodLoader2)
- [Arduino HID Project](https://github.com/NicoHood/HID)
- [Protocol Information](https://github.com/audiohacked/OpenCorsairLink/issues/70)
