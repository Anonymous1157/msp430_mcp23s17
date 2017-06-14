CC=msp430-elf-gcc
CFLAGS=-O0 -g3
TARGET_ARCH=-mmcu=msp430g2553
CPPFLAGS=-Wall -Werror -Wextra -Wpedantic
DEBUGGER=rf2500

main: main.o libspibuf.o

upload: main
	mspdebug $(DEBUGGER) erase "load $<" "verify $<" reset exit

clean:
	rm -f *.la *.o *.su main

.PHONY: clean upload