#include "types.h"
#include "param.h"
#include "spinlock.h"
#include "riscv.h"
#include "defs.h"
#include <stdarg.h>

#define DIAG_BUF_SIZE (DIAG_PAGES * PGSIZE)

char diag_buf[DIAG_BUF_SIZE];
uint diag_head = 0;
uint diag_tail = 0;
struct spinlock diag_lock;

int log_mask = 0;
int log_duration = 0;
int log_start_tick = 0;
struct spinlock logctl_lock;

void
diag_init(void)
{
  initlock(&diag_lock, "diag");
  diag_head = 0;
  diag_tail = 0;
}

void
diag_putc(char c)
{
  diag_buf[diag_tail] = c;
  diag_tail = (diag_tail + 1) % DIAG_BUF_SIZE;
  
  if (diag_tail == diag_head) diag_head = (diag_head + 1) % DIAG_BUF_SIZE;
}

static void
diag_putnum(uint64 n, int base, int sign)
{
  char buf[16];
  int i = 0;
  uint64 tmp = n;
  const char digits[] = "0123456789abcdef";

  if (sign && (long)n < 0) {
    diag_putc('-');
    tmp = -tmp;
  }

  do {
    buf[i++] = digits[tmp % base];
  } while ((tmp /= base) != 0);

  while (--i >= 0) diag_putc(buf[i]);
}

void
pr_msg(const char *fmt, ...)
{
  va_list ap;
  va_start(ap, fmt);

  acquire(&tickslock);
  uint64 t = ticks;
  release(&tickslock);

  acquire(&diag_lock);

  diag_putc('[');
  diag_putnum(t, 10, 0);
  diag_putc(']');
  diag_putc(' ');

  const char *s;
  for (; *fmt; fmt++) {
    if (*fmt != '%') {
      diag_putc(*fmt);
      continue;
    }
    fmt++;
    switch (*fmt) {
    case 'd':
      diag_putnum(va_arg(ap, int), 10, 1);
      break;
    case 'x':
      diag_putnum(va_arg(ap, int), 16, 0);
      break;
    case 'p':
      diag_putc('0'); diag_putc('x');
      diag_putnum((uint64)va_arg(ap, void*), 16, 0);
      break;
    case 's':
      if ((s = va_arg(ap, char*)) == 0) s = "(null)";
      for (; *s; s++) diag_putc(*s);
      break;
    case '%':
      diag_putc('%');
      break;
    default:
      diag_putc('%');
      diag_putc(*fmt);
      break;
    }
  }

  diag_putc('\n');
  release(&diag_lock);
  va_end(ap);
}

void logctl_init(void) {
  initlock(&logctl_lock, "logctl");
}

int log_is_enabled(int class) {
  int enabled = 0;
  acquire(&logctl_lock);
  if (log_mask & class) {
    enabled = 1;
    if (log_duration > 0) {
      acquire(&tickslock);
      int now = ticks;
      release(&tickslock);
      if (now >= log_start_tick + log_duration) {
        log_mask &= ~class;
        enabled = 0;
      }
    }
  }

  release(&logctl_lock);
  return enabled;
}

static const char* syscall_names[] = { "unused_zero_index",
  "fork", "exit", "wait", "pipe", "read", "kill", "exec", "fstat",
  "chdir", "dup", "getpid", "sbrk", "pause", "uptime", "open",
  "write", "mknod", "unlink", "link", "mkdir", "close", "dmesg",
  "logctl"
};

const char* syscall_name(int n) {
  if (n >= 0 && n < sizeof(syscall_names) / sizeof(syscall_names[0])) return syscall_names[n];
  return "unknown";
}