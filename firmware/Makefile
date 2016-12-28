
EXEC_NAME=sousvide
OBJS=main.o 1wire.o delay.o timebase.o controller.o uart.o com.o
LIBS=-lfixmath #-lprintf_flt
INCDIRS=-I./libfixmath
LIBDIRS=-L./libfixmath

COMPILE_ARCH=atmega8
AVRDUDE_PROGRAMMER=usbtiny
AVRDUDE_ARCH=m8
AVRDUDE_FLAGS=
AVRDUDE_FUSES=-U lfuse:w:0xce:m -U hfuse:w:0xd1:m
CPU_FREQ=20083200UL

CC=avr-gcc
LD=avr-gcc

CFLAGS=-DFIXMATH_NO_CACHE -DFIXMATH_NO_64BIT -Os -g --std=c11 -Wall -Wpedantic -Wshadow -Wcast-qual -Wstrict-prototypes -Wmissing-prototypes -Wformat -Wswitch-default -Wcast-align -Wbad-function-cast -Winline -Wundef -Wnested-externs -Wextra# -Wconversion
CXXFLAGS=
LDFLAGS=#-Wl,-u,vfprintf

EXTRA_PRE_TARGETS=libfixmath

TARGET=$(EXEC_NAME).elf
HEX_TARGET=$(EXEC_NAME).hex
LSS_TARGET=$(EXEC_NAME).lss
EEP_TARGET=$(EXEC_NAME).eep

# default target
.PHONY: all
all: $(TARGET) $(HEX_TARGET) $(LSS_TARGET) $(EEP_TARGET) size

###
# Automatic dependency generation
# http://make.mad-scientist.net/papers/advanced-auto-dependency-generation/#tldr
DEPDIR := .d
$(shell mkdir -p $(DEPDIR) >/dev/null)
DEPFLAGS = -MT $@ -MMD -MP -MF $(DEPDIR)/$*.Td

COMPILE.c = $(CC) $(DEPFLAGS) -DF_CPU=$(CPU_FREQ) -mmcu=$(COMPILE_ARCH) $(CFLAGS) $(INCDIRS) -c
COMPILE.cpp = $(CXX) $(DEPFLAGS) -DF_CPU=$(CPU_FREQ) -mmcu=$(COMPILE_ARCH) $(CXXFLAGS) $(INCDIRS) -c
POSTCOMPILE = mv -f $(DEPDIR)/$*.Td $(DEPDIR)/$*.d

# Compile source files
%.o : %.c
%.o : %.c $(DEPDIR)/%.d
	$(COMPILE.c) $<
	$(POSTCOMPILE)

# save auto-generated dependency files
$(DEPDIR)/%.d: ;
.PRECIOUS: $(DEPDIR)/%.d	
-include $(patsubst %,$(DEPDIR)/%.d,$(basename $(SRCS)))

###
# Binary conversion and diagnostic targets
%.hex: $(TARGET)
	avr-objcopy -O ihex $(HEX_FLAGS) -R .eeprom $< $@

%.eep: $(TARGET)
	avr-objcopy -O ihex $(HEX_FLAGS) -j .eeprom $< $@

%.lss: $(TARGET)
	avr-objdump -h -S $< > $@

.PHONY: size
size: $(TARGET)
	@echo
	@avr-size $(TARGET) -C --mcu=$(COMPILE_ARCH)

$(TARGET): $(EXTRA_PRE_TARGETS) $(OBJS)
	$(LD) $(LDFLAGS) $(LIBDIRS) $(OBJS) $(LIBS) -mmcu=$(COMPILE_ARCH) -o $(TARGET)

.PHONY: clean
clean:
	rm -f $(TARGET) $(LSS_TARGET) $(EEP_TARGET) $(HEX_TARGET) $(OBJS)
	$(MAKE) -C libfixmath clean

.PHONY: flash
flash: $(HEX_TARGET) size
	avrdude -c $(AVRDUDE_PROGRAMMER) -p $(AVRDUDE_ARCH) $(AVRDUDE_FLAGS) -U $<

.PHONY: flash-fuses
flash-fuses:
	avrdude -c $(AVRDUDE_PROGRAMMER) -p $(AVRDUDE_ARCH) $(AVRDUDE_FUSES)

###
# Extra targets
.PHONY: libfixmath
libfixmath:
	$(MAKE) -C libfixmath
