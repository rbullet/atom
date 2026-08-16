#include <stdio.h>
#include <stdint.h>
#include <atom.h>

#define STUDENT_THREAD_STACK_SIZE 1024

static uint32_t student1_thread_stack[STUDENT_THREAD_STACK_SIZE];
static uint32_t student2_thread_stack[STUDENT_THREAD_STACK_SIZE];
static uint32_t student3_thread_stack[STUDENT_THREAD_STACK_SIZE];

static bool school_is_over = false;

static mutex_t mutex = MUTEX_INITIALIZER;
static condition_variable_t end_of_school = CONDITION_VARIABLE_INITIALIZER;

static void* student_routine(void* const arg)
{
  char const* const student_name = (char const* const)arg;
  WITH_MUTEX(&mutex)
  {
    while (!school_is_over)
    {
      condition_variable_wait(&end_of_school, &mutex);
    }
    printf("%s: It's the end of school! I'm going back home!\r\n", student_name);
  }
  WITH_MUTEX_END
  return NULL;
}

int main(void)
{
  thread_t student1_thread;

  thread_init(
      &student1_thread,
      student1_thread_stack,
      STUDENT_THREAD_STACK_SIZE,
      student_routine,
      (void*)"Cartman"
  );
  thread_start(&student1_thread);

  thread_t student2_thread;

  thread_init(
      &student2_thread,
      student2_thread_stack,
      STUDENT_THREAD_STACK_SIZE,
      student_routine,
      (void*)"Kyle"
  );
  thread_start(&student2_thread);

  thread_t student3_thread;

  thread_init(
      &student3_thread,
      student3_thread_stack,
      STUDENT_THREAD_STACK_SIZE,
      student_routine,
      (void*)"Stan"
  );
  thread_start(&student3_thread);

  thread_sleep(duration_of(5, SECONDS));
  printf("Garrison: School is over! All students can go back home!\r\n");
  WITH_MUTEX(&mutex)
  {
    school_is_over = true;
    condition_variable_broadcast(&end_of_school);
  }
  WITH_MUTEX_END

  while (1)
  {
  }

  return 0;
}