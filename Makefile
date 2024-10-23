# Define programs
CC = avr-gcc
LD = avr-ld

# Serial port for programming
PORT = /dev/cu.usbserial-843320

# Target Device/Architecture
MCU = attiny214

# Compiler flags
CFLAGS = -g -Wall -mcall-prologues -mmcu=$(MCU) -Os -I include/

# Project name
NAME = realtemp

# Define the build DIR
BUILD_DIR = build

# The name of the target file
TARGET = $(BUILD_DIR)/$(NAME)

# Define all source files
SOURCES = $(wildcard *.c)
$(info Sources: $(SOURCES))

# Define all object files
OBJECTS = $(addprefix $(BUILD_DIR)/,$(notdir $(SOURCES:.c=.o)))
$(info Objects: $(OBJECTS))

all: $(TARGET).hex

# Compile: create object files from C source files
$(BUILD_DIR)/%.o: %.c
	$(info Compiling $<)
	$(CC) $(CFLAGS) -c $< -o $@

# ELF
$(TARGET).elf: $(OBJECTS)
	$(info Building ELF output $<)
	$(CC) $(CFLAGS) $(OBJECTS) -o $(TARGET).elf

# HEX (for programming)
$(TARGET).hex: $(TARGET).elf
	$(info Building HEX output $<)
	avr-objcopy -O ihex -R .eeprom $(TARGET).elf $(TARGET).hex

clean: 
	rm -f $(TARGET).elf $(TARGET).hex $(OBJECTS)

size:
	avr-size --mcu=$(MCU) -C $(TARGET).elf

# Program the device
program: $(TARGET).hex
	avrdude -p t214 -P $(PORT) -c serialUPDI -U flash:w:$(TARGET).hex
