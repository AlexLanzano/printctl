LIBRARIES =
INCLUDE = -I.
CFLAGS = -Wall -Werror $(INCLUDE) $(LIBRARIES) -g3 \
         -MMD -MF $(DEPDIR)/$*.d

DEPDIR = .deps/

COMMON_SOURCE ?= $(wildcard common/*.c)
COMMON_OBJECTS = $(patsubst %.c,%.o,$(COMMON_SOURCE))

CLIENT_SOURCE ?= $(wildcard client/*.c)
CLIENT_OBJECTS = $(patsubst %.c,%.o,$(CLIENT_SOURCE)) $(COMMON_OBJECTS)
CLIENT_BINARY = printctl

DAEMON_SOURCE ?= $(wildcard daemon/*.c)
DAEMON_OBJECTS = $(patsubst %.c,%.o,$(DAEMON_SOURCE)) $(COMMON_OBJECTS)
DAEMON_BINARY = printctld

DEPENDS = $(patsubst %.c,$(DEPDIR)/%.d,$(CLIENT_SOURCE)) \
          $(patsubst %.c,$(DEPDIR)/%.d,$(DAEMON_SOURCE)) \
          $(patsubst %.c,$(DEPDIR)/%.d,$(COMMON_SOURCE))

all: $(CLIENT_BINARY) $(DAEMON_BINARY)

%.d:
	@mkdir -p $(@D)

%.o: %.c Makefile
	gcc $(CFLAGS) -c -o $@ $<

$(CLIENT_BINARY): $(CLIENT_OBJECTS)
	gcc $(CFLAGS) -o $@ $^

$(DAEMON_BINARY): $(DAEMON_OBJECTS)
	gcc $(CFLAGS) -o $@ $^

install: $(CLIENT_BINARY) $(DAEMON_BINARY)
	install -m 755 $(CLIENT_BINARY) /usr/bin
	install -m 755 $(DAEMON_BINARY) /usr/bin
	install -m 644 printctl@.service /usr/lib/systemd/system

.PHONY: clean
clean:
	rm -rf $(DEPDIR) $(CLIENT_OBJECTS) $(DAEMON_OBJECTS) $(CLIENT_BINARY) $(DAEMON_BINARY)

include $(DEPENDS)
