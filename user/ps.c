#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define UNUSED 0
#define USED 1
#define SLEEPING 2
#define RUNNABLE 3
#define RUNNING 4
#define ZOMBIE 5

char*
state_to_string(int state)
{
  switch(state) {
    case UNUSED: return "UNUSED";
    case USED: return "USED";
    case SLEEPING: return "SLEEPING";
    case RUNNABLE: return "RUNNABLE";
    case RUNNING: return "RUNNING";
    case ZOMBIE: return "ZOMBIE";
    default: return "UNKNOWN";
  }
}

char*
find_name(struct procinfo *plist, int count, int pid)
{
  for(int i = 0; i < count; i++) {
    if(plist[i].pid == pid) return plist[i].name;
  }
  return "-";
}


int
main(int argc, char *argv[])
{

  int count = ps_listinfo(0, 0);
  if(count < 0) {
    fprintf(2, "Error: failed to get process count\n");
    exit(1);
  }

  struct procinfo *plist = (struct procinfo *)malloc(count * sizeof(struct procinfo));
  if(plist == 0) {
    fprintf(2, "Error: malloc failed\n");
    exit(1);
  }

  int ret = ps_listinfo(plist, count);
  if(ret < 0) {
    fprintf(2, "Error: failed to get process info\n");
    free(plist);
    exit(1);
  }

  printf(" PID        NAME           STATE         PPID        PNAME\n");
  printf("------ -------------- --------------- ---------- --------------\n");


  for(int i = 0; i < ret; i++) {
    char *pname = find_name(plist, ret, plist[i].ppid);
    
    printf("   %d         %s         %s          %d           %s\n",
        plist[i].pid, plist[i].name, state_to_string(plist[i].state), plist[i].ppid, pname);
  }

  free(plist);
  exit(0);
}