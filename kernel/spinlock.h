// Mutual exclusion lock.
struct spinlock {
  uint locked;       // Is the lock held?

  // For debugging:
  char *name;        // Name of lock.
  struct cpu *cpu;   // The cpu holding the lock.
};

// --- ADDED FOR TICKET LOCK ---
struct ticketlock {
  uint ticket;       // Next available ticket
  uint turn;         // Ticket currently allowed to run
  struct cpu *cpu;   // The cpu holding the lock
  char *name;        // Name of lock
};