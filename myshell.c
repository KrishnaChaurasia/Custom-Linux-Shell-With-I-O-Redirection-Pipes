/*
Program to implement myshell
*/
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<termios.h>
#include<string.h>
#include<dirent.h>
#include<fcntl.h>
#include<signal.h>
#include<sys/wait.h>

#define PROMPT "krishna@myshell:$ "

char mycmd[1024];
int cmdEntered=0,lencmd=0,bckgrndProcessCount = 0;

/**
* This function breaks a string based on the space
* @ param cmd pointer to the input string
* Returns the count of the number of strings separated by spaces
*/
int breakString(char *cmd, char *cmdcopy){
  int count;
  char *inParts;
  
  inParts = strtok(cmdcopy," ");
  while(inParts){
    count++;
    inParts = strtok(NULL," ");
  }
  return count;
}

/**
 * This is background process handler
 * Kills processes for which the parent didn't wait
 */
//This function is used to handle process that run in background for which the parent could not wait on
void bckgrndHandler(){
  int status;
  pid_t bckgrndproc;
  while ((bckgrndproc = waitpid(-1, &status, WNOHANG)) > 0);
  return;
}

/**
* This function executes the command given from myshell
* @ param mycmd Pointer to the mycmd
* Returns 1 if the command executes successfully
*/

