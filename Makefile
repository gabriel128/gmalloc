CC = clang
# ANSI Colors

GREEN='\033[0;32m'
NC='\033[0m' # No Color

# DEV compiling flags
CFLAGS=-g -Wall -Wextra -pedantic -Wno-unused-parameter \
	-Wno-unused-variable -Wno-unused-function -Wno-format-pedantic $(OPTFLAGS)

SOURCES=$(wildcard src/**/*.c src/*.c)
OBJECTS=$(patsubst src/%.c,build/%.o,$(SOURCES))
HEADERS=$(wildcard src/**/*.h src/*.h)

TARGET=lib/libgmalloc.a
SO_TARGET=$(patsubst %.a,%.so,$(TARGET))

# TESTS
TEST_DIR=tests
TEST_SRC=$(wildcard $(TEST_DIR)/*_tests.c)
TESTBINS=$(patsubst $(TEST_DIR)/%.c,$(TEST_DIR)/bin/%,$(TEST_SRC))

###

.PHONY: all clean tests release

all: $(TARGET) $(SO_TARGET)

build/%.o: src/%.c $(HEADERS)
		$(CC) $(CFLAGS) -c $< -o $@

.PRECIOUS: $(TARGET) $(OBJECTS)

# For binaries
# $(TARGET): $(OBJECTS) $(HEADERS)
# 		$(CC) $(OBJECTS) $(CFLAGS) $(LIBS) -o $@

$(TARGET): CFLAGS += -fPIC
$(TARGET): $(OBJECTS) $(HEADERS)
	ar rcs $@ $(OBJECTS)

$(SO_TARGET): $(TARGET) $(OBJECTS)
		$(CC) -shared -o $@ $(OBJECTS)

# Release build

release: CFLAGS=-DNDEBUG -O2 -Wall -Wextra $(OPTFLAGS)
release: clean
release: $(TARGET)


watch-tests: test
	@echo -e "${GREEN}** Running Test Watch **${NC}"
	while true; do \
	echo " "; \
	echo " ================================= "; \
	echo " "; \
	make --no-print-directory test; \
	inotifywait ./* -q -e modify; \
	done

clean:
		rm -rf build/* bin/* $(OBJECTS) $(TEST_DIR)/bin/* lib/*
		find . -name "*.gc*" -exec rm {} \;
		rm -rf `find . -name "*.dSYM" -print`

# The Checker
check:
		@echo Files with potentially dangerous functions.
		@egrep '[^_.>a-zA-Z0-9](str(n?cpy|n?cat|xfrm|n?dup|str|pbrk|tok|_)|stpn?cpy|a?sn?printf|byte_)' $(SOURCES) || true

$(TEST_DIR)/bin/%: $(TEST_DIR)/%.c $(OBJECTS) $(HEADERS)
	$(CC) $(CFLAGS) $< $(OBJECTS) -o $@ -lcriterion $(TARGET)

test: CFLAGS=-DNDEBUG
test: $(TARGET) $(TEST_DIR)/bin $(TESTBINS) $(HEADERS)
	for test in $(TESTBINS); do ./$$test --verbose=2 ; done

## Example
example: bin/example

bin/example: $(TARGET) examples/main.c
	$(CC) $(CFLAGS) examples/main.c -o $@ $(TARGET)

## INIT Project structure
init:
	mkdir -p src
	mkdir -p bin
	mkdir -p lib
	mkdir -p build
	mkdir -p tests
	mkdir -p tests/bin

format:
	find src -iname *.h -o -iname *.c | xargs clang-format -i
