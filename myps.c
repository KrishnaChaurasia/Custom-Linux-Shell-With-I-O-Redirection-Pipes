//Program to implement ps -ax command

#include<stdio.h>
#include<sys/types.h>
#include<string.h>
#include<dirent.h>
#include<sys/times.h>
#include<unistd.h>
#include<time.h>
#include<linux/limits.h>

FILE *input;

void readstr(char *x) { 
  int pid;
  fscanf(input, "%d ", &pid);
  fscanf(input, "%s ", x);
}

void readchar(char *c){
  fscanf(input, "%c", c);
}

void readnum(int *d){
  fscanf(input, "%d", d);
}

void printtime(int x) {  
  printf("%.2f", (((double)x) / (sysconf(_SC_CLK_TCK))));
}

int main(int argc, char *argv[]){
  FILE *fp;
  char c, fname[1000], pname[PATH_MAX], state, fullpath[PATH_MAX], st[10];                              
  int pid, ppid, pgrp, session, tty_nr, tpgid, flags, minflt, cminflt,
    majflt, cmajflt, utime;
  
  DIR *d;
  struct dirent *dir;
  
  unsigned int mask = 65280;
  
  input = NULL;
  if(argc < 1){
    printf("Invalid argument list\n");
    return 0;
  }
  else {
    //Go to /proc directory and search for directories having names as numbers; thus represent the processes runnning
    d = opendir("/proc");
    chdir("/proc");
    if(d){
      while ((dir = readdir(d)) != NULL){
	sprintf(fname, "%s", dir->d_name);
	if(isdigit(fname[0])){
	  sscanf(fname, "%d", &pid);
	  if(chdir(fname) == 0){
	    input = fopen("stat", "r");
	  }
	  if(!input){
	    perror("open");
	    return 1;
	  }
	  readstr(pname);
	  readchar(&state);
	  readnum(&ppid);
	  readnum(&pgrp);
	  readnum(&session);
	  readnum(&tty_nr);
	  readnum(&tpgid);
	  readnum(&flags);
	  readnum(&minflt);
	  readnum(&cminflt);
	  readnum(&majflt);
	  readnum(&cmajflt);
	  readnum(&utime);
	  printf("%d\t", pid);
	  if((tty_nr & mask) >> 8 == 136){
	    printf("pts/%d\t",((tty_nr >> 20) << 8) | (tty_nr & 255));
	  }
	  else if((tty_nr & mask) >> 8 == 4){
	    printf("tty%d\t",((tty_nr >> 20) << 8) | (tty_nr & 255));
	  }
	  else{ 
	    printf("?\t");
	  }
	  printf("%c\t", state);
	  printtime(utime);
	  printf("\t%s\n", pname);
	  chdir("..");
	}
      }
      closedir(d);
    }
  }
  return 0;
}
