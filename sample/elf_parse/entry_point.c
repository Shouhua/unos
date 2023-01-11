#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/mman.h>
#include <errno.h>

// return code 0
const char *instructions = "\x48\x31\xFF\xB8\x3C\x00\x00\x00\x0F\x05";

int main() {
    printf("        main @ %p\n", &main);
    printf("instructions @ %p\n", instructions);

    size_t region = (size_t) instructions;
    region = region & (~0xFFF);
    printf("        page @ %p\n", region);

    printf("making page executable...\n");
    int ret = mprotect(
        (void*) region,       // addr
        0x1000,               // len - now the size of a page (4KiB)
        PROT_READ | PROT_EXEC // prot
    );
    if (ret != 0) {
        printf("mprotect failed: error %d\n", errno);
        return 1;
    }
    void (*f)(void) = (void*) instructions;
    printf("jumping...\n");
    f();
    printf("after jump\b");
}