# blah: main.c
#		$(CC) main.c -o cquel -Wall -Wextra -pedantic

TARGET = ./bin/gmalloc
LIBS = -lm
CC = clang
CFLAGS = -g -Wall -Wextra -pedantic # -fsanitize=undefined

.PHONY: default all clean

default: $(TARGET)

all: default

SOURCES=$(wildcard src/**/*.c src/*.c)
OBJECTS=$(patsubst %.c,%.o,$(SOURCES))
HEADERS=$(wildcard src/**/*.h src/*.h)

%.o: %.c $(HEADERS)
		$(CC) $(CFLAGS) -c $< -o $@

.PRECIOUS: $(TARGET) $(OBJECTS)

$(TARGET): $(OBJECTS) $(HEADERS)
		$(CC) $(OBJECTS) -Wall $(LIBS) -o $@

clean:
		rm -rf build $(OBJECTS) $(TESTS)
		rm -rf bin/*
		find . -name "*.gc*" -exec rm {} \;
		rm -rf `find . -name "*.dSYM" -print`

# The Checker
check:
		@echo Files with potentially dangerous functions.
		@egrep '[^_.>a-zA-Z0-9](str(n?cpy|n?cat|xfrm|n?dup|str|pbrk
