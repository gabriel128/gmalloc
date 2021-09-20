#pragma once

#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define ANSI_COLOR_RED "\x1b[31m"
#define ANSI_COLOR_GREEN "\x1b[32m"
#define ANSI_COLOR_YELLOW "\x1b[33m"
#define ANSI_COLOR_BLUE "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN "\x1b[36m"
#define ANSI_COLOR_RESET "\x1b[0m"

#pragma clang diagnostic ignored "-Wgnu-zero-variadic-macro-arguments"

#ifdef NDEBUG
#define log_debug(M, ...)
#else
#define log_debug(M, ...)                                                      \
  fprintf(stderr, ANSI_COLOR_CYAN "[DEBUG] (%s:%d) " M ANSI_COLOR_RESET "\n",  \
          __FILE__, __LINE__, ##__VA_ARGS__)
#endif

#define clean_errno() (errno == 0 ? "None" : strerror(errno))

#define log_error(M, ...)                                                      \
  fprintf(stderr,                                                              \
          ANSI_COLOR_RED "[ERROR] (%s:%d: errno: %s) " M ANSI_COLOR_RESET      \
                         "\n",                                                 \
          __FILE__, __LINE__, clean_errno(), ##__VA_ARGS__)

#define log_info(M, ...)                                                       \
  fprintf(stderr, ANSI_COLOR_GREEN "[INFO] (%s:%d) " M ANSI_COLOR_RESET "\n",  \
          __FILE__, __LINE__, ##__VA_ARGS__)

// Result
// TODO: Make it IntResult

typedef enum { RESULT_OK, RESULT_ERR } ResultKind;

typedef struct Result {
  ResultKind kind;
  union {
    uint64_t val;
    char* err;
  } the;
} Result;

#define OK(value) ((Result){RESULT_OK, {.val = value}})
#define ERR(error) ((Result){RESULT_ERR, {.err = error}})
#define IS_OK(result) ((result).kind == RESULT_OK)
#define IS_ERR(result) ((result).kind == RESULT_ERR)