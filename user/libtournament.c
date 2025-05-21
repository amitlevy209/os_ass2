#include "user/user.h"
#include "kernel/syscall.h"

int T_levels;
int T_index;
int *T_locks;
int **T_info;

int
tournament_create(int processes)
{
    // Validate: processes must be power of 2 ≤ 16
    if (!(processes == 2 || processes == 4 || processes == 8 || processes == 16)) {
        printf("ERROR: num of processes must be power of 2 up to 16\n");
        return -1;
    }

    // Calculate levels (log2(processes))
    if (processes == 2) T_levels = 1;
    else if (processes == 4) T_levels = 2;
    else if (processes == 8) T_levels = 3;
    else T_levels = 4;

    // Allocate lock IDs
    T_locks = malloc((processes - 1) * sizeof(int));
    if (T_locks == 0) {
        printf("ERROR: malloc for T_locks failed\n");
        return -1;
    }

    // Create locks
    for (int i = 0; i < processes - 1; i++) {
        T_locks[i] = peterson_create();
        if (T_locks[i] == -1) {
            printf("ERROR: peterson_create failed at lock %d\n", i);
            return -1;
        }
    }

    // Fork processes
    int i;
    for (i = 1; i < processes; i++) {
        int pid = fork();
        if (pid < 0) {
            printf("ERROR: fork failed\n");
            return -1;
        }
        if (pid == 0) {
            T_index = i;
            break;
        }
    }

    // Parent gets index 0
    if (T_index != i)
        T_index = 0;

    // Allocate T_info[L][2] -> [role, lock_id]
    T_info = malloc(T_levels * sizeof(int *));
    if (T_info == 0) {
        printf("ERROR: malloc for T_info failed\n");
        return -1;
    }

    for (int j = 0; j < T_levels; j++) {
        T_info[j] = malloc(2 * sizeof(int));
        if (T_info[j] == 0) {
            printf("ERROR: malloc for T_info[%d] failed\n", j);
            return -1;
        }
    }

    // Fill role and lock_id per level
    for (int l = 0; l < T_levels; l++) {
        int role = (T_index >> (T_levels - l - 1)) & 1;
        int lock_idx = (T_index >> (T_levels - l)) + ((1 << l) - 1);
        T_info[l][0] = role;
        T_info[l][1] = T_locks[lock_idx];
    }

    return T_index;
}

int
tournament_acquire(void)
{
    for (int l = T_levels - 1; l >= 0; l--) {
        if (peterson_acquire(T_info[l][1], T_info[l][0]) < 0) {
            printf("ERROR: failed to acquire lock %d\n", T_info[l][1]);
            return -1;
        }
    }
    return 0;
}

int
tournament_release(void)
{
    for (int l = 0; l < T_levels; l++) {
        if (peterson_release(T_info[l][1], T_info[l][0]) < 0) {
            printf("ERROR: failed to release lock %d\n", T_info[l][1]);
            return -1;
        }
    }
    return 0;
}
