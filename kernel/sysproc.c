#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"
#include "vm.h"

extern struct proc proc[NPROC];
extern struct spinlock wait_lock;

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
sys_getprocinfo(void)
{
  struct procinfo info;
  struct proc *p = myproc();
  uint64 addr;

  argaddr(0, &addr);

  if(addr == 0) return -1;

  info.pid = p->pid;
  info.ppid = (p->parent) ? p->parent->pid : 0;
  strncpy(info.name, p->name, sizeof(info.name));
  info.state = p->state;

  if(copyout(p->pagetable, addr, (char *)&info, sizeof(info)) < 0) return -1;

  return 0;
}

uint64
sys_ps_listinfo(void)
{
  uint64 addr;
  int lim;
  int count = 0;
  int written = 0;
  struct proc *p;
  struct procinfo info;

  argaddr(0, &addr);
  argint(1, &lim);

  if(addr == 0) {
    for(p = proc; p < &proc[NPROC]; p++) {
      acquire(&p->lock);
      if(p->state != UNUSED) count++;
      release(&p->lock);
    }
    return count;
  }

  for(p = proc; p < &proc[NPROC]; p++) {
    acquire(&p->lock);
    if(p->state != UNUSED) count++;
    release(&p->lock);
  }

  if(count > lim) return -1;

  for(p = proc; p < &proc[NPROC]; p++) {

    if(written >= lim) return -1;

    acquire(&p->lock);

    if(p->state == UNUSED) {
      release(&p->lock);
      continue;
    }
    
    info.pid = p->pid;
    info.ppid = 0;

    struct proc *parent = p->parent;
    strncpy(info.name, p->name, sizeof(info.name));
    info.state = p->state;

    release(&p->lock);
    
    if(parent != 0) {
      acquire(&wait_lock);

      if(parent == p->parent) info.ppid = parent->pid;

      release(&wait_lock);
    }
    
    uint64 elem_addr = addr + written * sizeof(struct procinfo);
    if(copyout(myproc()->pagetable, elem_addr, (char *)&info, sizeof(info)) < 0) return -2;
    written++;
  }

  return written;
}
