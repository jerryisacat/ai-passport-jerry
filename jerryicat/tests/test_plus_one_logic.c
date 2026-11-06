#include "plus_one_logic.h"

#include <assert.h>
#include <stdint.h>
#include <stdio.h>

int main(void) {
    assert(plus_one_increment(0) == 1);
    assert(plus_one_increment(41) == 42);
    assert(plus_one_increment(UINT32_MAX - 1u) == UINT32_MAX);
    assert(plus_one_increment(UINT32_MAX) == UINT32_MAX);

    puts("plus-one logic tests: PASS");
    return 0;
}
