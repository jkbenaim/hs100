target  ?= hs100
objects := $(patsubst %.c,%.o,$(wildcard *.c))

CFLAGS=-std=c99 -Os
ifdef DEBUG
CFLAGS+=-Wall -Werror -ggdb
endif

.PHONY: all
all:	$(target)

.PHONY: clean
clean:
	rm -f $(target) $(objects)

$(target): $(objects)
