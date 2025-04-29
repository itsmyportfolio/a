/*  parent children configuration. Each child would execute one command and terminate. The gap between termination of one child to next child process be at least 5 minutes .The commands are (i) ls -al, (ii) man dig, (iii) man host, (iv) ip -s link, (v) dig google.com, (vi) man scp, (vii) man socket, (viii) man
connect, (ix) man listen, (x) man 3 bind 


compile and run

gcc file.c -o file
./file

*/


#include<stdio.h>   // standard input output Header
#include<stdlib.h>  // standard library Header eg malloc() , free() , exit() 
#include<sys/types.h> // system  types Header  : defines data types used in system calls such as pid_t for process ids
#include<sys/wait.h>  // system wait  Header : for wait() , waitpid()
#include<unistd.h>  // unix standard header : provide access to the POSIX OS API, including functions like fork() , sleep()
#include<time.h> // header file for manipulation for date and time

void execute_commands( const char* command ,   char * const argv[] ){
  pid_t pid = fork();
  if(pid == 0) {
      // child process 
      execvp( command , argv);
      printf("\n  execvp failed");
      return;
  }else if( pid > 0 ){
      // parent process
      wait(NULL); // wait for the child process to terminate
      printf("\n child process completed. waiting for  5 minutes ...\n");
      sleep(5);
    }else{
      printf("\n fork failed");
      return;
    }
}

int main() {
    char* commands[][4] = {
    { "man","scp"  , NULL },
    { "man" , "3" , "bind" , NULL },
    { "man" , "socket" , NULL} ,
        { "man", "connect" , NULL },
        { "man" , "listen" , NULL },
    { "ip", "-s" ,"link", NULL },
    { "dig", "google.com" , NULL },
    { "man" , "dig", NULL },
    { "ls","-l"  , NULL },
        
        { "man", "host" , NULL }
      };
      int n = sizeof(commands)/sizeof(commands[0]);
      int i ;
      for( i = 0 ; i < n ; i++ ){
        //printf("\n running command :->  %s \n", *commands[i]);
        printf("\n Running command %d", i);
        execute_commands(commands[i][0], commands[i]);
      }
      printf("\n All commands executed successfully");
}
