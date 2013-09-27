#85-110W single output battery charger

This is an opensource lithium-ion battery charger that use laptop power adapter as input.

##Features

1. boost input from 18.5V - 20V 6A to 42V 2.5A
2. stage charging; current control then voltage control
3. full protections: Short circuit / Overload / Over voltage / Over temperature
4. 1 LED indicator for charging status
5. BMS features; Balancing charge on each cell
6. custom number of serial cells 5..12
7. small size, only 30x90x8mm

![The PCB board](https://raw.github.com/evaletolab/bicyclelab-charger/master/hardware/bicyclelab-charger-v4.2.png "charger v4.2")

##Power stage (battery charge)

This if the fourth prototype release for the power board. The purpose of this board is to manage the output 
of a (5..12)serial lithium-ion battery charge with an input given by a laptop ac adapter (currently the apple magsafe1 85W). 
The software is mainly a PID control based on the MAX_POWER for the fast charge and the BATTERY_VOLTAGE for 
the constant charge. The size of the board is about 30x90mm


##Cell balancing and protection
This project is targeted for each individual cell. The purpose of this project is to make a simple cascading 
module to manage over/under voltage/temperatur for each cell. The first prototype is still not available for testing. 

1. an overvoltage measurement is triggering a passive balancing for the cell
2. TODO - implement an isolated one wire communication to broadcast,
  * undervoltage signal
  * over temperature signal
 
## License
GPL3

Copyright (c) 2012 Olivier Evalet (http://evaletolab.ch/)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the “Software”), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
