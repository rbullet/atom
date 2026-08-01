# Stdin Stdout Example

This example demonstrates console input and output using the standard C library interfaces provided by ATOM.

Instead of using ATOM-specific UART APIs, the application communicates through the standard streams (`stdin` and `stdout`), showing how ATOM integrates with the C runtime environment.

## Demonstrates

* Standard output with `printf()`
* Standard input with `fgets()`
* Newlib/libc integration
* Console interaction through ATOM's I/O backend

## Source

The application implements a simple console echo:

```c
#include <stdio.h>
#include <string.h>

#define BUFFER_SIZE 128

int main(void)
{
    char buffer[BUFFER_SIZE];

    printf("Console echo ready.\r\n");

    while (1)
    {
        printf("> ");

        if (fgets(buffer, sizeof(buffer), stdin) != NULL)
        {
            printf("echo: %s", buffer);
        }
    }

    return 0;
}
```

## Expected output

After flashing the example and opening the serial console:

```text
Console echo ready.
> Hello ATOM
echo: Hello ATOM
> 
```

## Purpose

This example validates the standard input/output path of ATOM.

It demonstrates that applications can use familiar C library functions without directly interacting with hardware drivers. The underlying console implementation is provided by ATOM, while the application remains hardware-independent.

## Notes

This example intentionally does not include `<atom.h>` because it does not use ATOM-specific APIs. The ATOM runtime provides the environment required for standard C I/O to work, but applications only need to include the interfaces they directly use.
