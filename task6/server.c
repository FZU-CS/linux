/* gcc server.c -o server -lpthread */
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

#define MAXLINE 1000 
#define LISTENQ 20 
#define PORT 5000 
#define MAXFD 20 

FILE *fp;
int i, maxi = -1;
int client[MAXFD];

void *get_client(void *sockfd) {
	char buf[MAXLINE];
	int rev;
	if ((int)sockfd < 0) printf("\nThe new client cannot enter the ChatRoom\n");
	else {
		printf("\nThe new client enter the ChatRoom...\n");
		do {
			memset(buf, 0, sizeof(buf)); 

			if ((rev = recv((int)sockfd, buf, 1024, 0)) < 0)
				printf("\nCannot read the user information.\n");
			
			if (rev == 0)
				printf("\nThe user ends up the connection.\n");
			else {
				printf("%s\n", buf); 
				for(i = 0;i <= maxi; i++)
					send(client[i],buf,strlen(buf)+1,0);
				fputs(buf,fp);
			}
	} while (rev != 0); 
		fclose(fp);
	}
	
	close((int)sockfd);
	return(NULL);
}

int main() {
	int connfd,listenfd,sockfd;  

	socklen_t length;
	
	fp = fopen("student.txt","w");
	
	struct sockaddr_in server;
	struct sockaddr tcpaddr;
	pthread_t tid;

	listenfd = socket(AF_INET,SOCK_STREAM,0); 
	
	if (listenfd < 0) {
		printf("Error: Cannot build up the socket.\n");
		exit(1);
	}
	
	memset(&server, 0, sizeof(server));
	server.sin_family = AF_INET;
	server.sin_port = htons(PORT);
	server.sin_addr.s_addr = htonl(INADDR_ANY);
	
	if (bind(listenfd, (struct sockaddr*)&server, sizeof(server))<0) {
		printf("Error: Cannot bind the socket.\n");
		exit(1); 
	}
	length = sizeof(server);

	if (getsockname(listenfd,(struct sockaddr*)&server,&length)<0) {
		printf("Error: Cannot get the server listen port.\n"); 
		exit(1);
	}

	for(i = 0; i < MAXFD; i++) {
		client[i] = -1; //initialize the client column
	}
	
	listen(listenfd, LISTENQ);
	printf("Server listen port: %d\n", ntohs(server.sin_port));
	printf("Welcome to this ChatRoom!\n");

	for (;;) {
		connfd = accept(listenfd, &tcpaddr, &length);
		for (i = 0; i < MAXFD; i++) {
			if (client[i] < 0) {
				client[i] = connfd;
				break;
			}
		}

		if (i == MAXFD-1) {
			printf("The user number has exceeded the threshold.\n"); 
			exit(0);
		}

		if (i > maxi) maxi = i;
		pthread_create(&tid, NULL, &get_client, (void*)connfd); 
	}
	return 0;
}