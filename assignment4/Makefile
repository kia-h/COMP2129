CC = clang
CFLAGS = -g -O0 -Wall -W -std=gnu99
LDFLAGS = -lm -lpthread

HASTE = ~comp2129/assignment4/bin/haste --all
QUIET = ~comp2129/assignment4/bin/haste --test --quiet

UPDATE = ~comp2129/assignment4/bin/haste --update

TEST = ~comp2129/assignment4/bin/haste --test
RAIN = ~comp2129/assignment4/bin/haste --rain

SUBMISSION = ~comp2129/bin/submit-assignment4

BINARIES = pagerank

.PHONY: all clean haste quiet test rain update submission

all: $(BINARIES)
	$(UPDATE)

pagerank: pagerank.o

haste:
	$(HASTE)

quiet:
	$(QUIET)

test:
	$(TEST)

update:
	$(UPDATE)

rain:
	$(RAIN)

submission:
	$(SUBMISSION)

clean:
	-rm -f *.o
	-rm -f $(BINARIES)

