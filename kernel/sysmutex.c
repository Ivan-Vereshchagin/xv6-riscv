#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "file.h"
#include "mutex.h"
#include "proc.h"

uint64
sys_mutex(void)
{
  struct file *f;
  int fd;

  if ((f = mutexalloc()) == 0) return -1;

  if ((fd = fdalloc(f)) < 0) {
    fileclose(f);
    return -1;
  }

  return fd;
}

uint64
sys_mutex_lock(void)
{
  int fd;
  struct file *f;

  argint(0, &fd);

  if (fd < 0 || fd >= NOFILE) return -1;

  f = myproc()->ofile[fd];
  if (f == 0 || f->type != FD_MUTEX) return -1;

  acquiresleep(&f->mutex->lk);

  return 0;
}

uint64
sys_mutex_unlock(void)
{
  int fd;
  struct file *f;

  argint(0, &fd);

  if (fd < 0 || fd >= NOFILE) return -1;

  f = myproc()->ofile[fd];
  if (f == 0 || f->type != FD_MUTEX) return -1;

  if (!holdingsleep(&f->mutex->lk)) return -1;

  releasesleep(&f->mutex->lk);

  return 0;
}