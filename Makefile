target  ?= hs100
objects := $(patsubst %.c,%.o,$(wildcard *.c))
CFLAGS=-Wall -Werror -std=c99 -ggdb -Os

.PHONY: all
all:	$(target)

.PHONY: clean
clean:
	rm -f $(target) $(objects)

$(target): $(objects)
