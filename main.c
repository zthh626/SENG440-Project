#include "arm_neon.h"
#include <stdio.h>

int x = 0;

int main( void) {
    printf("hello world");
    x += 2;
    printf("hello world %d", x);
    uint32x2_t aa, bb, ss;

    ss = vadd_u32( aa, bb);

    return 0;
}