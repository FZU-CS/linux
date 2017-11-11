/* gcc client.c -o client -lpthread */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>
#include <pthread.h>
#define TRUE 1
#define PORT 5000

int quit = 0; 

void *get_server(void* sockfd) {
	char buf[1024];
	int rev;

	if (((int)sockfd) < 0)
		printf("\nCannot get the server information.\n");
	else {
		printf("\n\007\n");
		for (;;) {
			if (!quit) {
				if ((rev = recv((int)sockfd, buf, 1024, 0)) > 0)
					printf("\n\007%s\n", buf);
				
				if (rev == 0) {
					printf("\nThe server ends up the connection.\n");
					quit = 1;
					continue;
				}
				printf("\n");
			} else {
				close((int)sockfd); 
				break;
			}
		}
		return(NULL);
	}
}

int main() {
	int connfd,snd,slenth;
	
	struct sockaddr_in server;
	struct hostent *hp;
	
	char honame[20], msg2[1024], msg1[1024], cln[102], qstr[] = {"Quit"}; 

	pthread_t tid;

	printf("Please enter the server IP address.\n");
	scanf("%s*", honame);
	printf("Try to connect the server using socket...\n");

	if ((connfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
		printf("Error: Cannot create the socket.\n");
	
	if ((hp = gethostbyname(honame)) == NULL) {
		printf("Error: Cannot get the server IP address\n");
		exit(1);
	} else printf(" \n");
	
	memcpy(&server.sin_addr, hp->h_addr, hp->h_length); 

	server.sin_family = AF_INET;
	server.sin_port = htons(PORT);
	
	if (connect(connfd, (struct sockaddr*)&server, sizeof(server)) < 0) {
		printf("Error: Cannot connect to the server.\n");
		exit(1);
	}

	printf("Build up the connection successfully.\n"); 
	printf("Welcome to the ChatRoom.\n");

	printf("Please input your username:\n");

	scanf("%s",msg1);
	
	slenth = strlen(msg1);
	msg1[slenth] = ':';
	msg1[slenth+1] = '\0';

	strcpy(cln, msg1); 
	pthread_create(&tid, NULL, &get_server, (void*)connfd); 

	printf("\nLet's start chatting!\n");
	
	while (TRUE) {
		printf("\n");
		scanf("%s", msg2);

		if (strcmp(msg2, qstr) == 0) {
			close(connfd);
			quit = 1; 
		} else {
			strcat(msg1, msg2); 
			snd = send(connfd, msg1, strlen(msg1)+1, 0); 
			strcpy(msg1, cln);

			if (snd < 0) printf("\n \n");
		}
	}

	return 0;
}
