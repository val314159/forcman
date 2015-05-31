#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>

#include <vector>
#include <iostream>
#include <map>

#include "options.c"

std::map< int, const char ** > mm;

const char *line0[] = {"web0","php","-S","0:8000"};
const char *line1[] = {"web", "sh", "-c", "php -S 0:8001"};
const char *line2[] = {"hi1", "sh", "-c", "sleep 2; echo HI1"};
const char *line3[] = {"hi2", "sh", "-c", "sleep 2; echo HI2"};
const char**lines[] = { line1, line2, line3, 0 };

void start(const char *line0[]){
  int pid=fork();
  switch (pid) {
  case -1:
    perror("fork");
    break;
  case 0:
    execvp(line0[1],(char**)line0+1);
    perror("execvp");
    exit(1);
  default:
    printf("success\n");
    mm[pid] = line0;
  }
}

int main(){
  DocoptArgs args = docopt(argc, argv, /* help */ 1, /* version */ "2.0rc2");

  setbuf(stdout,0);

  start(line0);
  start(line1);
  start(line2);
  start(line3);

  while (1) {
    int ret, pid = wait(&ret);
    printf("ret=%d / pid=%d\n", ret, pid);
    if (!ret) { run(mm[pid]); }
    else      { sleep(1); }
  }
}
