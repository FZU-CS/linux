#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

int father_process_waiting_flag = 1; 
int child_process_id = 0; 

void father_process_wait() {
	while (father_process_waiting_flag) {
		printf("Father Process Waiting\n");
		sleep(1);
	}
}

void child_process_wait() {
	while (1) {
		printf("Child Process Waiting\n");
		sleep(2);
	}
}

void father_process_stop() {
	if (child_process_id) {  
		kill(child_process_id, SIGUSR1); 
		wait(0); 
		printf("Child Process Exited\n");
		child_process_id = 0;
	} else { 
		printf("Father Process Exiting\n");
		father_process_waiting_flag = 0;
	}
}

void child_process_stop() {
	printf("Child Process Exiting\n");
	exit(0);
}

int main() {
	child_process_id = fork();
	
	if (child_process_id < 0) {  
		perror("");
		exit(child_process_id);
	}
	
	if (child_process_id) { 
		signal(SIGINT, father_process_stop); 
		father_process_wait(); 
	} else { 
		signal(SIGINT, SIG_IGN); 
		signal(SIGUSR1, child_process_stop); 
		child_process_wait(); 
	}

	return 0;
}
