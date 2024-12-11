target  = hs100
objects = comms.o handlers.o hs100.o escape.o

CFLAGS  = -std=c99 -Wall -Werror
# for those who like their warnings turned up to 11
CFLAGS += -Wextra -Wstrict-prototypes -Wmissing-prototypes -Wshadow -pedantic
CFLAGS += -Wpointer-arith -Wcast-align -Wcast-qual -Wwrite-strings -Wundef

# also passes cppcheck --std=c99 --enable=all --suppress=missingInclude *.c

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
