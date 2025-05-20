#include "types.h"
#include "user.h"     

#define MAX_PROCS   16

static int    T_n;        
static int    T_levels;   
static int    T_nlocks;   
static int   *T_locks;    
static int    T_index;    


int
tournament_create(int processes)
{
  // 1) validate that processes is a power of two ≤ MAX_PROCS
  if(processes <= 0 ||
     processes > MAX_PROCS ||
     (processes & (processes - 1)) != 0)
    return -1;

  // 2) compute levels = log2(processes)
  T_n = processes;
  T_levels = 0;
  for(int t = processes; t > 1; t >>= 1)
    T_levels++;
  T_nlocks = T_n - 1;

  // 3) allocate array to hold underlying Peterson‐lock IDs
  T_locks = malloc(T_nlocks * sizeof(int));
  if(!T_locks)
    return -1;

  // 4) create each Peterson lock
  for(int i = 0; i < T_nlocks; i++){
    int id = peterson_create();
    if(id < 0)
      return -1;
    T_locks[i] = id;
  }

  // 5) fork out exactly T_n – 1 children (parent keeps index 0)
  T_index = 0;
  for(int i = 1; i < T_n; i++){
    int pid = fork();
    if(pid < 0)
      return -1;
    if(pid == 0){
      // child #i
      T_index = i;
      break;
    }
  }

  // 6) each of the T_n processes returns its unique leaf index
  return T_index;
}


int
tournament_acquire(void)
{
  if(T_index < 0) return -1;
  for(int l = 0; l < T_levels; l++){
    int role      = (T_index >> (T_levels - l - 1)) & 1;
    int group     = T_index >> (T_levels - l);
    int lock_idx  = group + ((1 << l) - 1);
    if(peterson_acquire(T_locks[lock_idx], role) < 0)
      return -1;
  }
  return 0;
}

int
tournament_release(void)
{
  if(T_index < 0) return -1;
  for(int l = T_levels - 1; l >= 0; l--){
    int role      = (T_index >> (T_levels - l - 1)) & 1;
    int group     = T_index >> (T_levels - l);
    int lock_idx  = group + ((1 << l) - 1);
    if(peterson_release(T_locks[lock_idx], role) < 0)
      return -1;
  }
  return 0;
}
