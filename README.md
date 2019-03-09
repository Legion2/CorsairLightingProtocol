
[![arduino-library-badge](https://www.ardu-badge.com/badge/Corsair%20Lighting%20Protocol.svg?)](https://www.ardu-badge.com/Corsair%20Lighting%20Protocol)
![Actions Status](https://wdp9fww0r9.execute-api.us-west-2.amazonaws.com/production/badge/Legion2/CorsairLightingProtocol)

**This library can be used to integrate custom/unofficial RGB strips with iCUE.**

## Features
* Add support of the Lighting Node PRO protocol to Arduino.
* Control LEDs with the Corsair Link or the iCUE software.
* Easy to use with [FastLED](http://fastled.io/)
* [Supported LED chipsets](https://github.com/FastLED/FastLED/wiki/Overview#chipsets)
* persistent settings for use without USB connection

![Overview](docs/Overview.png)

# Getting started

## Requirements
The library is compatible with all boards using the MCU ATmega32U4.
This includes **Arduino Micro**, **Arduino Leonardo** and **Pro Micro**.
It is **not** compatible with Arduino Uno, Arduino Mega and Arduino Nano.

In the rest of the documentation the board with the ATmega32U4 is called "Arduino" regardless of the manufacturer.

As an IDE Visual Studio with the extension [Visual Micro](https://marketplace.visualstudio.com/items?itemName=VisualMicro.ArduinoIDEforVisualStudio) is required.
The Arduino IDE is not supported because it does not provide a way to configure the build options without creating your own board configuration.
Advanced users can create their own boards configuration with the correct [build settings](https://github.com/Legion2/CorsairLightingProtocol/blob/master/examples/SimpleLightingController/board.txt) and then use this boards configuration with the Arduino IDE.

## Install the library
Use the [Library-Manager](https://www.visualmicro.com/page/User-Guide.aspx?doc=Library-Manager.html#) to install this library or download a [release](https://github.com/Legion2/CorsairLightingProtocol/releases).

## Example
Open the example "SimpleLightingController", you can find it in the Visual Micro Explorer.
The example only requires one LED Stip connected to the Arduino.
The library [FastLED](http://fastled.io/) is used to control the leds, for more information on [how to wiring the leds](https://github.com/FastLED/FastLED/wiki/Wiring-leds) and [how to set up the leds in the code](https://github.com/FastLED/FastLED/wiki/Basic-usage#setting-up-the-leds) see the links.
After you did the wiring or at least know which pin is the data pin, you can set this pin in the example sketch and upload it to the arduino.

To verify the library works as expected open the Windows settings -> devices -> connected devices. Somewhere in the list of devices there should be a device called "Lighting Node PRO". (If not, please open an [Issue](https://github.com/Legion2/CorsairLightingProtocol/issues))
Open [iCUE](https://www.corsair.com/icue) there should also be the "Lighting Node PRO".

![iCUE RGB Strip example](docs/iCUE.jpg)

In iCUE open the "Lighting Setup" tab of the Lighting Node PRO(LNP) and set for both Lighting Channels the device to "RGB Light Strip" and the amount to a tenth of the leds you have.
iCUE groups the LEDs into groups of ten.
So if you have 20 LEDs, set the amount to 2.
The "SimpleLightingController" example support maximum 5 per channel.
Because the "SimpleLightingController" concatenate the two channels a total of 100 leds can be controlled.
Now you can create lighting effects in the "Lighting Channel #" tabs.

## Use the library
If you want to use this library in your own sketch don't forget to use Visual Studio as IDE and include the [board.txt](https://github.com/Legion2/CorsairLightingProtocol/blob/master/examples/SimpleLightingController/board.txt) in the root directory of your sketch.

## How it works
This library uses the USB HID interface of the Arduino Micro or Leonardo.
In the board.txt the unique VID and PID of a "Lighting Node PRO" are defined.
After uploading a sketch with the library and these IDs, iCUE recognizes the Arduino as a Lighting Node PRO.
In iCUE you can then select the "Lighting Node PRO" and set some lighting effects.
iCUE sends these via the CorsairLightingProtocol to the Arduino.
These commands are understood by the library and converted into lighting effects on the RGB strips connected to the Arduino.

# DISCLAIMERS
This is a DO IT YOURSELF project, use at your own risk.

# Credits
- [HoodLoader2](https://github.com/NicoHood/HoodLoader2)
- [Arduino HID Project](https://github.com/NicoHood/HID)
- [Protocol Information](https://github.com/audiohacked/OpenCorsairLink/issues/70)
