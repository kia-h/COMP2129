CC = clang
CFLAGS = -g -O0 -pedantic -Wall -W -std=c99
LDFLAGS =

HASTE = ~comp2129/assignment3/bin/haste --all

TEST = ~comp2129/assignment3/bin/haste --test
RAIN = ~comp2129/assignment3/bin/haste --rain
COVER = ~comp2129/assignment3/bin/haste --cover

SUBMISSION = ~comp2129/bin/submit-assignment3

BINARIES = dw

.PHONY: all clean depends haste test rain cover

all: $(BINARIES)

clean:
	-rm -f *.o
	-rm -f $(BINARIES)

depends:
	$(CC) $(CFLAGS) -MM *.c > Makefile.depends

haste:
	$(HASTE)

test:
	$(TEST)

rain:
	$(RAIN)

cover:
	$(COVER)

submission:
	$(SUBMISSION)

dw: dw.o
	$(CC) $(LDFLAGS) $^ -o $@

-include Makefile.depends
