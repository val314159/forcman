#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>

#include <vector>
#include <iostream>
#include <map>

#include "options.c"

DocoptArgs args;

std::map< int, const char ** > mm;

void startProc(const char *line0[]){
  int pid=fork();
  switch (pid) {
  case -1:
    perror("fork");
    break;
  case 0:
    printf("[%s]\n", line0[1]);
    execvp(line0[1],(char**)line0+1);
    perror("execvp");
    exit(1);
  default:
    printf("success\n");
    mm[pid] = line0;
  }
}

int parseProcfile(const char*procfilename=0) {
  FILE *f = fopen(procfilename,"r");
  if (!f) {
    exit(1);
  }
  char buf[10241];
  char *c = 0;
  memset(buf, 0, 10241);
  while (c = fgets(buf, 10240, f)) {
    c = strdup(c);

    char *colon=0;
    for (char *s=c;*s;s++) {
      switch(*s){
      case '\r': case '\n': *s='\0'; break;
      case ':':
	if (!colon) {
	  *(colon = s) = '\0';
	  while (*(++colon) && isspace(*colon));
	}
      }
    }
    if (colon) {
      const char  *linearr[]= { c, "sh", "-c", colon, 0 };
      const int sz = sizeof(linearr);
      const char **linebuf  = (const char**)malloc(sz);
      memcpy(linebuf, linearr, sz);
      startProc(linebuf);
    } else {
      fprintf(stderr, "no colon [%s]\n", c);
    }
  }
  fclose(f);
  return 0;
}

void waitForRestarts() {
  while (1) {
    int ret, pid = wait(&ret);
    printf("ret=%d / pid=%d\n", ret, pid);
    if (!ret) { startProc(mm[pid]); }
    else      { sleep(1); }
  }
}

int main(int argc, char *argv[]){
  args = docopt(argc, argv, 1, "1.0.0");
  setbuf(stdout,0);
  printf("%d\n", args.start);
  if (args.start) {
    if (!parseProcfile("Procfile"))
      waitForRestarts();
  }
}
