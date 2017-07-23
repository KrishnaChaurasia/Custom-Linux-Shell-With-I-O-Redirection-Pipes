//Program to implement the mkdir command
#include<stdio.h>
#include<unistd.h>
#include<string.h>
#include<sys/stat.h>
#include<stdlib.h>
#include<fcntl.h>
#include<sys/types.h>
#include<sys/ioctl.h>
#include<dirent.h>

//Function to create the directory in the given location; creates parent directory if not already present
void create_dir(const char *path) {
  char tmp[PATH_MAX];
  char *p = NULL;
  size_t len;
  
  snprintf(tmp, sizeof(tmp),"%s", path);
  len = strlen(tmp);
  if(tmp[len - 1] == '/')
    tmp[len - 1] = 0;
  for(p = tmp + 1; *p; p++)
    if(*p == '/') {
      *p = 0;
      mkdir(tmp, S_IRWXU);
      *p = '/';
    }
  mkdir(tmp, S_IRWXU);
}

int main(int argc, char *argv[]){
  create_dir(argv[1]);
  return 0;
}
