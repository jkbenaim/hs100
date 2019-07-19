target  ?= hs100
objects := $(patsubst %.c,%.o,$(wildcard *.c))

LDFLAGS=-lm
CFLAGS=-std=gnu99 -Os
ifdef DEBUG
CFLAGS+=-Wall -Werror -ggdb
endif

.PHONY: all
all:	$(target)

.PHONY: clean
clean:
	rm -f $(target) $(objects)

$(target): $(objects)
