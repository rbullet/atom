#pragma once
#ifdef __cplusplus
extern "C" {
#endif

#ifdef DEBUG

#define ATOM_ASSERT(condition, message)             \
  do                                                \
  {                                                 \
    if (!(condition))                               \
    {                                               \
      atom_assert(__FILE__, __LINE__, (message));   \
    }                                               \
  } while (0)

#else

#define ATOM_ASSERT(condition, message) \
  ((void)0)

#endif

void atom_assert(char const* file, int line, char const* message);

#ifdef __cplusplus
}
#endif