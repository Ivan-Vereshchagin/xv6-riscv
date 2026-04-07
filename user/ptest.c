#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/riscv.h"

int global_var = 42;
int global_array[100];

int
main(int argc, char *argv[])
{
  printf("Initial page table:\n");
  pagetableprint();
  
  printf("\nAfter reading global variables:\n");
  pagetableprint();

  global_var = 100;
  global_array[0] = 999;
  
  printf("\nAfter writing global variables:\n");
  pagetableprint();

  int stack_var = 123;
  printf("\nAfter creating stack variable:\n");
  pagetableprint();

  volatile int x = stack_var;
  (void)x;
  stack_var = 456;
  
  printf("\nAfter read/write stack variable:\n");
  pagetableprint();

  int stack_array[500];
  for (int i = 0; i < 500; i++) stack_array[i] = i * 2;
  
  printf("\nAfter creating stack array:\n");
  pagetableprint();

  char *heap_array = malloc(16384);
  if (heap_array == 0) {
    fprintf(2, "Error: malloc failed\n");
    exit(1);
  }

  printf("\nAfter malloc:\n");
  pagetableprint();

  for (int i = 0; i < 16384; i++) heap_array[i] = (char)i;
  
  printf("\nAfter writing to heap array:\n");
  pagetableprint();

  pagetableclear(heap_array, 16384, PTE_A | PTE_D);
  
  pagetableclear(stack_array, sizeof(stack_array), PTE_A | PTE_D);
  
  printf("\nAfter clearing A and D:\n");
  pagetableprint();
  
  volatile char c = heap_array[0];
  (void)c;
  volatile int gv = global_var;
  (void)gv;
  volatile int sa = stack_array[0];
  (void)sa;
  
  printf("\nAfter reading:\n");
  pagetableprint();
  
  heap_array[0] = 255;
  global_var = 777;
  stack_array[0] = 888;
  
  printf("\nAfter writing:\n");
  pagetableprint();
  
  int ret;
  
  ret = pagetablecheck(heap_array, 16384, PTE_A);
  printf("Check A on heap_array: %d\n", ret);
  
  ret = pagetablecheck(heap_array, 16384, PTE_D);
  printf("Check D on heap_array: %d\n", ret);
  
  ret = pagetablecheck(heap_array, 16384, PTE_U);
  printf("Check U on heap_array: %d\n", ret);
  
  free(heap_array);
  heap_array = 0;
  
  printf("\nAfter free:\n\n");
  pagetableprint();
  
  printf("global_var = %d\n", global_var);
  printf("stack_var = %d\n", stack_var);
  printf("stack_array[0] = %d\n", stack_array[0]);
  
  exit(0);
}