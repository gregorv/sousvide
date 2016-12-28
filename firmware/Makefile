
EXEC_NAME=sousvide
OBJS=main.o 1wire.o delay.o timebase.o controller.o uart.o com.o
LIBS=-lfixmath #-lprintf_flt
INCDIRS=-I./libfixmath
LIBDIRS=-L./libfixmath

COMPILE_ARCH=atmega8
AVRDUDE_ARCH=m8
AVRDUDE_FLAGS=
CPU_FREQ=20083200L 

CC=avr-gcc
LD=avr-gcc

CFLAGS=-Wall -Os -Wno-pointer-sign
CXXFLAGS=
LDFLAGS=#-Wl,-u,vfprintf

EXTRA_PRE_TARGETS=libfixmath

TARGET=$(EXEC_NAME).elf
HEX_TARGET=$(EXEC_NAME).hex
LSS_TARGET=$(EXEC_NAME).lss

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
	avr-objcopy -O ihex $(HEX_FLAGS) $< $@

%.lss: $(TARGET)
	avr-objcopy -h -S $< > $@

.PHONY: size
size: $(TARGET)
	@echo
	@avr-size $(TARGET)

$(TARGET): $(EXTRA_PRE_TARGETS) $(OBJS)
	$(LD) $(LDFLAGS) $(LIBDIRS) $(LIBS) $(OBJS) -o $(TARGET)

.PHONY: clean
clean:
	rm -f $(TARGET) $(LSS_TARGET) $(HEX_TARGET) $(OBJS)
	$(MAKE) -C libfixmath clean

#sousvide.elf: main.c main.h 1wire.h 1wire.c delay.h delay.c timebase.c timebase.h#	$(CC) -o sousvide.elf -DF_CPU=20083200L -mmcu=atmega8 -O3 -Wall -Wno-pointer-sign -lm -lprintf_flt -Wl,-u,vfprintf main.c delay.c 1wire.c timebase.c

.PHONY: all
all: $(TARGET) $(HEX_TARGET) $(LSS_TARGET).lss size

flash: $(HEX_TARGET)
	avrdude -c $(AVRDUDE_PROGRAMMER) -p $(AVRDUDE_ARCH) $(AVRDUDE_FLAGS) -e -U $<

###
# Extra targets
.PHONY: libfixmath
libfixmath:
	$(MAKE) -C libfixmath
