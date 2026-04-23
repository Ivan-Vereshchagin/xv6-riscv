#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "sleeplock.h"
#include "riscv.h"
#include "defs.h"
#include "fs.h"
#include "file.h"
#include "pseudo.h"

struct {
  struct spinlock lock;
  uint64 seed;
} urandom_state;

struct {
  struct spinlock lock;
  uint64 written_bytes;
} nullstat_state;

void
pseudostateinit(void)
{
  initlock(&urandom_state.lock, "urandom");
  urandom_state.seed = 1;
  
  initlock(&nullstat_state.lock, "nullstat");
  nullstat_state.written_bytes = 0;
}

static uint64
lcg_next(uint64 *seed)
{
  *seed = (*seed * 1103515245 + 12345) & 0x7FFFFFFF;
  return *seed;
}

int
pseudoread(int major, uint64 addr, int n, int minor)
{
  uint64 seed;
  char byte;
  
  switch(minor) {

  case MINOR_NULL: return 0;

  case MINOR_ZERO:
    static char zero_buf[512];
    uint64 total = 0;

    while (total < n) {
      int to_copy = (n - total > sizeof(zero_buf)) ? sizeof(zero_buf) : (n - total);

      if (either_copyout(1, addr + total, zero_buf, to_copy) == -1) return (total == 0) ? -1 : total;
      
      total += to_copy;
    }
    
    return n;

  case MINOR_URANDOM:
    acquire(&urandom_state.lock);
    seed = urandom_state.seed;
    
    for (int i = 0; i < n; i++) {
      seed = lcg_next(&seed);
      byte = (char)(seed & 0xFF);
      if (either_copyout(1, addr + i, &byte, 1) == -1) {
        urandom_state.seed = seed;
        release(&urandom_state.lock);
        return i;
      }
    }
    urandom_state.seed = seed;
    release(&urandom_state.lock);
    return n;

  case MINOR_NULLSTAT:
    if (n != sizeof(uint64)) return -1;
    
    acquire(&nullstat_state.lock);
    uint64 count = nullstat_state.written_bytes;
    release(&nullstat_state.lock);
    
    if(either_copyout(1, addr, &count, sizeof(uint64)) == -1) return -1;
    return sizeof(uint64);

  default: return -1;
  }
}

int
pseudowrite(int major, uint64 addr, int n, int minor)
{
  uint64 seed;
  
  switch(minor) {

  case MINOR_NULL: return n;

  case MINOR_ZERO: return -1;

  case MINOR_URANDOM:
    if(n != sizeof(uint64)) return -1;
    
    if (either_copyin(&seed, 1, addr, sizeof(uint64)) == -1) return -1;
    
    acquire(&urandom_state.lock);
    urandom_state.seed = seed;
    release(&urandom_state.lock);
    return n;

  case MINOR_NULLSTAT:
    acquire(&nullstat_state.lock);
    nullstat_state.written_bytes += n;
    release(&nullstat_state.lock);
    return n;

  default: return -1;
  }
}

void
pseudoinit(void)
{
  pseudostateinit();

  devsw[PSEUDO_MAJOR].read = pseudoread;
  devsw[PSEUDO_MAJOR].write = pseudowrite;
}
