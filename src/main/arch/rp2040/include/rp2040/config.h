#pragma once
#ifdef __cplusplus
extern "C" {
#endif

#include "atom_config.h"

#define XOSC_FREQUENCY_HZ 12000000

#define malloc_spinlock spinlock31

#define scheduler_spinlock spinlock30

#define thread_spinlock spinlock29

#define deferred_tasks_spinlock spinlock28

#define mutex_spinlock spinlock27

#define semaphore_spinlock spinlock26

#define condition_variable_spinlock spinlock25

#ifdef __cplusplus
}
#endif