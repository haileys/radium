#include <radium.h>
#include "crt0.h"

int
main()
{
    if(fork()) {
        wait();
        return 123;
    } else {
        regdump();
        yield();
        regdump();
        return 456;
    }
}
