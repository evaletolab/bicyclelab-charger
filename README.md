![hello](https://lh3.googleusercontent.com/u6uzfmIr64OXDIhBdAWevHADYqP8KsHIfpmXf05qrN_KUxtjE6e559r5vRt3JOEzLLrSXmzISk8z81a6Snw-NsPD5wVTzAcZcbistyxwU4rw48ZnIu08DzG8bk2zXzrOsH9Sxmq_mbfY9tilDW0yokgKVcQWWOkhWqDF018Fj1MG8RYrJbAnxGKuOFZOtOKQM8kHR_O-9ecDBc-MtK5wGp1JTlU2pkFK8m_9UlSsgpcn3zNaRW6YBkDl3Cw-h0NPUXmE5QXqf1LkA7RDjbII3VxnrkzTkMj_JF5CdP1nlVDdnCq78GrG_QyUJ4OHLejKDcrtINvDBpYs25HNeUBvjhycg1evn1uYmBA_1vjWYZlAGtvL1bi_b45mjkQtr1MpHKamagQchMqzIEcccduzAo5RA_sMmFd4T1d8_baATDcoI2LuY2IFhmAXsUGqCit4FH5hNhmHVIjLGGwvTxXQ_jCw7juLtHCUqzSJH76fx1iuBS8TUHBcF09mbdtvDazsoqH7V7HwZO6jiW7SomLb-5kJHOTF7QoHMV24qfb2VYBMjSMPJQVtXnsG534_BBLcdbJ3=w1199-h1598-no)
#85-110W single output battery charger

This is an opensource lithium-ion battery charger that use laptop power adapter as input.

[![Bitdeli Badge](https://d2weczhvl823v0.cloudfront.net/evaletolab/bicyclelab-charger/trend.png)](https://bitdeli.com/free "Bitdeli Badge")

##Features

1. boost input from 18.5V - 4.7A to 42V (or 47.2V) with ~95% of efficiency 
2. stage charging; current control then voltage control
3. full protections: Short circuit / Overload / Over voltage / Over temperature
4. 1 LED indicator for charging status
5. BMS features; Balancing charge on each cell [TODO]
6. custom number of serial cells 5..12
7. small size, only 30x90x8mm

![The PCB board](https://raw.github.com/evaletolab/bicyclelab-charger/master/hardware/bicyclelab-charger-v4.2.png "charger v4.2")

##The result
![8S2P](https://farm8.staticflickr.com/7239/13272785655_40815b8815_z.jpg)

##Power stage (battery charge)

This if the fourth prototype release. The purpose of this board is to manage the charge  
of a 10/11 serial lithium-ion cell with an input given by a laptop ac adapter (currently the apple magsafe1 85W). 
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
