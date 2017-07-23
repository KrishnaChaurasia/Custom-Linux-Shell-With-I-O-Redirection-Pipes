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

//Function to change the directory to the given location
void change_dir(char *path){
  char tmp[PATH_MAX];
  int j;
  if(path==NULL || strlen(path) == 0)
    //Change to home directory
    chdir((char*)getenv("HOME"));
  else if(strcmp(path,"..") == 0){
    for(j=strlen((char*)getcwd(tmp,PATH_MAX))-2;j>0;j--){
      if(path[j] == '/')
	break;
    }
    path[j]='\0';
    chdir(path);
  }
  else			
    chdir(path);			
}

int main(int argc, char *argv[]){
  while(1){
    change_dir(argv[1]);
    system("pwd");
    break;
  }
  return 0;
}
