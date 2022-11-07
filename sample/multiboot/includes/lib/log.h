#ifndef __LOG_H__
#define __LOG_H__

#include "lib/string.h"

#if DEBUG >= 4
#define debug(msg, ...) printf("[DEBUG] %s:%i " msg "\n", __FILE__, __LINE__, ##__VA_ARGS__)
#else
#define debug(msg, ...)
#endif

#if DEBUG >= 3
#define info(msg, ...) printf("[INFO] " msg "\n", __FILE__, __LINE__, ##__VA_ARGS__)
#else
#define info(msg, ...)
#endif

#if DEBUG >= 1
#define warn(msg, ...) printf("[WARN] %s:%i " msg "\n", __FILE__, __LINE__, ##__VA_ARGS__)
#else
#define warn(msg, ...)
#endif

#if DEBUG >= 0
#define error(msg, ...) printf("[ERROR] %s:%i " msg "\n", __FILE__, __LINE__, ##__VA_ARGS__)
#else
#define error(msg, ...)
#endif

#define PANIC(msg) { printf("[>PANIC<] %s at %s:%i\n", msg, __FILE__, __LINE__); asm volatile("cli\nhlt"); }

#endif