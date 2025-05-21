#include "user.h"

int main(int argc, char *argv[]) {
    // ALL fails prints are in the functions.
    if (argc != 2) {
        printf("Usage: tournament N\n");
        exit(1);
    }

    int processes = atoi(argv[1]);
    int process_id = tournament_create(processes);
    if (process_id < 0) exit(1);

    if (tournament_acquire() < 0) exit(1);

    // START CRITICAL SECTION
    printf("Process with tournament ID %d, PID %d is in critical section\n",
           process_id, getpid());
    // END CRITICAL SECTION

    if (tournament_release() < 0) exit(1);

    exit(0);
}
