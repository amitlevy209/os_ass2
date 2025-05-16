#include "spinlock.h"

#define MAXPETLOCKS 15


struct petersonlock {
  int active;        // 0=unused, 1=in use
  int flag[2];       // flag[role] indicates intent to enter CS
  int turn;          // whose turn it is when both want in
};

extern struct petersonlock peterson_locks[MAXPETLOCKS];

extern struct spinlock peterson_lock;

void peterson_init(void);
