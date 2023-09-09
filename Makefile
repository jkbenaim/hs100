target  = hs100
objects = comms.o handlers.o hs100.o escape.o

CFLAGS  = -std=gnu99 -Wall -Werror

MACHINE := $(shell $(CC) -dumpmachine)
$(info MACHINE="$(MACHINE)")

ifneq (,$(findstring mingw,$(MACHINE)))
    LDFLAGS = -lws2_32
else
    LDFLAGS = -lresolv
endif

.PHONY: all
all:	$(target)

.PHONY: clean
clean:
	rm -f $(target) $(objects)

$(target): $(objects)
	$(CC) -o $@ $(objects) $(LDFLAGS)
