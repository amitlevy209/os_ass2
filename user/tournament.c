#include "types.h"
#include "user.h"   

int
main(int argc, char *argv[])
{
  if(argc != 2){
    printf("Usage: tournament N\n");
    exit(1);
  }

  int n  = atoi(argv[1]);
  int id = tournament_create(n);
  if(id < 0){
    printf("ERROR: tournament_create(%d) failed\n", n);
    exit(1);
  }

  // Only one critical‐section entry per process, then exit:
  tournament_acquire();
  printf("Process with tournament ID %d, PID %d is in critical section\n",
         id, getpid());
  tournament_release();

  exit(0);
}
