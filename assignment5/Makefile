CC = clang
CFLAGS = -g -W -Wall --std=gnu99
LDFLAGS =

BINARIES = devm

SUBMISSION = ~comp2129/bin/submit-assignment5

.PHONY: all clean submission update

all: $(BINARIES)

clean:
	-rm -f *.o
	-rm -f $(BINARIES)

devm: devm.o

submission:
	$(SUBMISSION)
