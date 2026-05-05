#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"
#include "vm.h"

#define DIAG_BUF_SIZE (DIAG_PAGES * PGSIZE)

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

uint64
sys_dmesg(void)
{
  uint64 ubuf;
  int len;
  argaddr(0, &ubuf);
  argint(1, &len);
  if (len <= 0) return -1;

  acquire(&diag_lock);

  if (diag_head == diag_tail) {
    release(&diag_lock);
    if (copyout(myproc()->pagetable, ubuf, "\0", 1) < 0) return -1;
    return 0;
  }

  int search = diag_head;
  while (search != diag_tail && diag_buf[search] != '\n')
    search = (search + 1) % DIAG_BUF_SIZE;

  int start = diag_head;
  if (search != diag_tail) {
    start = (search + 1) % DIAG_BUF_SIZE;
    if (start == diag_tail) start = diag_head;
  }

  int avail = (diag_tail - start + DIAG_BUF_SIZE) % DIAG_BUF_SIZE;
  int to_copy = (avail < len - 1) ? avail : len - 1;
  int copied = 0;

  if (to_copy > 0) {
    if (start + to_copy <= DIAG_BUF_SIZE) {
      if (copyout(myproc()->pagetable, ubuf, &diag_buf[start], to_copy) < 0) {
        release(&diag_lock);
        return -1;
      }
      copied = to_copy;
    } else {
      int first = DIAG_BUF_SIZE - start;
      int second = to_copy - first;
      if (copyout(myproc()->pagetable, ubuf, &diag_buf[start], first) < 0 ||
          copyout(myproc()->pagetable, ubuf + first, &diag_buf[0], second) < 0) {
        release(&diag_lock);
        return -1;
      }
      copied = to_copy;
    }
  }

  if (copyout(myproc()->pagetable, ubuf + copied, "\0", 1) < 0) {
    release(&diag_lock);
    return -1;
  }

  release(&diag_lock);
  return copied;
}

uint64
sys_logctl(void)
{
  int mask, duration;
  argint(0, &mask);
  argint(1, &duration);

  acquire(&logctl_lock);
  if (mask != -1) log_mask = mask;
  if (duration != -1) {
    log_duration = duration;
    acquire(&tickslock);
    log_start_tick = ticks;
    release(&tickslock);
  }
  release(&logctl_lock);
  return 0;
}
