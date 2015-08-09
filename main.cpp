#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <ctype.h>
#include <signal.h>
#include <map>
#include "options.c"

DocoptArgs args;

std::map< int, const char ** > ProcMap;

void startProc(const char *line0[]){
  int pid=fork();
  switch (pid) {
  case -1:
    perror("fork");
    break;
  case 0:
    printf("Running [%s]'%s'", line0[0], line0[1]);
    for (const char **sptr = line0+2; *sptr; sptr++)
      printf(" '%s'", *sptr);
    printf("\n");
    execvp(line0[1],(char**)line0+1);
    perror("execvp");
    exit(1);
  default:
    // success!  record for next time
    ProcMap[pid] = line0;
  }
}

int parseProcfile(const char*procfilename=0) {
  FILE *f = fopen(procfilename,"r");
  if (!f) {
    perror("fopen");
    exit(1);
  }
  char buf[10241];
  memset(buf, 0, 10241);
  while (char *line = fgets(buf, 10240, f)){
    int colon_offset=0, cmd_offset=0;
    for (char *s=line;*s;s++) {
      switch(*s){

      // trim the line if you see one of these
      case '#': case'\r': case'\n':
	*s = '\0';
	goto big_break;

      case ':': // record where the colon is, and where the command starts
	if (!colon_offset) {
	  colon_offset = s - line;
	  char *cursor = s;
	  while (*(++cursor) && isspace(*cursor));
	  cmd_offset   = cursor - line;
	}
      default: ; // do nothing
      }
    }
  big_break:
    if (colon_offset) {
      line = strdup(line); // malloc the string, but only the length we need
      line[colon_offset] = '\0'; // terminate that process group
      const char * linearr[]= { line, "sh", "-c", &line[cmd_offset], 0 };
      const int linesize = sizeof(linearr);
      const char **linebuf  = (const char**)malloc(linesize);
      memcpy(linebuf, linearr, linesize);
      startProc(linebuf);
    } else {
      fprintf(stderr, "no colon [%s]\n", line);
    }
  }
  fclose(f);
  return 0;
}

void waitForRestarts() {
  while (1) {
    int ret, pid = wait(&ret);
    printf("ret=%d / pid=%d\n", ret, pid);
    if (!ret) { startProc(ProcMap[pid]); }
    else      { sleep(1); }
    usleep(10);
  }
}

int main(int argc, char *argv[]){
  args = docopt(argc, argv, 1, "1.0.1");
  if (args.unbuffered)
    setbuf(stdout,0);
  if (args.start) {
    if (!parseProcfile("Procfile"))
      waitForRestarts();
  } else {
    fprintf(stderr, help_message);
  }
}
