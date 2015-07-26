CC = clang
CFLAGS = -g -O0 -pedantic -Wall -W -std=c99
LDFLAGS =

HASTE = ~comp2129/assignment2/bin/haste --all

TEST = ~comp2129/assignment2/bin/haste --test
RAIN = ~comp2129/assignment2/bin/haste --rain
COVER = ~comp2129/assignment2/bin/haste --cover

SUBMISSION = ~comp2129/bin/submit-assignment2

BINARIES = minesweeper

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

minesweeper: minesweeper.o
	$(CC) $(LDFLAGS) $^ -o $@

-include Makefile.depends
