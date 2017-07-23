//Program to implement move command

#include<stdio.h>
#include<fcntl.h> 
#include<stdlib.h>

int main(int argc,char *argv[]){
  int src, dest, count;
  char buf[512];
  
  if(argc != 3){
    printf("Invalid number of arguments\n");
    exit(0);
  }
  else{
    src = open(argv[1], O_RDONLY); 
    if(src == -1){
      printf("Source file does not exist\n"); 
      exit(0);
    }
    dest = open(argv[2],O_WRONLY); 
    //Create if file not present already
    if(dest == -1)
      dest = creat(argv[2],0766);
    
    //Read from the source file and write to the destination file
    while((count = read(src, buf, 512)) > 0){
      write(dest, buf, count);
    }
    close(src);
    close(dest);
  }
  
  //Delete the source file
  count = unlink(argv[1]); 
  return 0;
}
