target  = hs100
objects = comms.o handlers.o hs100.o escape.o

CFLAGS  = -std=gnu99
LDFLAGS = -lresolv

.PHONY: all
all:	$(target)

.PHONY: clean
clean:
	rm -f $(target) $(objects)

$(target): $(objects)
	$(CC) -o $@ $(objects) $(LDFLAGS)
