#include <atom.h>

typedef struct
{
  char const* name;
  bool has_driving_licence;
}user_t;

static void buy_car(user_t const* user)
{
  ATOM_ASSERT(user->has_driving_licence, "%s does not have a driving licence", user->name);
  printf("%s bought a car.\r\n", user->name);
}

int main(void)
{
  user_t const paul = { .name = "Paul", .has_driving_licence = true };
  user_t const john = { .name = "John", .has_driving_licence = false };

  buy_car(&paul);
  buy_car(&john);

  while (1)
  {
  }
}