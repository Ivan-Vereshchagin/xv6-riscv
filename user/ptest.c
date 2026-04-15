#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/riscv.h"

int global_var = 42;
int global_array[100];

void
check_flags(char *location, void *addr, uint64 len)
{
  int ret;
  
  printf("%s:\n", location);
  
  ret = pagetablecheck(addr, len, PTE_R);
  printf("  R: %d\n", ret);
  
  ret = pagetablecheck(addr, len, PTE_W);
  printf("  W: %d\n", ret);
  
  ret = pagetablecheck(addr, len, PTE_X);
  printf("  X: %d\n", ret);
  
  ret = pagetablecheck(addr, len, PTE_U);
  printf("  U: %d\n", ret);
  
  ret = pagetablecheck(addr, len, PTE_A);
  printf("  A: %d\n", ret);
  
  ret = pagetablecheck(addr, len, PTE_D);
  printf("  D: %d\n", ret);
}

int
main(int argc, char *argv[])
{
  printf("Initial page table:\n");
  pagetableprint();
  
  printf("\nAfter reading global variables:\n");
  pagetableprint();
  check_flags("global_var", &global_var, sizeof(global_var));

  global_var = 100;
  global_array[0] = 999;
  
  printf("\nAfter writing global variables:\n");
  pagetableprint();
  check_flags("global_var", &global_var, sizeof(global_var));
  check_flags("global_array", global_array, sizeof(global_array));

  int stack_var = 123;
  printf("\nAfter creating stack variable:\n");
  pagetableprint();
  check_flags("stack_var", &stack_var, sizeof(stack_var));

  volatile int x = stack_var;
  (void)x;
  stack_var = 456;
  
  printf("\nAfter read/write stack variable:\n");
  pagetableprint();
  check_flags("stack_var", &stack_var, sizeof(stack_var));

  int stack_array[500];
  for (int i = 0; i < 500; i++) stack_array[i] = i * 2;
  
  printf("\nAfter creating stack array:\n");
  pagetableprint();
  check_flags("stack_array", stack_array, sizeof(stack_array));

  char *heap_array = malloc(16384);
  if (heap_array == 0) {
    fprintf(2, "Error: malloc failed\n");
    exit(1);
  }

  printf("\nAfter malloc:\n");
  pagetableprint();
  check_flags("heap_array", heap_array, 16384);

  for (int i = 0; i < 16384; i++) heap_array[i] = (char)i;
  
  printf("\nAfter writing to heap array:\n");
  pagetableprint();
  check_flags("heap_array", heap_array, 16384);

  pagetableclear(heap_array, 16384, PTE_A | PTE_D);
  pagetableclear(stack_array, sizeof(stack_array), PTE_A | PTE_D);
  pagetableclear(&global_var, sizeof(global_var), PTE_A | PTE_D);
  
  printf("\nAfter clearing A and D:\n");
  pagetableprint();
  check_flags("heap_array", heap_array, 16384);
  check_flags("stack_array", stack_array, sizeof(stack_array));
  check_flags("global_var", &global_var, sizeof(global_var));
  
  volatile char c = heap_array[0];
  (void)c;
  volatile int gv = global_var;
  (void)gv;
  volatile int sa = stack_array[0];
  (void)sa;
  
  printf("\nAfter reading:\n");
  pagetableprint();
  check_flags("heap_array", heap_array, 16384);
  check_flags("stack_array", stack_array, sizeof(stack_array));
  check_flags("global_var", &global_var, sizeof(global_var));
  
  heap_array[0] = 255;
  global_var = 777;
  stack_array[0] = 888;
  
  printf("\nAfter writing:\n");
  pagetableprint();
  check_flags("heap_array", heap_array, 16384);
  check_flags("stack_array", stack_array, sizeof(stack_array));
  check_flags("global_var", &global_var, sizeof(global_var));
  
  free(heap_array);
  heap_array = 0;
  
  printf("\nAfter free:\n\n");
  pagetableprint();
  
  printf("global_var = %d\n", global_var);
  printf("stack_var = %d\n", stack_var);
  printf("stack_array[0] = %d\n", stack_array[0]);
  
  exit(0);
}