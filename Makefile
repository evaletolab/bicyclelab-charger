###################################################################

CC = avr-gcc $(CPP_FLAGS)
LD = avr-gcc $(LD_FLAGS)
AR = avr-ar $(AR_FLAGS)
OBJCP = avr-objcopy
SIZE = avr-size -A --mcu=$(MCU)
AVRDUDE = avrdude
#DUDE = -cstk500v1 -P$(PORT) -b$(UPLOAD_RATE) -DF
DUDE = -cusbasp -v
#DUDEB= -B 16 



# setting fuse (original values:lfuse:0x62, hfuse:0xDF, efuse:0xFF) for external crystal
# http://www.engbedded.com/fusecalc/


# //
# // FUSE configuration for breadboard usage
# //
# Int. RC Osc. 8 MHz; Start-up time PWRDWN/RESET: 6 CK/14 CK + 64 ms; [CKSEL=0010 SUT=10]
# Preserve EEPROM memory through the Chip Erase cycle; [EESAVE=1]
# Serial program downloading (SPI) enabled; [SPIEN=1]
# Brown-out Detection disabled [BODLEVEL=111] // save energy
#  NOTE: If you have a bootloader or are storing data in the EEPROM you must set the BOD!
# Divide clock by 8 [CKDIV8=1]
# Watch-dog Timer always on; [WDTON=0]
FUSE = -U lfuse:w:0xe2:m -U hfuse:w:0xDF:m -U efuse:w:0xFF:m

# // https://github.com/jcw/ide-hardware/blob/master/avr/boards.txt
# Int. RC Osc. 8 MHz; Start-up time PWRDWN/RESET: 6 CK/14 CK + 0 ms; [CKSEL=0010 SUT=00]
# Brown-out detection disabled; [BODLEVEL=111] 
# Preserve EEPROM memory through the Chip Erase cycle; [EESAVE=0]
# Serial program downloading (SPI) enabled; [SPIEN=0]
FUSE = -U lfuse:w:0xC2:m -U hfuse:w:0xD7:m -U efuse:w:0xFF:m


LIBRARIES = tiny.a

# define *main* or *test* programm
OUTPUT = main


# Customise project sources/objects/includes here  
SRC = src/energy.cpp  src/PID/PID_v1.cpp src/$(OUTPUT).cpp
OBJECTS =   src/energy.o src/PID/PID_v1.o src/$(OUTPUT).o
INCLUDES = -I"/usr/lib/avr/include/avr" -I"./include" -I"src"  -I"lib/tiny" -I"src/PID"

#
DEFINES = -DF_CPU=$(F_CPU)L -DTINY=007  -D_DEBUG_SERIAL

#TODO
# OBJECTS = $(SRC:.c=.o) $(SRC:.cpp=.o) $(ASRC:.S=.o)
# SRC = $(foreach dir,$(SRCDST),$(wildcard $(dir)/*.c))
# OBJ = $(addsuffix .o, $(basename $(subst ${SRCDST},${OBJDST},${SRC})))


CPP_FLAGS = -g -Os -w -fno-exceptions -ffunction-sections -fdata-sections -mmcu=$(MCU) $(DEFINES) $(INCLUDES)
LD_FLAGS = -Os -Wl,--gc-sections -mmcu=$(MCU)
AR_FLAGS = rcs

#Int. clock, 8Mhz
F_CPU = 8000000
USB=0
PORT = /dev/ttyUSB$(USB)
MCU = attiny84
FORMAT = ihex
UPLOAD_RATE = 57600
MAX_SIZE = 30720



all: $(LIBRARIES) $(OUTPUT).hex
# find lib/ -name \*.cpp|while read i; do echo -e "$i: $i\n\t\$(CC) -c $i -o $i\n"; done
# find lib/ -name \*.c|while read i; do echo -e "\t\$(AR) $i"; done

tiny.a: lib/tiny/WString.o lib/tiny/TinyDebugSerial.o lib/tiny/TinyDebugSerial38400.o lib/tiny/WMath.o lib/tiny/TinyDebugSerial115200.o lib/tiny/TinyDebugSerial9600.o lib/tiny/HardwareSerial.o lib/tiny/Tone.o lib/tiny/main.o lib/tiny/Print.o lib/tiny/TinyDebugSerialErrors.o lib/tiny/wiring_digital.o lib/tiny/wiring_shift.o lib/tiny/pins_arduino.o lib/tiny/wiring.o lib/tiny/wiring_analog.o lib/tiny/wiring_pulse.o lib/tiny/WInterrupts.o
	$(AR) tiny.a lib/tiny/WString.o
	$(AR) tiny.a lib/tiny/TinyDebugSerial.o
	$(AR) tiny.a lib/tiny/TinyDebugSerial38400.o
	$(AR) tiny.a lib/tiny/WMath.o
	$(AR) tiny.a lib/tiny/TinyDebugSerial115200.o
	$(AR) tiny.a lib/tiny/TinyDebugSerial9600.o
	$(AR) tiny.a lib/tiny/HardwareSerial.o
	$(AR) tiny.a lib/tiny/Tone.o
	$(AR) tiny.a lib/tiny/main.o
	$(AR) tiny.a lib/tiny/Print.o
	$(AR) tiny.a lib/tiny/TinyDebugSerialErrors.o
	$(AR) tiny.a lib/tiny/wiring_digital.o
	$(AR) tiny.a lib/tiny/wiring_shift.o
	$(AR) tiny.a lib/tiny/pins_arduino.o
	$(AR) tiny.a lib/tiny/wiring.o
	$(AR) tiny.a lib/tiny/wiring_analog.o
	$(AR) tiny.a lib/tiny/wiring_pulse.o
	$(AR) tiny.a lib/tiny/WInterrupts.o
	
