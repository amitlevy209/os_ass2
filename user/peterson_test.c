// user/peterson_test.c
#include "types.h"
#include "user.h"

int
main(void)
{
  int lock = peterson_create();
  if(lock < 0){
    printf("peterson_create failed\n");
    exit(0);
  }

  int pid = fork();
  int role = pid > 0 ? 0 : 1;

  for(int i = 0; i < 20; i++){
    peterson_acquire(lock, role);
    printf(role == 0 ? "P%d\n" : "C%d\n", i);
    peterson_release(lock, role);
  }

  if(pid > 0){
    wait(0);
    peterson_destroy(lock);
  }
  exit(0);
}
