CFLAGS=-O2 -Wall -Wextra -fpic -shared $(OPTFLAGS)
PREFIX?=/usr/local
SOURCES=$(wildcard src/**/*.c src/*.c)
OBJECTS=$(patsubst %.c,%.o,$(SOURCES))
TEST_SRC=$(wildcard tests/*_tests.c)
TESTS=$(patsubst %.c,%,$(TEST_SRC))
TARGET=bin/gmalloc
HEADERS=$(wildcard src/**/*.h src/*.h)
# SO_TARGET=$(patsubst %.a,%.so,$(TARGET))
CC = clang

.PHONY: default all clean tests

default: $(TARGET)

dev: CFLAGS=-g -Wall -DDEBUG -Wextra -pedantic -Wno-unused-parameter -Wno-unused-variable -Wno-unused-function -Wno-format-pedantic $(OPTFLAGS)
dev: all

all: default

%.o: %.c $(HEADERS)
		$(CC) $(CFLAGS) -c $< -o $@

.PRECIOUS: $(TARGET) $(OBJECTS)

$(TARGET): $(OBJECTS) $(HEADERS)
		$(CC) $(OBJECTS) $(CFLAGS) $(LIBS) -o $@

# Run
run: dev
	./bin/gmalloc

watch: run
	./auto_runner.sh

clean:
		rm -rf build $(OBJECTS) $(TESTS)
		rm -rf bin/*
		find . -name "*.gc*" -exec rm {} \;
		rm -rf `find . -name "*.dSYM" -print`

# The Checker
check:
		@echo Files with potentially dangerous functions.
		@egrep '[^_.>a-zA-Z0-9](str(n?cpy|n?cat|xfrm|n?dup|str|pbrk|tok|_)|stpn?cpy|a?sn?printf|byte_)' $(SOURCES) || true
