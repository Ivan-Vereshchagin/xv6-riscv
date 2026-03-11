#ifndef _MUTEX_H_
#define _MUTEX_H_

#include "types.h"
#include "spinlock.h"
#include "sleeplock.h"

struct mutex {
  int ref;
  struct spinlock lock;
  struct sleeplock lk;
};

struct file* mutexalloc(void);
void mutexclose(struct mutex*);

#endif