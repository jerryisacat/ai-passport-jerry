#include "jerryicat_nav.h"

#include <assert.h>
#include <stdio.h>

int main(void) {
    assert(jerryicat_nav_next(0, 1, 3) == 1);
    assert(jerryicat_nav_next(2, 1, 3) == 0);
    assert(jerryicat_nav_next(0, -1, 3) == 2);
    assert(jerryicat_nav_next(1, 0, 3) == 1);
    assert(jerryicat_nav_next(0, 1, 1) == 0);
    assert(jerryicat_nav_next(0, -1, 1) == 0);
    assert(jerryicat_nav_next(0, 1, 0) == -1);
    assert(jerryicat_nav_next(0, 1, -2) == -1);
    assert(jerryicat_nav_next(9, 1, 3) == 1);
    assert(jerryicat_nav_next(-4, -1, 3) == 2);
    assert(jerryicat_nav_next(1, 7, 3) == 2);

    puts("jerryicat nav tests: PASS");
    return 0;
}
