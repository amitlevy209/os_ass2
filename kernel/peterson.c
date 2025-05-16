#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "riscv.h"
#include "spinlock.h"
#include "peterson.h"
#include "defs.h"

struct spinlock peterson_lock;
struct petersonlock peterson_locks[MAXPETLOCKS];

void
peterson_init(void)
{
  // initialize the spinlock
  initlock(&peterson_lock, "peterson");

  // zero out all slots
  for(int i = 0; i < MAXPETLOCKS; i++){
    peterson_locks[i].active  = 0;
    peterson_locks[i].flag[0] = peterson_locks[i].flag[1] = 0;
    peterson_locks[i].turn    = 0;
  }
}
