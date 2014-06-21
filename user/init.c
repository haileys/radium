#include <radium.h>
#include "crt0.h"

int
main()
{
    fork();
    regdump();
    yield();
    return 123;
}
