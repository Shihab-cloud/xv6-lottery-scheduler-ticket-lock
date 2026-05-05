#include "kernel/param.h"
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/pstat.h"

int main() {
    int pid[3];
    int tickets[3] = {30, 20, 10}; // 3:2:1 ratio

    printf("Starting 3 processes with tickets 30, 20, and 10...\n");

    for (int i = 0; i < 3; i++) {
        pid[i] = fork();
        if (pid[i] == 0) {
            // Child process: set tickets and do an infinite loop
            settickets(tickets[i]);
            while (1) {
                // Infinite loop to consume CPU time
            }
        }
    }

    // Parent process: let the children run for a while (approx 100 ticks)
    sleep(100); 

    // Gather the statistics from the kernel
    struct pstat st;
    getpinfo(&st);

    printf("\n--- CPU Fairness Test Results ---\n");
    printf("PID\tTickets\tTicks\n");
    
    // Print the results only for our 3 child processes
    for (int i = 0; i < NPROC; i++) {
        if (st.inuse[i]) {
            for(int j = 0; j < 3; j++) {
                if(st.pid[i] == pid[j]) {
                    printf("%d\t%d\t%d\n", st.pid[i], st.tickets[i], st.ticks[i]);
                }
            }
        }
    }

    // Cleanup: kill the infinite loop children and wait for them to die
    for (int i = 0; i < 3; i++) {
        kill(pid[i]);
        wait(0);
    }

    exit(0);
}