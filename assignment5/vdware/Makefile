CC = clang
CFLAGS = -g -Wall -W -std=gnu99
LDFLAGS = -lm

BINARIES = vdware

.PHONY: all clean configure

all: $(BINARIES)

configure: configure.sh
	./configure.sh

vdware.o: vdware.h

vdware: vdware.o

update:
	hg pull && hg up

clean:
	-killall -9 vdware devm 2> /dev/null ||:
	-rm -f *.o
	-rm -f $(BINARIES)
	-rm -f devm.{log,pid}
	-rm -f new_devices
	-rm -f dev/* output/*
