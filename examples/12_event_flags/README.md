# Event Flags Example

This example demonstrates how to use **ATOM event flags** to synchronize multiple threads using shared state conditions.

Event flags are useful when threads need to wait for one or more conditions to become true. Unlike semaphores or notifications, event flags are **persistent**: once a flag is set, it remains set until it is explicitly cleared.

In this example, the event flags represent the state of a concert:

* The concert has started.
* Brian is present.
* (A future example could use Julie being present.)

## Scenario

Three threads participate in the scenario:

* **Annie** waits for the concert to start.
* **Brian** waits for the concert to start, then announces his presence.
* **James** waits until both the concert has started and Brian is present.

The event flags are defined as:

```c
#define CONCERT_STARTED (1u << 0)
#define BRIAN_PRESENT   (1u << 1)
#define JULIE_PRESENT   (1u << 2)
```

Each bit represents a condition.

The shared event flags object is:

```c
event_flags_t concert_events = EVENT_FLAGS_INITIALIZER;
```

## Waiting for an Event

Annie waits for the concert to start:

```c
event_flags_wait(&concert_events,
                 CONCERT_STARTED,
                 EVENT_FLAGS_ALL_SET);
```

The thread blocks until the requested flag is set.

Because only one bit is requested, `EVENT_FLAGS_ALL_SET` means:

> The `CONCERT_STARTED` bit must be set.

## Multiple Threads Can Wait on the Same Flag

Both Annie and Brian wait for the same condition:

```c
event_flags_wait(&concert_events,
                 CONCERT_STARTED,
                 EVENT_FLAGS_ALL_SET);
```

When the main thread starts the concert:

```c
event_flags_set(&concert_events, CONCERT_STARTED);
```

both threads are released.

Event flags are not consumed by a waiting thread. The flag remains set, allowing multiple threads to observe the same event.

## Waiting for Multiple Conditions

James waits for two conditions:

```c
event_flags_wait(&concert_events,
                 CONCERT_STARTED | BRIAN_PRESENT,
                 EVENT_FLAGS_ALL_SET);
```

This means:

> Continue only when both `CONCERT_STARTED` and `BRIAN_PRESENT` are set.

After Brian notices the concert has started, he sets his presence flag:

```c
event_flags_set(&concert_events, BRIAN_PRESENT);
```

The event state becomes:

```
CONCERT_STARTED = true
BRIAN_PRESENT   = true
```

James can now continue.

## Expected Output

```
The concert will start soon...!
Annie: I'm waiting the beginning of the concert!
Brian: I'm waiting the beginning of the concert!
James: I'm waiting for Brian to arrive!
The concert has started!
Annie: The concert has started!
Brian: The concert has started! Let me warn James!
James: Brian is here! Let's enjoy the concert!
```

## Key Concepts Demonstrated

### Persistent State

Event flags represent conditions, not one-shot notifications.

If a thread starts waiting after a flag has already been set, it immediately continues.

Example:

```
Concert starts
      |
      v
CONCERT_STARTED = true

        (later)

James starts waiting
      |
      v
Condition already satisfied
```

James does not wait for the next concert.

### Broadcast Behavior

Setting a flag wakes all waiting threads whose conditions are satisfied.

Event flags are therefore suitable for:

* Initialization completion
* Hardware readiness
* System state changes
* Multiple threads waiting for the same condition

For one-to-one communication or ownership transfer, use a different synchronization primitive such as a semaphore, mutex, or message queue.
