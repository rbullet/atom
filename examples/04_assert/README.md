# Assert Example

This example demonstrates how to use ATOM assertions to validate assumptions and detect programming errors during development.

The application defines a `buy_car()` function that requires users to have a valid driving licence. The assertion ensures that this requirement is respected before continuing execution.

## Demonstrates

* Using `ATOM_ASSERT`
* Validating function preconditions
* Detecting invalid program states
* Generating meaningful assertion error messages

## Source

```c
#include <atom.h>

typedef struct user_t
{
  char const* name;
  bool has_driving_licence;
} user_t;

static void buy_car(user_t const* user)
{
  ATOM_ASSERT(
    user->has_driving_licence,
    "%s does not have a driving licence",
    user->name
  );

  printf("%s bought a car.\r\n", user->name);
}

int main(void)
{
  user_t const paul = {
    .name = "Paul",
    .has_driving_licence = true
  };

  user_t const john = {
    .name = "John",
    .has_driving_licence = false
  };

  buy_car(&paul);
  buy_car(&john);

  while (1)
  {
  }
}
```

## Expected output

The first call succeeds because Paul has a driving licence:

```text
Paul bought a car.
```

The second call violates the requirement enforced by the assertion:

```c
buy_car(&john);
```

ATOM detects the invalid state and reports a fatal error:

```text
[FATAL] main.c:11 Assertion 'user->has_driving_licence' failed: John does not have a driving licence
```

The exact line number may vary depending on the source file.

## Purpose

Assertions are used to verify conditions that should always be true if the program is behaving correctly.

In this example, `buy_car()` has a precondition:

> A user must have a driving licence before buying a car.

Instead of silently accepting invalid input, the assertion immediately reports the programming error.

Assertions are useful for detecting:

* Invalid function parameters
* Broken assumptions
* Unexpected internal states
* Programming mistakes

## Assertions vs error handling

Assertions are intended for conditions that indicate a bug in the program.

For example:

```c
ATOM_ASSERT(user->has_driving_licence, "...");
```

is appropriate here because the function assumes that callers only provide valid users.

Assertions are not a replacement for normal runtime error handling. Expected failures, such as a missing file or unavailable peripheral, should usually be handled using normal error reporting mechanisms.

## Notes

A good assertion should document an assumption made by the code.

When an assertion fails, ATOM provides:

* The failure severity (`FATAL`)
* The source file
* The line number
* The failed expression
* A custom diagnostic message

This allows developers to locate and fix problems quickly during development.
