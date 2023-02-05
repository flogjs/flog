MAIN=flog
FLAGS=-Wall -O3 -o $(MAIN) -rdynamic -lgit2 -Ideps
SRCS=$(wildcard src/*.c src/**/*.c)

S4_DIR=deps/s4
S4_FILES=cutils.c libregexp.c libunicode.c quickjs.c
S4_SRCS=$(patsubst %.c,$(S4_DIR)/%.c,$(S4_FILES))
S4_OBJS=$(patsubst %.c,%.o,$(S4_FILES))
S4_OS=qjs_objs
S4_A=libqjs.a
S4_VERSION='"$(shell cat $(S4_DIR)/VERSION)"'

all: $(MAIN)

init-submodules:
	@if git submodule status | grep -E -q '^[-]|^[+]';\
		then git submodule update --init;\
	fi

$(S4_OS): init-submodules
	$(CC) -DCONFIG_VERSION=$(S4_VERSION) -O -c $(S4_SRCS) -lm

$(S4_A): $(S4_OS)
	ar rcs $@ $(S4_OBJS)
	ranlib $@

$(MAIN): $(S4_A)
	$(CC) -DS4_VERSION=$(S4_VERSION) -L. $(SRCS) $(S4_A) $(FLAGS) -lm -ldl
	rm -f $(S4_OBJS)

main:
	$(CC) -DS4_VERSION=$(S4_VERSION) -L. $(SRCS) $(S4_A) $(FLAGS) -lm -ldl

debug:
	$(CC) -DS4_VERSION=$(S4_VERSION) -L. $(SRCS) $(S4_A) $(FLAGS) -g -lm -ldl

clean:
	rm f $(MAIN)