//This function executes the command entered by the user
//Created pipes if based on the number of pipes detected in the command
//Sets input and output based on the redirection
int executeCmd(char *mycmd){
  char *temp,*token,*tokencopy,*inParts,*svptr1,*svptr2,*path,*temppath;
  char ***cmd=NULL;
  unsigned int len;
  int *pipes,*argCountPerCmd;
  int pid,cmdLen,cmdCount,argPerCmd,permissions,flags,bckgrdFound,pipeFound,status,inputfd,outputfd,i,j;
  
  path = (char *)calloc(PATH_MAX, sizeof(char));
  getcwd(path,500);

  permissions = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH  | S_IWOTH;
  flags = O_CREAT | O_RDWR | O_APPEND;
  
  cmdLen = strlen(mycmd);
  bckgrdFound = 0;
  //Check for backgroound processes
  if(mycmd[cmdLen-1] == '&'){
    bckgrdFound = 1;
    mycmd[cmdLen-1] = '\0';
    cmdLen = strlen(mycmd);
  }
  //Separate the commands individually from pipes
  temp = (char*)calloc(cmdLen,sizeof(char));
  strcpy(temp,mycmd);
  cmdCount = 0;
  token = strtok(temp,"|");
  while(token){
    cmdCount++;
    token = strtok(NULL,"|");
  }
  
  //Creation of pipes based on the number of pipes in the command
  pipes = (int*)calloc((cmdCount-1)*2,sizeof(int));
  if(cmdCount > 1)
    pipeFound = 1;
  else
    pipeFound = 0;
  argCountPerCmd = (int*)calloc(cmdCount,sizeof(int));
  
  for(j = 0; j<(cmdCount-1);j++){
    if(pipe(pipes + j*2) < 0) {
      printf("error in pipe creating!\n");
      exit(1);
    }
  }
  cmd  = (char***)calloc(cmdCount,sizeof(char**));
  token = strtok_r(mycmd,"|",&svptr2);
  i = 0;
  //Calculation of number of paramters per command
  while(token){
    if(token[strlen(token)-1] == '\n')
      token[strlen(token)-1] = '\0';
    tokencopy = (char*)calloc(strlen(token),sizeof(char));
    strcpy(tokencopy, token);
    argPerCmd = breakString(token, tokencopy);
    argCountPerCmd[i] = argPerCmd;
    cmd[i] = (char**)calloc(argPerCmd,sizeof(char*));
    j=0;
    inParts = strtok_r(token," ",&svptr1);
    while(inParts && j<argPerCmd){
      cmd[i][j] = (char*)calloc(strlen(inParts),sizeof(char));
      strcpy(cmd[i][j],inParts);
      token += strlen(inParts);
      inParts = strtok_r(NULL," ", &svptr1);
      j++;
    }
    i++;
    token=strtok_r(NULL,"|",&svptr2);
  }
  
  //Execute commands individually calling fork for each command
  for(i=0;i<cmdCount;i++){
    pid = fork();
    switch(pid){
    case -1 :
      printf("Error creating child process\n");
      exit(1);
    case 0 : //Execution of child process
      //Create a new session if there is any background process
      if(bckgrdFound){
	setsid();
      }
      if(i == 0){
	//No modification in stdin for first command; its write end of the pipe becomes the output for next command
	if(pipeFound){
	  close(pipes[0]);
	  dup2(pipes[2*i+1], 1);
	  close(pipes[2*i+1]);
	}
	//Set stdin of first command if redirection operation is present
	if(argCountPerCmd[i] >= 2 && cmd[i][argCountPerCmd[i]-2][0] == '<'){
	  inputfd = open(cmd[i][argCountPerCmd[i]-1], O_RDONLY);
	  dup2(inputfd,0);
	  close(inputfd);
	  cmd[i][argCountPerCmd[i]-1] = NULL;
	  cmd[i][argCountPerCmd[i]-2] = NULL;
	}
       	//Set stdout if redirection operation is present
	if(argCountPerCmd[i] >= 2 && cmd[i][argCountPerCmd[i]-2] != NULL && cmd[i][argCountPerCmd[i]-2][0]  == '>'){
	  outputfd = open(cmd[i][argCountPerCmd[i]-1], O_CREAT | O_WRONLY, permissions);
	  dup2(outputfd,1);
	  close(outputfd);
	  cmd[i][argCountPerCmd[i]-1] = NULL;
	  cmd[i][argCountPerCmd[i]-2] = NULL;
	}
	if(argCountPerCmd[i] >= 4 && cmd[i][argCountPerCmd[i]-4][0] == '<'){
	  inputfd = open(cmd[i][argCountPerCmd[i]-3],O_CREAT | O_RDONLY | O_APPEND,permissions);
	  dup2(inputfd,0);
	  close(inputfd);
	  cmd[i][argCountPerCmd[i]-1] = NULL;
	  cmd[i][argCountPerCmd[i]-2] = NULL;
	  cmd[i][argCountPerCmd[i]-3] = NULL;
	  cmd[i][argCountPerCmd[i]-4] = NULL;
	}
      }
      //No modification in stdout for the last command; it reads from the read end of the pipe of its previous command
      else if(i == cmdCount-1){
	//No change in stdout for last command
	if(pipeFound){
	  close(pipes[2*(i-1)+1]);
	  dup2(pipes[2*(i-1)],0);
	  close(pipes[2*(i-1)]);
	}
	//Change stdout if output to be written on file
	if(argCountPerCmd[i] >=2 && strcmp(cmd[i][argCountPerCmd[i]-2],">") == 0){
	  outputfd = open(cmd[i][argCountPerCmd[i]-1],O_CREAT | O_WRONLY | O_APPEND,permissions);
	  dup2(outputfd,1);
	  close(outputfd);
	  cmd[i][argCountPerCmd[i]-1] = NULL;
	  cmd[i][argCountPerCmd[i]-2] = NULL;
	}
      }
      //All the other intermediate process read from the read end of its previous command's pipe and write to the  write end of its next command pipe
      else {
	if(pipeFound){
	  dup2(pipes[2*(i-1)],0);
	  dup2(pipes[2*i+1],1);
	}
      }
      //Execution of my* command
      if(strncmp(cmd[i][0],"my",2) == 0){
	temppath = NULL;
	temppath = path;
	strcat(temppath,"/./");
	strcat(temppath,cmd[i][0]);				
	strcat(temppath,".out");				
	execvp(temppath,cmd[i]);
      }
      //Execution of standard commands 
      else{				
	execvp(cmd[i][0],cmd[i]);
      }
      exit(0);
      
    default : //Parent does nothing
      ;
      break;
    }
  }
  //Close the opened pipes
  for(i=0;pipeFound && i<2*(cmdCount-1);i++)	
    close(pipes[i]);
  if(bckgrdFound){
    printf("\n[%d] %d\n",++bckgrndProcessCount,pid);
    printf(PROMPT);
  }
  //Wait for processes to terminate
  if(!bckgrdFound)
    while(waitpid(0,&status,WUNTRACED)>0);
  
  return 0;
}

