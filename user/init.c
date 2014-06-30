#include "crt.h"

int
main()
{
    console_log("Hello world from init!\n");

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
