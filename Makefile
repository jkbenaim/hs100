target  = hs100
objects = comms.o handlers.o hs100.o

CFLAGS  = -std=gnu99

.PHONY: all
all:	$(target)

.PHONY: clean
clean:
	rm -f $(target) $(objects)

$(target): $(objects)