/**
 * This function detects the arrow keypress for showing the command history
 * Returns a char value based on the key pressed; 1 for uparrow key, 2 for downarrowkey, 3 for left&right arrow key and 0 otherwise
 */
//Detect for the key press for history and return a char based on the value of key press
char keyPress(){
  static struct termios st, old;
  char key;
  char c;
  
  //Set the terminal into non-canonical mode for getting the arrow key detected without having to press newline character
  tcgetattr(STDIN_FILENO, &st);
  old = st;
  st.c_lflag &= ~ICANON;
  tcsetattr(STDIN_FILENO, TCSAFLUSH, &st);
  
  //Condition for arrow key
  if((c=getchar()) == '\033' && (c=getchar())){
    c = getchar();
    lencmd = 0;
    cmdEntered = 0;
    st.c_lflag &= ~ECHO;
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &st);
    printf("\33[2K\r");
    
    switch(c){
    case 'A':
      key = 'u'; 
      break;
    case 'B':
      key = 'd';
      break;
    default:
      key = 'o';
      break;
    }
  }
  //For backspace
  else if(c == '\177'){
    printf("\33[2K\r");
    if(--lencmd < 0)
      lencmd = 0;
    mycmd[lencmd] = '\0';
    printf(PROMPT);
    printf("%s",mycmd);	
    while(c=getchar()){
      if(c == '\177'){
	printf("\33[2K\r");
	if(--lencmd < 0)
	  lencmd = 0;
	mycmd[lencmd] = '\0';
	printf(PROMPT);
	printf("%s",mycmd);
      }
      else{
	mycmd[lencmd++] = c;
	if(c == '\n'){
	  mycmd[lencmd-1] = '\0';
	  cmdEntered = 1;
	}
	break;
      }
    }
  }
  else {
    mycmd[lencmd++] = c;
    if(c == '\n'){
      if(lencmd <= 1){
	printf(PROMPT);
	mycmd[--lencmd] = '\0';
      }
      else{
	mycmd[lencmd-1] = '\0';
	cmdEntered = 1;
      }
    }
    key = '0';
  }
  st = old;
  tcsetattr(0, TCSAFLUSH, &st);
  return key;
}

/**
* This is the main function
* Returns 0 on sucessful termination of the program
*/

int main(){  
  char c;
  int arrowPressCount=0,count=0,read,len; 
  char **cmd,keyId;
  struct sigaction sa;
  
  cmd = (char**)calloc(1, sizeof(char*));
  cmd[0] = NULL;
  
  //Handling the backgorund processes
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = 0;
  sa.sa_handler = bckgrndHandler;
  if (sigaction(SIGCHLD, &sa, NULL) == -1)
    printf("signal handler error");
  
  printf(PROMPT);
  while(1){
    keyId = keyPress();
    //Display command history based on the number of arrow key presses
    if(keyId == 'u'){
      printf(PROMPT);
      arrowPressCount++;
      if(arrowPressCount >= count)
	arrowPressCount = count;
      if(count > 0){
	strcpy(mycmd, cmd[count - arrowPressCount]);
	lencmd = strlen(mycmd);
	printf("%s",mycmd);
      }
    }
    else if(keyId == 'd'){
      printf(PROMPT);
      arrowPressCount--;
      if(arrowPressCount < 0)
	arrowPressCount = 0;
      if(count > 0){
	strcpy(mycmd, cmd[count-1-arrowPressCount]);
	lencmd = strlen(mycmd);
	printf("%s",mycmd);
      }
    }
    else if(keyId=='o')
      printf(PROMPT);
    //Detect if non-arrow key pressed
    else{
      //if newline character detected then execute command
      if(cmdEntered == 1){
	if(count <=1 || strcmp(cmd[count-1],mycmd) != 0){
	  cmd = (char**)realloc(cmd,(count+1)*sizeof(char*));
	  cmd[count] = (char*)calloc(strlen(mycmd),sizeof(char));
	  strcpy(cmd[count], mycmd);
	  arrowPressCount = 0;
	  count++;
	}
	cmdEntered=lencmd=0;
	if(strcmp(mycmd,"myexit") == 0){
	  printf("\n");
	  exit(0);
	}
	printf(PROMPT);
	executeCmd(mycmd);
      }
    }
  } 
  return 0;
}
