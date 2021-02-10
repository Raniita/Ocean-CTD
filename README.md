# Ocean-CTD
<img src="https://user-images.githubusercontent.com/30501761/107502497-0439d780-6b99-11eb-9fac-b34615552ace.jpg" width="800" height="600">

A CTD device’s primary function is to detect how the conductivity and temperature of the water column changes relative to depth.
This CTD obtain the values of colored dissolved organic matter (CDOM), Phycoerythrin (PE) and Chlorophyll (CHL).

### Based on:
Actually using a master-slave communication. An Arduino is doing the slave, and the mastering task its doing for other devices, like phone or laptop.

#### Arduino (v2)
<img src="https://user-images.githubusercontent.com/30501761/107502491-02701400-6b99-11eb-85ca-b9cfb308657b.jpg" width="800" height="600">

* Arduino UNO
* USR ES1 W5500
* Custom PCB for cyclop cable conector
* I2C Logic converter, 3.3V to 5V
* PoE passive splitter, Ethernet + 5V
* [Cable (UTP) 26AWG (PoE over it)](https://bluerobotics.com/store/cables-connectors/cables/cab-nbpuf-4utp-26awg/)

##### Connections
Powering the Arduino with Ethernet PoE, and connecting it to a Router. The Router spawns a WiFi called "Boat", that allow the communication over laptop-CTD. On Router, we configure IP static for the CTD. 

##### Programs
Using a python based program to ask the CTD all values sensors, saving to a CSV and live plotting all the received values.

### Used sensors:
* [Turner Cyclops-7F CDOM](https://www.turnerdesigns.com/cyclops-7f-submersible-fluorometer)
* [Turner Cyclops-7F PE](https://www.turnerdesigns.com/cyclops-7f-submersible-fluorometer)
* [Turner Cyclops-7F CHL](https://www.turnerdesigns.com/cyclops-7f-submersible-fluorometer)
* [Depth/Pressure Sensor MS5837-30BA](https://bluerobotics.com/store/sensors-sonars-cameras/sensors/bar30-sensor-r1/)
* [Temperature TSYS01](https://bluerobotics.com/store/sensors-sonars-cameras/sensors/celsius-sensor-r1/)

### Disclaimer
This is a research version of CTD. Actually is on development.

This CTD is used by the research group [Mar Menor UPCT](https://ocean.upct.es/). 



