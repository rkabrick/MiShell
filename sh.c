#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <limits.h>
#include <unistd.h>
#include <stdlib.h>
#include <pwd.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include "sh.h"

int sh(int argc, char **argv, char **envp){
  char *prompt = calloc(PROMPTMAX, sizeof(char));
  char *commandline = calloc(MAX_CANON, sizeof(char));
  char *command, *arg, *commandpath, *p, *pwd, *owd;
  char **args = calloc(MAXARGS, sizeof(char*));
  int uid, i, status, argsct, go = 1;
  struct passwd *password_entry;
  char *homedir;
  struct pathelement *pathlist = NULL;
  void ls(struct dirent *dirp, DIR *dp);


  uid = getuid();
  password_entry = getpwuid(uid);               /* get passwd info */
  homedir = password_entry->pw_dir;		/* Home directory to start
        					  out with*/

  if ( (pwd = getcwd(NULL, PATH_MAX+1)) == NULL )
  {
    perror("getcwd");
    exit(2);
  }
  owd = calloc(strlen(pwd) + 1, sizeof(char));
  memcpy(owd, pwd, strlen(pwd));
  prompt[0] = ' '; prompt[1] = '\0';

  /* Put PATH into a linked list */
  pathlist = get_path();

  do
  {
    /* print your prompt */
    printf("[%s] >> ", pwd);
    /* get command line and process */
    if (fgets(commandline, MAX_CANON, stdin) != NULL){
      int len = strlen(commandline);
      commandline[len-1] = '\0'; //replaces \n with terminator
      if (strcmp(commandline, "q") == 0 || strcmp(commandline, "exit") == 0) {
		return;
      }
      char *ptr = strtok(commandline, " ");
      //args[0] = calloc(strlen(commandline), sizeof(char));
      //strcpy(args[0], commandline);
      int i = 0;
      while (ptr != NULL) {
		args[i] = calloc(strlen(ptr), sizeof(char));
		strcpy(args[i], ptr);
        ptr = strtok(NULL, " ");
		i++;
      }
      args[i] = NULL;
      if(strcmp(commandline, "pwd") == 0){ //if pwd entered
        printf("%s", pwd = getcwd(NULL, PATH_MAX+1));
      }

      //Where
      else if (strcmp(commandline, "where") == 0) {
	if (args[1] !=NULL) {
          ptr = where(args[1], pathlist);
          if (ptr != NULL){
            printf("%s\n",ptr);
            free(ptr);
          }
	}
        else{
          printf("Error: Too few arguments");
        }
      }

      //Which
      else if (strcmp(commandline, "which") == 0) {
		if (args[1] != NULL) {
		  ptr = which(args[1], pathlist);
		  if (ptr != NULL) {
	      	printf("%s\n", ptr);
	      	free(ptr);
	      }
		}
        else{
          printf("Error: Too few arguments");
          continue;
        }
      }

      else if(strcmp(commandline, "pid") == 0){
        int pid = getpid();
        printf("PID = %d\n", pid);
      }

      else if(strcmp(commandline,"ls") == 0){
        DIR *dp;
        struct dirent *dirp;
        int i = 1;
        while(args[i] != NULL){
          while((dp = opendir(args[i])) != NULL){
            //dirp = readdir(dp);
            printf("%s: \n", args[i]);
            ls(dirp, dp);
            printf("\n\n");
            closedir(dp);
            break;
          }
          i++;
        }
      }
      else if(strcmp(commandline, "printenv") == 0){
        if (args[2] != NULL){
        	printf("Error: Too many arguments.\n"); //change to be same as tcsh
        }
		else {
			printEnv(args[1], envp); //if args[1] == NULL prints all
		}
      }
      else if(strcmp(commandline, "cd") == 0){
      }
    }
  }while(1);
}




char* getCommand(char **argv) {
  if (argv == NULL){
    return NULL;}

  return argv[0];
}

void *cd(char *newDir, char *pwd){
}

void ls(struct dirent *dirp, DIR *dp){
  while((dirp = readdir(dp)) != NULL){
    if(strcmp(dirp -> d_name, ".") == 0 || strcmp(dirp -> d_name, "..") == 0){
      continue;
    }
    printf("\n \t %s", dirp -> d_name);
    ls(dirp, dp);
  }
}

void printEnv(char *env, char **envlist){
	if (env == NULL) {
		int i = 0;
		while(envlist[i] != NULL) {
			printf("%s\n", envlist[i]);
			i++;
		}
	}
	else {
		char *value;
		if ((value = getenv(env)) != NULL) {
			printf("%s : %s\n", env, value);
		}
		else {
			printf("Error: Path variable not found.\n");
		}
	}
}

char *which(char *command, struct pathelement *pathlist )
{
  DIR *dp;
  struct dirent *dirp;

  while(pathlist -> next != NULL){
    dp = opendir(pathlist -> element);
    while((dirp = readdir(dp)) != NULL){
      if (strcmp(dirp -> d_name, command) == 0){
	char *whichPath = calloc(PATH_MAX, sizeof(char));
	strcpy(whichPath, pathlist -> element);
	strcat(whichPath, "/");
	strcat(whichPath, dirp -> d_name);
	return whichPath;
      }
    }
    pathlist = pathlist -> next;
  }
} /* which */

char *where(char *command, struct pathelement *pathlist ){
  DIR *dp;
  struct dirent *dirp;
  char *wherePath = calloc(PATH_MAX, sizeof(char));
  while(pathlist -> next != NULL){
    dp = opendir(pathlist -> element);
    while((dirp = readdir(dp)) != NULL){
      if (strcmp(dirp -> d_name, command) == 0){
	strcat(wherePath, pathlist -> element);
	strcat(wherePath, "/");
	strcat(wherePath, dirp -> d_name);
        wherePath = strcat(wherePath, "\n");
      }
    }

    pathlist = pathlist -> next;
  }
  return wherePath;
}

