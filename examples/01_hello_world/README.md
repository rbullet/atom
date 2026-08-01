# Hello World Example

This is the simplest ATOM example. It verifies that the basic application pipeline is working correctly.

## Demonstrates

* ATOM startup sequence
* C runtime initialization
* `main()` execution
* Standard output through `printf()`
* UART output integration

## Source

The application simply prints a message and then enters an infinite loop:

```c
#include <stdio.h>

int main(void)
{
    printf("Hello World\n");

    while (1)
    {
    }

    return 0;
}
```

## Expected output

After flashing the example and opening the serial console:

```
Hello World
```

## Purpose

This example is intended as a minimal smoke test for ATOM. If this example runs correctly, the toolchain, linker configuration, startup code, and basic I/O path are correctly configured.
