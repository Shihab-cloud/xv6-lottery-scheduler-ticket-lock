#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"
#include "vm.h"

uint64
sys_exit(void)
{
  int n;
  argint(0, &n);
  kexit(n);
  return 0;  // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return kfork();
}

uint64
sys_wait(void)
{
  uint64 p;
  argaddr(0, &p);
  return kwait(p);
}

uint64
sys_sbrk(void)
{
  uint64 addr;
  int t;
  int n;

  argint(0, &n);
  argint(1, &t);
  addr = myproc()->sz;

  if(t == SBRK_EAGER || n < 0) {
    if(growproc(n) < 0) {
      return -1;
    }
  } else {
    // Lazily allocate memory for this process: increase its memory
    // size but don't allocate memory. If the processes uses the
    // memory, vmfault() will allocate it.
    if(addr + n < addr)
      return -1;
    if(addr + n > TRAPFRAME)
      return -1;
    myproc()->sz += n;
  }
  return addr;
}

uint64
sys_pause(void)
{
  int n;
  uint ticks0;

  argint(0, &n);
  if(n < 0)
    n = 0;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(killed(myproc())){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

uint64
sys_kill(void)
{
  int pid;

  argint(0, &pid);
  return kkill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

// --- ADDED FOR LOTTERY SCHEDULER ---
uint64
sys_settickets(void)
{
  int n;
  
  // Read the integer argument passed from user space
  argint(0, &n);
    
  // Prevent a process from setting negative or zero tickets
  if(n < 1)
    return -1;

  // myproc() gets the currently running process
  myproc()->tickets = n;
  
  return 0; // Success
}


uint64
sys_getpinfo(void)
{
  uint64 pstat_addr; // The user pointer passed into the function
  
  // Get the pointer from user space
  argaddr(0, &pstat_addr);
  if(pstat_addr == 0)
    return -1;
    
  // Call the helper function we wrote in proc.c
  return getpinfo(pstat_addr);
}

uint64
sys_sleep(void)
{
  int n;
  argint(0, &n);
  
  // Simple loop to yield the CPU and pass time safely
  for(int i = 0; i < n; i++) {
    yield();
  }
  
  return 0;
}



struct ticketlock global_test_lock;
int lock_init_done = 0;

uint64
sys_testlock(void)
{
  // Initialize the lock only once
  if(lock_init_done == 0){
    initticketlock(&global_test_lock, "test lock");
    lock_init_done = 1;
  }

  // THE FIFO QUEUE: Processes arrive and grab a ticket
  acquireticket(&global_test_lock);
  
  printf("Process %d: Captured Ticket Lock!\n", myproc()->pid);
  
  // Hold the lock for a short duration to ensure a queue forms
  for(int i = 0; i < 500000; i++) {
     __asm__ volatile("nop"); 
  }

  releaseticket(&global_test_lock);

  // ADD THIS FOR YOUR REPORT:
  printf("Process %d: Released Ticket Lock.\n", myproc()->pid);

  return 0;
}