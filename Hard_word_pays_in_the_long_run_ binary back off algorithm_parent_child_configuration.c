
/*
parent children configuration. Each child would act as a competing process, These children 16 in number would check, if a buffer, common to the parent and children is available for writing, if it becomes available to one said child process, it locks it, writes statement “hard work pays in the long run” and identifies itself with its pid by  writing pid alongside the written statement, and then unlocks the buffer. If buffer is not available they would use binary back off algorithm to wait
for their turn for the use of buffer.


gcc file.c -o file -pthread
./file


*/

#include<stdio.h>   // standard input output Header
#include<stdlib.h>  // standard library Header eg malloc() , free() , exit() 
#include<sys/types.h> // system  types Header  : defines data types used in system calls such as pid_t for process ids
#include<sys/wait.h>  // system wait  Header : for wait() , waitpid()
#include<unistd.h>  // unix standard header : provide access to the POSIX OS API, including functions like fork() , sleep()
#include<time.h>  // contains definitions of functions to get and manipulate date and time information.
#include<signal.h>  // define additional macros with names beginning with SIG to specify the values of additional signals. 
#include<string.h> // to use string header : str.len() 
#include<sys/ipc.h> // used by three mechanisms for interprocess communication (IPC): messages, semaphores, and shared memory
#include<sys/shm.h>  // define the following symbolic constants: SHM_RDONLY. Attach read-only (else read-write).
#include<sys/mman.h> // process memory locking, or shared memory objects options 
#include<semaphore.h> // used to perform semaphore operations


#define NUM_CHILDREN 16
#define BUFFER_SIZE 256
#define RUN_TIME 1

// shared memory structure
typedef struct {
	char buffer[BUFFER_SIZE];
	sem_t mutex;
	int usage_count[NUM_CHILDREN];
} shared_data_t;

// linear backoff function
void linear_backoff(int attempt){
	// wait in milisec
	usleep(100*attempt);
}

// exponential backoff function
void exponential_backoff(int attempt){
	usleep(100*(1<<attempt));
}

void child_process( shared_data_t *shared_data, int id, int use_exponential ){
	int attempt = 0;
	int maxattempts = 20;
	while(attempt < maxattempts){
		// try to lock the buffer
		if(sem_trywait(&shared_data->mutex)==0){
			// critical section
			snprintf(shared_data->buffer, BUFFER_SIZE, "Hard work pays off in long run - PID : %d", getpid());
			
			shared_data->usage_count[id]++;
			printf("Child %d wrote to buffer: %s\n", id, shared_data->buffer);
			
			// unlock the buffer
			sem_post(&shared_data->mutex);
			// reset attempt counter
			attempt = 0;
			// simulate same work
			usleep(50000);
		}
		else{
			// backoff if buffer is locked
			if(use_exponential){
				exponential_backoff(attempt);
			}
			else{
				linear_backoff(attempt);
			}
			attempt++;
		}
	}
}

int main(){
	pid_t children[NUM_CHILDREN];
	shared_data_t *shared_data;
	
	// create shared memory
	// create new space in the virtual address space of the calling process
	shared_data = mmap(NULL, sizeof(shared_data_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0 );
	if(shared_data == MAP_FAILED){
		perror("mmap");
		exit(EXIT_FAILURE);
	}
	
	// initial shared memory
	sem_init(&shared_data->mutex, 1, 1);
	
	// binary semaphore
	for(int i=0;i<NUM_CHILDREN;i++){
		shared_data->usage_count[i] = 0;
	}
	
	// fork child processes
	for(int i=0;i<NUM_CHILDREN;i++){
		children[i] = fork();
		if(children[i] < 0){
			perror("fork");
			exit(EXIT_FAILURE);
		}
		if(children[i] == 0){
			// child process
			int use_exponential = 0;
			child_process(shared_data, i, use_exponential);
		}
		
	}
	// parent process run for specified time
	sleep(RUN_TIME);
	// kill all child processes
	for(int i=0;i<NUM_CHILDREN;i++){
		kill(children[i], SIGKILL);
		waitpid(children[i], NULL, 0);
	}
	
	// analyse and print results
	printf("\n Buffer usage statistics: \n");
	for(int i=0;i<NUM_CHILDREN;i++){
		printf("Child %d : %d accesses \n ", i, shared_data->usage_count[i]);
	}
	
	// cleanup
	sem_destroy(&shared_data->mutex);
	munmap(shared_data, sizeof(shared_data_t));
	
	return 0;
}















