#include "kernel/param.h"
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/pstat.h"

// Helper function to print a specific character N times
void print_chars(char c, int count) {
    for (int i = 0; i < count; i++) {
        printf("%c", c);
    }
}

// The Gantt Chart Generator
void draw_gantt_chart(int pids[], int ticks[], int num_procs) {
    int total_ticks = 0;
    int max_terminal_width = 60; // Keep it under 80 to prevent line wrapping
    int widths[10];              // Array to hold scaled widths
    
    // 1. Calculate total ticks
    for (int i = 0; i < num_procs; i++) {
        total_ticks += ticks[i];
    }

    // 2. Calculate integer-scaled widths for the terminal
    for (int i = 0; i < num_procs; i++) {
        // Integer math equivalent of: (ticks / total) * max_width
        widths[i] = (ticks[i] * max_terminal_width) / total_ticks;
        // Ensure at least 1 character width for tiny bursts
        if(widths[i] < 3) widths[i] = 3; 
    }

    printf("\n--- Scaled CPU Execution Gantt Chart ---\n\n");

    // --- TOP BORDER ---
    for (int i = 0; i < num_procs; i++) {
        printf("+");
        print_chars('-', widths[i]);
    }
    printf("+\n");

    // --- PROCESS IDs (Centered) ---
    for (int i = 0; i < num_procs; i++) {
        printf("|");
        int padding_left = (widths[i] - 4) / 2; // 4 is length of "PXX "
        int padding_right = widths[i] - 4 - padding_left;
        
        print_chars(' ', padding_left);
        printf("P%d", pids[i]);
        print_chars(' ', padding_right);
    }
    printf("|\n");

    // --- BOTTOM BORDER ---
    for (int i = 0; i < num_procs; i++) {
        printf("+");
        print_chars('-', widths[i]);
    }
    printf("+\n");
}

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

    int chart_pids[3];
    int chart_ticks[3];
    int count = 0;

    printf("\n--- CPU Fairness Test Results ---\n");
    printf("PID\tTickets\tTicks\n");
    
    // Print the results only for our 3 child processes
    for (int i = 0; i < NPROC; i++) {
        if (st.inuse[i]) {
            for(int j = 0; j < 3; j++) {
                if(st.pid[i] == pid[j]) {
                    
                    // Print the table row to the terminal
                    printf("%d\t%d\t%d\n", st.pid[i], st.tickets[i], st.ticks[i]);
                    
                    // Capture the data into the arrays we created above
                    if (count < 3) {
                        chart_pids[count] = st.pid[i];
                        chart_ticks[count] = st.ticks[i];
                        count++;
                    }
                }
            }
        }
    }

    // Cleanup: kill the infinite loop children and wait for them to die
    for (int i = 0; i < 3; i++) {
        kill(pid[i]);
        wait(0);
    }


    // NEW: Just call the chart using your real arrays!
    draw_gantt_chart(chart_pids, chart_ticks, 3);

    exit(0);
}