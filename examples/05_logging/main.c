#include <atom.h>

int main(void)
{
  log_set_min_level(LOG_LEVEL_DEBUG);

  log_info("Starting the application");

  char const* player_name = "Alice";
  uint32_t health = 100;
  log_debug("Player %s started with %u HP", player_name, health);

  health = 20;
  log_warn("Player health is low: %u HP remaining", health);

  //...

  log_error("Failed to save your progress");

  while (1)
  {
  }
}