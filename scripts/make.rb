#!/usr/bin/ruby
# 20.11.2011 customized by Olivier.Evalet@gelux.ch for multiple target
#
# Written by: Paulo H. "Taka" Torrens <paulo_torrens@hotmail.com>

require 'optparse'

# This hash will hold all of the options
# parsed from the command-line by
# OptionParser.
options = {}

optparse = OptionParser.new do|opts|
	# Set a banner, displayed at the top
	# of the help screen.
	opts.banner = "Usage: make.rb [options] "

	# Define the options, and what they do
	options[:target] = 'attiny85'
	opts.on( '-t', '--target', 'Select MCU Target:attiny44|attiny45|attiny84|*attiny85|atmega168|atmega328' ) do|target|
		options[:target] = target
	end

	options[:dude] = 'usbasp'
	opts.on( '-d', '--dude', 'Select dude command: stk500v1|usbasp|avrisp' ) do|dude|
		options[:dude] = dude
	end
end
optparse.parse!


Libs = []
Headers = []
Extensions = /\.(c$|cpp$)/

Target = {
	'atmega168'=>"MCU = atmega168\nF_CPU = 16000000\nFORMAT = ihex\nUPLOAD_RATE = 19200\nMAX_SIZE = 14336\n",
	'atmega328'=>"MCU = atmega328\nF_CPU = 16000000\nFORMAT = ihex\nUPLOAD_RATE = 57600\nMAX_SIZE = 30720\n",
	'attiny45'=>"MCU = attiny45\nF_CPU = 8000000\nFORMAT = ihex\nUPLOAD_RATE = 57600\nMAX_SIZE = 8192\n",
	'attiny85'=>"MCU = attiny85\nF_CPU = 8000000\nFORMAT = ihex\nUPLOAD_RATE = 57600\nMAX_SIZE = 8192\n",
	'attiny84'=>"MCU = attiny84\nF_CPU = 8000000\nFORMAT = ihex\nUPLOAD_RATE = 57600\nMAX_SIZE = 8192\n"
}

class LoL
  attr_accessor :dir
  def get_files(dir = @dir)
    Headers.push("-I\"#{dir}\"")
    Headers.uniq!
    array = []
    Dir.foreach(dir) do |file|
      next if [".", ".."].include?(file)
      name = dir + "/" + file
      if FileTest.directory?(name)
        array += get_files(name)
      else
        if file =~ Extensions
          array.push(name)
        end
      end
    end
    return array
  end
  def command(x)
    obj = x.gsub(Extensions, ".o")
    return "#{obj}: #{x}\n\t$(CC) -c #{x} -o #{obj}"
  end
end
class Lib < LoL
  attr_accessor :name
  def initialize(name)
    @name = name + ".a"
    @dir = "lib/" + name
  end
  def to_s
    array = get_files
    objects = array.collect { |x| x.gsub(Extensions, ".o") }
    return [
      "#{@name}: #{objects.join(" ")}\n\t#{objects.collect { |x| "$(AR) #{@name} #{x}" }.join("\n\t")}",
      array.collect { |x| command(x) }
    ].compact.join("\n\n")
  end
  def to_src
    array = get_files
    return "#{array.join(' ')}"
  end
  def <=> (other)
    if @name == "default.a"
      return 1
    else
      return @name <=> other.name
    end
  end
end
class Source < LoL
  attr_accessor :objects
  attr_accessor :array
  def initialize(ext)
    @dir = "src"
    @array = get_files
    @objects = @array.collect { |x| x.gsub(Extensions, ext) }
  end
  def to_s
    return @array.collect { |x| command(x) }.join("\n\n")
  end
end
Dir.foreach("lib") do |dir|
  next if [".", ".."].include?(dir)
	Libs.push(Lib.new(dir))
end

Sources = Source.new(".o")
Sources.to_s
Libs.sort!
Libs.join(" ")
Makefile = <<EOF
###################################################################

CC = avr-gcc $(CPP_FLAGS)
LD = avr-gcc $(LD_FLAGS)
AR = avr-ar $(AR_FLAGS)
OBJCP = avr-objcopy
SIZE = avr-size -A --mcu=$(MCU)
AVRDUDE = avrdude
#DUDE = -cstk500v1 -P$(PORT) -b$(UPLOAD_RATE) -D 
DUDE = usbasp -D

LIBRARIES = #{([Libs.collect { |x| x.name }] - ["tiny.a"]).join(" ")}

# Customise project sources/objects/includes here  
SRC = #{Sources.array.join(" ")}
OBJECTS = #{Sources.objects.join(" ")}
INCLUDES = -I"/usr/lib/avr/include/avr" -I"./include" #{Headers.join(" ")}

#
DEFINES = -DF_CPU=$(F_CPU)L -DARDUINO=22

#TODO
# OBJECTS = $(SRC:.c=.o) $(SRC:.cpp=.o) $(ASRC:.S=.o)
# SRC = $(foreach dir,$(SRCDST),$(wildcard $(dir)/*.c))
# OBJ = $(addsuffix .o, $(basename $(subst ${SRCDST},${OBJDST},${SRC})))


CPP_FLAGS = -g -Os -w -fno-exceptions -ffunction-sections -fdata-sections -mmcu=$(MCU) $(DEFINES) $(INCLUDES)
LD_FLAGS = -Os -Wl,--gc-sections -mmcu=$(MCU)
AR_FLAGS = rcs

USB=0
PORT = /dev/ttyUSB$(USB)
#{Target[options[:target]]}

OUTPUT = irmimic

all: $(LIBRARIES) $(OUTPUT).hex

#{Libs.join("\n\n")}

#
#
# Compile: create object files from C++ source files.
.cpp.o:
	$(CC) -c $< -o $@

# Compile: create object files from C source files.
.c.o:
	$(CC) -c  $< -o $@

$(OUTPUT).elf: $(OBJECTS) $(LIBRARIES)
\t$(LD) $(OBJECTS) $(LIBRARIES) -lm -o $(OUTPUT).elf

$(OUTPUT).hex: $(OUTPUT).elf
\t$(OBJCP) -O ihex -R .eeprom $(OUTPUT).elf $(OUTPUT).hex
\t$(SIZE) -A $(OUTPUT).hex 
  
  
.PHONY: upload clean

upload: all
\tstty -F $(PORT) hupcl
\t$(AVRDUDE) -p$(MCU) -c$(DUDE)  -Uflash:w:$(OUTPUT).hex:i 

clean:
\t@rm -f $(LIBRARIES) $(OUTPUT).elf $(OUTPUT).hex $(shell find . -follow -name "*.o")
  
#Makefile: scripts/make.rb $(shell find src -follow -not -type f -newer Makefile)
#\t@scripts/make.rb
EOF
File.open("Makefile.NEW", "w") do |file|
  file.write(Makefile)
end
