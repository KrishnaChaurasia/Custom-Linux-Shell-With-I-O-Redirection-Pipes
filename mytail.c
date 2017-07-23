//Program to implement the tail command

#include <stdio.h>
#include<stdlib.h>
int main(int argc, char *argv[]){
  FILE *fileptr;
  int total_lines = 0, count_lines = 0, n;
  char c;
  
  if(argc != 3){
    printf("Invalid argument list\n");
    return -1;
  }
  
  fileptr = fopen(argv[2], "r");
  sscanf(argv[1], "%c %d", &c, &n);
  
  //extract character from file and store in c
  c = getc(fileptr);
  while (c != EOF){
    //Count whenever new line is encountered
    if (c == '\n'){
      total_lines = total_lines + 1;
    }
    //take next character from file
    c = getc(fileptr);
  }
  
  //set file pointer to the start location
  fseek(fileptr, 0, SEEK_SET);
  
  c = getc(fileptr);
  while (c != EOF){
    //Count whenever new line is encountered
    if(count_lines >= total_lines - n){
      printf("%c", c);
    }
    if(c == '\n'){
      count_lines += 1;
    }
    
    //read next character from file.
    c = getc(fileptr);
  }
  fclose(fileptr); //close file
  return 0;
}
