#include "peterson.h"

struct petersonlock peterson_locks[MAXPETLOCKS];

void
peterson_init(void)
{
  for(int i = 0; i < MAXPETLOCKS; i++){
    peterson_locks[i].active      = 0;
    peterson_locks[i].flag[0]     = 0;
    peterson_locks[i].flag[1]     = 0;
    peterson_locks[i].turn        = 0;
  }
}
