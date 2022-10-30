MAIN=flog
CFLAGS=-Wall -O3 -o $(MAIN) -rdynamic -lgit2
SRCS=$(wildcard src/*.c src/**/*.c)

QJS_DIR=deps/quickjs
QJS_FILES=cutils.c libbf.c libregexp.c libunicode.c quickjs.c
QJS_SRCS=$(patsubst %.c,$(QJS_DIR)/%.c,$(QJS_FILES))
QJS_OBJS=$(patsubst %.c,%.o,$(QJS_FILES))
QJS_OS=qjs_objs
QJS_A=libqjs.a
QJS_VERSION='"$(shell cat $(QJS_DIR)/VERSION)"'

all: $(MAIN)

init-submodules:
	@if git submodule status | grep -E -q '^[-]|^[+]';\
		then git submodule update --init;\
	fi

$(QJS_OS): init-submodules
	$(CC) -DCONFIG_VERSION=$(QJS_VERSION) -O -c $(QJS_SRCS) -lm

$(QJS_A): $(QJS_OS)
	ar rcs $@ $(QJS_OBJS)
	ranlib $@

$(MAIN): $(QJS_A)
	$(CC) -DQJS_VERSION=$(QJS_VERSION) -L. $(SRCS) $(QJS_A) $(CFLAGS) -lm -ldl
	rm -f $(QJS_OBJS)

main:
	$(CC) -DQJS_VERSION=$(QJS_VERSION) -L. $(SRCS) $(QJS_A) $(CFLAGS) -lm -ldl

clean:
	rm f $(MAIN)
