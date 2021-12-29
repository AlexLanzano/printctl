LIBRARIES =
INCLUDE = -I.
CFLAGS = -Wall -Werror $(INCLUDE) $(LIBRARIES) -g3 \
         -MMD -MF $(DEPDIR)/$*.d

DEPDIR = .deps/

SOURCE ?= $(wildcard *.c)
OBJECTS = $(patsubst %.c,%.o,$(SOURCE))
DEPENDS = $(patsubst %.c,$(DEPDIR)/%.d,$(SOURCE))
BINARY = printctl

all: $(BINARY)

%.d:
	@mkdir -p $(@D)

%.o: %.c Makefile
	gcc $(CFLAGS) -c -o $@ $<

$(BINARY): $(OBJECTS)
	gcc $(CFLAGS) -o $@ $^

.PHONY: clean
clean:
	rm -rf $(DEPDIR) $(OBJECTS) $(BINARY)

include $(DEPENDS)
