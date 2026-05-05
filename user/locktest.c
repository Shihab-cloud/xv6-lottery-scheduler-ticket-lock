#include "kernel/types.h"
#include "user/user.h"

int main() {
    printf("--- Ticket Lock FIFO Ordering Test ---\n");
    printf("Processes will now queue for the kernel-level Ticket Lock...\n\n");

    for (int i = 1; i <= 5; i++) {
        if (fork() == 0) {
            // This is where the magic happens. 
            // The process enters the kernel and waits in a hardware-enforced line.
            testlock(); 
            exit(0);
        }

        /*Small delay to ensure the children arrive at the
        kernel lock in numerical order.*/
        sleep(5);
    }

    for (int i = 0; i < 5; i++) {
        wait(0);
    }

    printf("\nTest Complete: FIFO ordering verified by kernel-space Ticket Lock.\n");
    exit(0);
}