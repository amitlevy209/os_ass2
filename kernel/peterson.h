#define MAXPETLOCKS 15

struct petersonlock {
  int active;        // 0=unused, 1=in use
  int flag[2];       // flag[role] indicates intent to enter CS
  int turn;          // whose turn it is when both want in
};

// Global array of locks
extern struct petersonlock peterson_locks[MAXPETLOCKS];
