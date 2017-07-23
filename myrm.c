//Program to implement the rm command

#include<stdio.h>
#include<dirent.h>
#include<sys/stat.h>
#include<malloc.h>
#include<string.h>
#include<sys/types.h>
#include<errno.h>
#include<stdlib.h>

//Fuction to remove the directory
int removedir(char *path){
  DIR *pdir = NULL;
  struct dirent *pent = NULL;
  struct stat eStat;
  
  pdir = opendir(path);
  char x[500];
  
  while ((pent=readdir(pdir)) != NULL){
    if((strcmp((pent->d_name),".") == 0) || (strcmp((pent->d_name),"..") == 0)){	
      continue;
    }
    else{
      strcpy(x,path);
      path = strcat(path,"/");
      path = strcat(path,pent->d_name);
      if(stat(path, &eStat) == -1){
	perror("stat");
	exit(0);
      }
      else{
	//check if path is a directory
	if(S_ISDIR(eStat.st_mode)){ 
	  removedir(path);
	  strcpy(path,x);
	}
	//delete if it is a file
	else{
	  unlink(path);
	  strcpy(path,x);
	}
      }
    }
  }
  rmdir(path);
  return 0; 
}

int main(int argc, char *argv[]){
  int ret;
  char path[500];
  struct stat eStat;
  if(argc < 2){
    printf("Invalid list of arguments\n");
    return -1;
  }
  getcwd(path, 500);
  strcat(path, "/");
  strcat(path, argv[1]);
  stat(path, &eStat);
  
  //Delete if the argument is a file else called the removedir
  if(S_ISDIR(eStat.st_mode)){
     ret = removedir(argv[1]);
  }else{
    unlink(path);
  }
  return 0;
}
