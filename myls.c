//Program to implement the myls command
 
# include <stdio.h>
# include <sys/stat.h>
# include <sys/types.h>
# include <dirent.h> 
# include <unistd.h> 
# include <fcntl.h> 
# include <sys/ioctl.h> 
# include <stdlib.h>
# include <string.h>
# include <pwd.h>
# include <grp.h>
# define STR_SIZE sizeof("rwxrwxrwx")
# define FP_SPECIAL 1

char str[STR_SIZE];
int address (const struct dirent *dir){
  return 1;  
}  

//Function that returns the char pointer to the permissions of the file
char *file_permissions(mode_t perm, int flags){
  //char str[STR_SIZE];
  snprintf(str, STR_SIZE, "%c%c%c%c%c%c%c%c%c",
	   (perm & S_IRUSR) ? 'r' : '-', (perm & S_IWUSR) ? 'w' : '-',
	   (perm & S_IXUSR) ?
	   (((perm & S_ISUID) && (flags & FP_SPECIAL)) ? 's' : 'x') :
	   (((perm & S_ISUID) && (flags & FP_SPECIAL)) ? 'S' : '-'),
	   (perm & S_IRGRP) ? 'r' : '-', (perm & S_IWGRP) ? 'w' : '-',
	   (perm & S_IXGRP) ?
	   (((perm & S_ISGID) && (flags & FP_SPECIAL)) ? 's' : 'x') :
	   (((perm & S_ISGID) && (flags & FP_SPECIAL)) ? 'S' : '-'),
	   (perm & S_IROTH) ? 'r' : '-', (perm & S_IWOTH) ? 'w' : '-',
	   (perm & S_IXOTH) ?
	   (((perm & S_ISVTX) && (flags & FP_SPECIAL)) ? 't' : 'x') :
	   (((perm & S_ISVTX) && (flags & FP_SPECIAL)) ? 'T' : '-'));
  return str;
}

//Function to print the listing of each file
void print(const struct stat *sb ,char *fName){
  char *t0, *t1 = NULL;
  switch (sb->st_mode & S_IFMT) {
  case S_IFDIR: 
    printf("d");
    break;
  default:
    printf("-");
  }
  struct passwd *pwd = getpwuid(sb->st_uid);
  struct group  *grp = getgrgid(sb->st_gid);
  
  printf("%s ",file_permissions(sb->st_mode, 0));
  
  printf("%ld ", (long) sb->st_nlink);
  printf("%s %s ",pwd->pw_name,grp->gr_name);
  printf("%5lld ", (long long) sb->st_size);
  t0 = (char*)ctime(&sb->st_mtime);
  t1 = (char*)calloc(strlen(t0)-1,sizeof(char));
  strncpy(t1,t0,strlen(t0)-1);
  printf("%s %s",t1,fName);
}

int main(int arg, char **args){
  struct stat sb;
  char pwd[PATH_MAX];
  
  unsigned int count = 0;
  struct dirent **files;
  
  getcwd(pwd,PATH_MAX);
  
  //Scan for directory to get the contents
  int total = scandir(pwd, &files, address, alphasort);
  while(count<total){
    if(files[count]->d_name[0] != '.'){
      stat(files[count]->d_name, &sb); 
      print(&sb,files[count]->d_name);
      printf("\n");
    }
    count++;
  }
  
  return 0;
}
