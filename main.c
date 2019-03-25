#include "sh.h"
#include <signal.h>
#include <stdio.h>

void sig_handler(int signal);

int execTime = 0;

int main( int argc, char **argv, char **envp){
  /* put signal set up stuff here */
  signal(SIGINT, sig_handler); //Program interrupt from keyboard (Ctrl-c)
  signal(SIGABRT, sig_handler); //Process detects error
  signal(SIGSEGV, sig_handler); //Process trying to access NULL memory
  signal(SIGTERM, sig_handler); //Termination of process
  signal(SIGKILL, sig_handler); //Causes immediate program termination
  signal(SIGCHLD, sig_handler); //Used so parent process knows to collect child's exit code
  signal(SIGALRM, sig_handler); //Used for Timeout Extra Credit
  sh(argc, argv, envp);
}

void sig_handler(int signal){
  /* define your signal handler */
  if (signal == (SIGINT || SIGTERM || SIGABRT)){
    printf("\n");
  }

  else if (signal == SIGCHLD){
    printf("Child has been brutally raped and murdered");
  }

  else if (signal == SIGKILL){

  }

  else if(signal == SIGALRM){
    printf("Command elapsed: %d seconds\n",execTime);
    printf("Process timing out\n");
    //kill (Process)
    printf(">>");
  }

  else if(signal == SIGABRT){
    printf("Received SIGABRT BIATCH\n");
    sleep(1);
    return;
  }
}
