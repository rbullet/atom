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
