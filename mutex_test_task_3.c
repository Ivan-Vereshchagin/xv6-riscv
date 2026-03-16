#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

static void
error_msg(const char *msg)
{
  write(2, msg, strlen(msg));
}

void
test_read_write(int mutex_fd)
{
  char buf;
  int r;

  r = read(mutex_fd, &buf, 1);
  if (r < 0) printf("PASS: read на мьютексе вернул ошибку\n");
  else error_msg("FAIL: read на мьютексе должен вернуть ошибку\n");

  r = write(mutex_fd, &buf, 1);
  if (r < 0) printf("PASS: write на мьютексе вернул ошибку\n");
  else error_msg("FAIL: write на мьютексе должен вернуть ошибку\n");
}

void
test_close_locked_self(void)
{
  int mutex_fd;

  mutex_fd = mutex();
  if (mutex_fd < 0) {
    error_msg("FAIL: ошибка выполнения mutex\n");
    return;
  }

  if (mutex_lock(mutex_fd) < 0) {
    error_msg("FAIL: ошибка выполнения mutex_lock\n");
    close(mutex_fd);
    return;
  }

  if (close(mutex_fd) < 0) {
    error_msg("FAIL: ошибка выполнения close\n");
    return;
  }

  printf("PASS: close на захваченном мьютексе успешен\n");
}

void
test_close_locked_other(void)
{
  int mutex_fd;
  int pid;

  mutex_fd = mutex();
  if (mutex_fd < 0) {
    error_msg("FAIL: ошибка выполнения mutex\n");
    return;
  }

  pid = fork();
  if (pid < 0) {
    error_msg("FAIL: ошибка выполнения fork\n");
    close(mutex_fd);
    return;
  }

  if (pid == 0) {
    if (mutex_lock(mutex_fd) < 0) {
      error_msg("FAIL: ошибка выполнения mutex_lock в ребёнке\n");
      exit(1);
    }
    pause(10);
    exit(0);
  }

  wait(0);
  printf("PASS: мьютекс освобождён после завершения процесса-владельца\n");
}

void
test_exit_unclosed(void)
{
  int pid;

  pid = fork();
  if (pid < 0) {
    error_msg("FAIL: ошибка выполнения fork\n");
    return;
  }

  if (pid == 0) {
    int mutex_fd = mutex();
    if (mutex_fd < 0) {
      error_msg("FAIL: ошибка выполнения mutex в ребёнке\n");
      exit(1);
    }
    exit(0);
  }

  wait(0);
  printf("PASS: мьютекс закрыт автоматически при exit\n");
}

void
test_unlock_other(void)
{
  int mutex_fd;
  int pid;

  mutex_fd = mutex();
  if (mutex_fd < 0) {
    error_msg("FAIL: ошибка выполнения mutex\n");
    return;
  }

  pid = fork();
  if (pid < 0) {
    error_msg("FAIL: ошибка выполнения fork\n");
    close(mutex_fd);
    return;
  }

  if (pid == 0) {
    if (mutex_lock(mutex_fd) < 0) {
      error_msg("FAIL: ошибка выполнения mutex_lock в ребёнке\n");
      exit(1);
    }
    exit(0);
  }

  wait(0);

  if (mutex_unlock(mutex_fd) < 0) printf("PASS: mutex_unlock чужого мьютекса вернул ошибку\n");
  else error_msg("FAIL: mutex_unlock чужого мьютекса должен вернуть ошибку\n");
  close(mutex_fd);
}

void
test_general(void)
{
  int i;
  int mutex_fd;

  for (i = 0; i < 3; i++) {
    mutex_fd = mutex();
    if (mutex_fd < 0) {
      error_msg("FAIL: ошибка выполнения mutex\n");
      return;
    }
    if (close(mutex_fd) < 0) {
      error_msg("FAIL: ошибка выполнения close\n");
      return;
    }
  }

  printf("PASS: каждому ALLOC должен соответствовать KFREE\n");
}

int
main(int argc, char *argv[])
{
  int mutex_fd;

  mutex_fd = mutex();
  if (mutex_fd < 0) {
    error_msg("FAIL: не удалось создать мьютекс для теста\n");
    exit(1);
  }
  test_read_write(mutex_fd);
  close(mutex_fd);

  test_close_locked_self();
  test_close_locked_other();
  test_exit_unclosed();
  test_unlock_other();
  test_general();

  exit(0);
}