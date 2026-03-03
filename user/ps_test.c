#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

void
write_err(char *msg)
{
  write(2, msg, strlen(msg));
}

void
write_msg(char *msg)
{
  write(1, msg, strlen(msg));
}

void
print_result(char *test_name, int passed)
{
  write_msg("TEST: ");
  write_msg(test_name);
  if(passed) {
    write_msg("pass\n");
  } else {
    write_msg("fail\n");
  }
}

int
test_null_pointer()
{
  int count = ps_listinfo(0, 0);
  
  if(count < 1) {
    write_err("Error: expected at least initial process");
    return 0;
  }
  return 1;
}

int
test_normal_operation()
{
  int count = ps_listinfo(0, 0);
  if(count < 0) return 0;
  
  struct procinfo *plist = (struct procinfo *)malloc(count * sizeof(struct procinfo));
  if(plist == 0) {
    write_err("Error: malloc fail\n");
    return 0;
  }
  
  int ret = ps_listinfo(plist, count);
  
  int valid = 1;
  if(ret != count) {
    write_err("Error: returned count mismatch\n");
    valid = 0;
  }
  
  free(plist);
  return valid;
}

int
test_not_enough_buffer_space()
{
  int count = ps_listinfo(0, 0);
  if(count < 0) return 0;
  
  int lim = (count > 1) ? count - 1 : 1;
  struct procinfo *plist = (struct procinfo *)malloc(lim * sizeof(struct procinfo));
  if(plist == 0) return 0;
  
  int ret = ps_listinfo(plist, lim);
  free(plist);
  
  if(ret != -1) {
    write_err("Error: test with not enough buffer space failed\n");
    char c = '0' + ret;
    write(2, &c, 1);
    write_err("\n");
    return 0;
  }
  return 1;
}

int
test_invalid_address()
{
  int ret = ps_listinfo((struct procinfo *)1, 10);
  
  if(ret >= 0) {
    write_err("Error: expected error for invalid address\n");
    return 0;
  }
  return 1;
}

int
test_zero_limit()
{
  int count = ps_listinfo(0, 0);
  if(count < 0) return 0;
  
  if(count == 0) return 1;
  
  struct procinfo *plist = (struct procinfo *)malloc(sizeof(struct procinfo));
  if(plist == 0) return 0;
  
  int ret = ps_listinfo(plist, 0);
  free(plist);
  
  if(ret != -1) {
    write_err("Error: expected -1 for zero processes limit\n");
    return 0;
  }
  return 1;
}

int
test_dynamic_buffer()
{
  int lim = 2;
  int attempts = 0;
  int max_attempts = 5;

  while(attempts < max_attempts) {
    struct procinfo *plist = (struct procinfo *)malloc(lim * sizeof(struct procinfo));
    if(plist == 0) {
      write_err("Error: malloc fail in dynamic test\n");
      return 0;
    }
    
    int ret = ps_listinfo(plist, lim);
    
    if(ret == -1) {
      free(plist);
      lim = lim * 2;
      attempts++;
      continue;
    }
    
    if(ret < 0) {
      free(plist);
      write_err("Error: unexpected error in dynamic test\n");
      return 0;
    }
    
    int valid = 1;
    for(int i = 0; i < ret; i++) {
      if(plist[i].pid <= 0) {
        valid = 0;
        break;
      }
    }
    
    free(plist);
    return valid;
  }
  
  write_err("Error: max attempts exceeded in dynamic test\n");
  return 0;
}

int
main(int argc, char *argv[])
{
  int passed = 0;
  int total = 6;
  
  if(test_null_pointer()) passed++;
  print_result("NULL pointer: ", (passed >= 1));
  
  if(test_normal_operation()) passed++;
  print_result("Normal operation: ", (passed >= 2));
  
  if(test_not_enough_buffer_space()) passed++;
  print_result("Too small buffer: ", (passed >= 3));
  
  if(test_invalid_address()) passed++;
  print_result("Invalid address: ", (passed >= 4));
  
  if(test_zero_limit()) passed++;
  print_result("Zero limit: ", (passed >= 5));
  
  if(test_dynamic_buffer()) passed++;
  print_result("Dynamic buffer: ", (passed >= 6));
  
  if(passed == total) {
    write_msg("All tests passed\n");
    exit(0);
  } else {
    write_err("Some tests failed\n");
    exit(1);
  }
}