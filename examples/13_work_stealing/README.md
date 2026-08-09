# Work Stealing

This example demonstrates ATOM's SMP scheduler and its work-stealing mechanism.

Four threads are created before the secondary processor is started. Initially, all threads execute on the primary processor. After a short delay, the application starts the secondary processor with `scheduler_start_secondary()`.

Once both processors are running, the scheduler can redistribute runnable threads between processors. When a processor has no suitable local work, it can steal work from the other processor.

## What it demonstrates

* Creating multiple concurrent threads.
* Running threads on multiple processors.
* Starting the secondary processor explicitly.
* SMP scheduling and work stealing.
* Thread migration between processors.
* Mutex synchronization between threads executing on different processors.

All four threads share a single mutex while printing their messages:

```c
WITH_MUTEX(&mutex)
{
  printf("Annie: Hello from CORE %lu!\r\n", CPUID);
}
```

This ensures that console output from different processors does not interleave.

## Expected behavior

Before the secondary processor is started, the threads run on the primary processor.

After:

```c
scheduler_start_secondary();
```

the scheduler enables the secondary processor and begins balancing the workload between both processors.

The output therefore contains messages such as:

```text
Annie: Hello from CORE 0!
Brian: Hello from CORE 0!
James: Hello from CORE 1!
Sarah: Hello from CORE 1!
Annie: Hello from CORE 1!
Brian: Hello from CORE 0!
```

The exact processor on which a thread executes is nondeterministic and may change over time as the scheduler performs work stealing.

## Running

Build and flash the example using the normal ATOM build procedure.

The example starts in single-processor mode and explicitly enables the secondary processor after five seconds. This makes the transition from single-processor scheduling to SMP scheduling visible in the console output.