lib/tiny/WString.o: lib/tiny/WString.cpp
	$(CC) -c lib/tiny/WString.cpp -o lib/tiny/WString.o
	
lib/tiny/TinyDebugSerial.o: lib/tiny/TinyDebugSerial.cpp
	$(CC) -c lib/tiny/TinyDebugSerial.cpp -o lib/tiny/TinyDebugSerial.o
	
lib/tiny/TinyDebugSerial38400.o: lib/tiny/TinyDebugSerial38400.cpp
	$(CC) -c lib/tiny/TinyDebugSerial38400.cpp -o lib/tiny/TinyDebugSerial38400.o
	
lib/tiny/WMath.cpp: lib/tiny/WMath.cpp
	$(CC) -c lib/tiny/WMath.cpp -o lib/tiny/WMath.o
	
lib/tiny/TinyDebugSerial115200.o: lib/tiny/TinyDebugSerial115200.cpp
	$(CC) -c lib/tiny/TinyDebugSerial115200.cpp -o lib/tiny/TinyDebugSerial115200.o
	
lib/tiny/TinyDebugSerial9600.o: lib/tiny/TinyDebugSerial9600.cpp
	$(CC) -c lib/tiny/TinyDebugSerial9600.cpp -o lib/tiny/TinyDebugSerial9600.o
	
lib/tiny/HardwareSerial.o: lib/tiny/HardwareSerial.cpp
	$(CC) -c lib/tiny/HardwareSerial.cpp -o lib/tiny/HardwareSerial.o
	
lib/tiny/Tone.o: lib/tiny/Tone.cpp
	$(CC) -c lib/tiny/Tone.cpp -o lib/tiny/Tone.o
	
lib/tiny/main.o: lib/tiny/main.cpp
	$(CC) -c lib/tiny/main.cpp -o lib/tiny/main.o
	
lib/tiny/Print.o: lib/tiny/Print.cpp
	$(CC) -c lib/tiny/Print.cpp -o lib/tiny/Print.o
	
lib/tiny/TinyDebugSerialErrors.o: lib/tiny/TinyDebugSerialErrors.cpp
	$(CC) -c lib/tiny/TinyDebugSerialErrors.cpp -o lib/tiny/TinyDebugSerialErrors.o

lib/tiny/wiring_digital.o: lib/tiny/wiring_digital.c
	$(CC) -c lib/tiny/wiring_digital.c -o lib/tiny/wiring_digital.o

lib/tiny/wiring_shift.o: lib/tiny/wiring_shift.c
	$(CC) -c lib/tiny/wiring_shift.c -o lib/tiny/wiring_shift.o

lib/tiny/pins_arduino.o: lib/tiny/pins_arduino.c
	$(CC) -c lib/tiny/pins_arduino.c -o lib/tiny/pins_arduino.o

lib/tiny/wiring.o: lib/tiny/wiring.c
	$(CC) -c lib/tiny/wiring.c -o lib/tiny/wiring.o

lib/tiny/wiring_analog.o: lib/tiny/wiring_analog.c
	$(CC) -c lib/tiny/wiring_analog.c -o lib/tiny/wiring_analog.o

lib/tiny/wiring_pulse.o: lib/tiny/wiring_pulse.c
	$(CC) -c lib/tiny/wiring_pulse.c -o lib/tiny/wiring_pulse.o

lib/tiny/WInterrupts.o: lib/tiny/WInterrupts.c
	$(CC) -c lib/tiny/WInterrupts.c -o lib/tiny/WInterrupts.o


#
#
# Compile: create object files from C++ source files.
.cpp.o:
	$(CC) -c $< -o $@

# Compile: create object files from C source files.
.c.o:
	$(CC) -c  $< -o $@

$(OUTPUT).elf: $(OBJECTS) $(LIBRARIES)
	$(LD) $(OBJECTS) $(LIBRARIES) -lm -o $(OUTPUT).elf

$(OUTPUT).hex: $(OUTPUT).elf
	$(OBJCP) -O ihex -R .eeprom $(OUTPUT).elf $(OUTPUT).hex
	$(SIZE) -A $(OUTPUT).hex 
  
  
.PHONY: upload clean fuse

reset:
	$(AVRDUDE) -p$(MCU) $(DUDE) 


fuse: all
	$(AVRDUDE) -p$(MCU) $(DUDE) -B 12 $(FUSE)
	
	
upload: all
#	stty -F $(PORT) hupcl
	$(AVRDUDE) -p$(MCU) $(DUDE) $(DUDEB)  -Uflash:w:$(OUTPUT).hex:i 

clean:
	@rm -f $(LIBRARIES) $(OUTPUT).elf $(OUTPUT).hex $(shell find . -follow -name "*.o")
  
#Makefile: scripts/make.rb $(shell find src -follow -not -type f -newer Makefile)
#	@scripts/make.rb
