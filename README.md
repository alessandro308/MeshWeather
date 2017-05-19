# Introduction
This project try to build a distributed network with ESP-01 chips. This network is used by some weather station proudly powered with Arduino. 
All data are sent to server that analyze the values in order to know how set external operator (e.g. pump, windows of greenhouse...)

![Ideal Space Organization](https://raw.githubusercontent.com/alessandro308/MeshWeather/master/esempiomw.png)

## Hardware
In order to build  system, we use 

- 2 ESP-01 connected to Arduino Weather Station
- 6 ESP-01 used to build a mesh network
- 2 Arduino used to read and send sensors values
- 1 Raspberry used as gateway that sends all data to external server 

## Software
To build the mesh network we have used [easyMesh library](https://github.com/Coopdis/easyMesh). On this library we wrote a layer that sends packets to server from source Arduino node using a route discovery with best effort choice (*src/network/ESPMesh/ESPMesh.ino*)

[More info](http://www.apagiaro.it/meshweather/) (*in italian*)

# How use/execute this code
### Flash your ESP-01
In order to flash your chip, follow [this tutorial](https://h3ron.com/post/programmare-lesp8266-ovvero-arduino-con-il-wifi-a-meno-di-2/) (in italian).

To use ESP-01 without Arduino, we have used [this circuite](http://fritzing.org/projects/esp01-development-board) (for flashing) and [this one](http://apagiaro.it/assets/only-usage.fzz) for normal usage mode.


