#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "file.h"
#include "mutex.h"
#include "sleeplock.h"
#include "spinlock.h"
#include "proc.h"

struct file*
mutexalloc(void)
{
  struct file *f;
  struct mutex *m;

  if ((f = filealloc()) == 0) return 0;

  if ((m = (struct mutex*)kalloc()) == 0) {
    fileclose(f);
    return 0;
  }

  initsleeplock(&m->lk, "mutex");

  f->type = FD_MUTEX;
  f->mutex = m;
  f->readable = 1;
  f->writable = 1;

  printf("MUTEX_ALLOC: mutex=%p file=%p pid=%d\n", m, f, myproc()->pid);

  return f;
}

void
mutexclose(struct mutex *m)
{

  printf("MUTEX_CLOSE: mutex=%p pid=%d\n", m, myproc()->pid);
  
  kfree((char*)m);
  
  printf("MUTEX_KFREE: mutex=%p\n", m);
}