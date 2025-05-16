#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"
#include "peterson.h"

uint64
sys_exit(void)
{
  int n;
  argint(0, &n);
  exit(n);
  return 0;  // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return fork();
}

uint64
sys_wait(void)
{
  uint64 p;
  argaddr(0, &p);
  return wait(p);
}

uint64
sys_sbrk(void)
{
  uint64 addr;
  int n;

  argint(0, &n);
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

uint64
sys_sleep(void)
{
  int n;
  uint ticks0;

  argint(0, &n);
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(killed(myproc())){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

uint64
sys_kill(void)
{
  int pid;

  argint(0, &pid);
  return kill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

uint64
sys_peterson_create(void)
{
  acquire(&peterson_lock);
  for(int i = 0; i < MAXPETLOCKS; i++){
    if(!peterson_locks[i].active){
      peterson_locks[i].active  = 1;
      peterson_locks[i].flag[0] = peterson_locks[i].flag[1] = 0;
      peterson_locks[i].turn    = 0;
      release(&peterson_lock);
      return i;
    }
  }
  release(&peterson_lock);
  return -1;
}

uint64
sys_peterson_acquire(void)
{
  int id, role;
 
  argint(0, &id);
  argint(1, &role);

 
  if (id < 0 || id >= MAXPETLOCKS || (role != 0 && role != 1))
    return -1;

  struct petersonlock *lk = &peterson_locks[id];
  if (!lk->active)
    return -1;

  int other = 1 - role;
  __sync_synchronize();
  __sync_lock_test_and_set(&lk->flag[role], 1);
  __sync_synchronize();
  lk->turn = other;

    __sync_synchronize();
  
    while (lk->flag[other] && lk->turn == other) {
      yield();
    }
  
  return 0;
}

uint64
sys_peterson_release(void)
{
  int id, role;
  argint(0, &id);
  argint(1, &role);
  if (id < 0 || id >= MAXPETLOCKS || (role != 0 && role != 1))
    return -1;

  struct petersonlock *lk = &peterson_locks[id];
  if (!lk->active)
    return -1;

  __sync_synchronize();
  __sync_lock_release(&lk->flag[role]);
  __sync_synchronize();
  return 0;
}

uint64
sys_peterson_destroy(void)
{
  int id;
  argint(0, &id);
  if (id < 0 || id >= MAXPETLOCKS)
    return -1;

  acquire(&peterson_lock);
  if (!peterson_locks[id].active) {
    release(&peterson_lock);
    return -1;
  }
  peterson_locks[id].active = 0;
  release(&peterson_lock);
  return 0;
}
