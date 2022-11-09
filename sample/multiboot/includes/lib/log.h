#ifndef __LOG_H__
#define __LOG_H__

#include "lib/string.h"

#if DEBUG >= 4
#define KDEBUG(msg, ...) printf("[DEBUG] %s:%i " msg "\n", __FILE__, __LINE__, ##__VA_ARGS__)
#else
#define KDEBUG(msg, ...)
#endif

#if DEBUG >= 3
#define INFO(msg, ...) printf("[INFO] " msg "\n", __FILE__, __LINE__, ##__VA_ARGS__)
#else
#define INFO(msg, ...)
#endif

#if DEBUG >= 1
#define WARN(msg, ...) printf("[WARN] %s:%i " msg "\n", __FILE__, __LINE__, ##__VA_ARGS__)
#else
#define WARN(msg, ...)
#endif

#if DEBUG >= 0
#define ERROR(msg, ...) printf("[ERROR] %s:%i " msg "\n", __FILE__, __LINE__, ##__VA_ARGS__)
#else
#define ERROR(msg, ...)
#endif

#define PANIC(msg) { printf("[>PANIC<] %s at %s:%i\n", msg, __FILE__, __LINE__); asm volatile("cli\nhlt"); }

#endif