#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "GOBANG.h"
#define PORT 16923

int main()
{
	char inputbuf[100];
    char recbuf[100];
    
	int final = 0;
	int over = 0;
	int player = 1;
	int x, y;

	int role = 0;
	int server_sockfd, client_sockfd;
	struct sockaddr_in server_addr = {0};
	struct sockaddr_in client_addr = {0};
	int client_sockaddr_len;
    int optval = 1;
	do {
		printf("(L)isten , (C)onnect or (E)xit:");
		scanf("%s", inputbuf);
		switch(inputbuf[0]) {
		case 'L':case 'l': 
			server_sockfd = socket(AF_INET, SOCK_STREAM, 0); 
			if(server_sockfd == -1) {
				perror("");
				return 1;
			}
			if((setsockopt(server_sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)))  == -1) { //设置套接字选项
				perror("Setsockopt Error");
				return 1;
			}
			server_addr.sin_family = AF_INET;
			server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
			server_addr.sin_port = htons(PORT);
			if(bind(server_sockfd, (struct sockaddr *) &server_addr, sizeof(server_addr)) == -1) { //套接字绑定
				perror("Bind Error");
				return 1;
			}
			if(listen(server_sockfd, 5) == -1) { 
				perror("Listen Error");
				return 1;
			}
			printf("Waiting client...\n");
			client_sockaddr_len = sizeof(client_addr);
			client_sockfd = accept(server_sockfd, (struct sockaddr *) &client_addr, (socklen_t *__restrict) &client_sockaddr_len); //等待客户端连接
			if(client_sockfd == -1) {
				perror("");
				return 1;
			}
			role = 1; 
			break;
		case 'C':case 'c': 
			printf("Server address:");
			scanf("%s", inputbuf);
			client_sockfd = socket(AF_INET, SOCK_STREAM, 0); 
			if(client_sockfd == -1) {
				perror("");
				return 1;
			}
			client_addr.sin_family = AF_INET;
			client_addr.sin_port = htons(PORT);
			client_addr.sin_addr.s_addr = inet_addr(inputbuf); 
			if(client_addr.sin_addr.s_addr == -1) {
				printf("Address is invalid");
				return 1;
			}
			server_sockfd = connect(client_sockfd, (struct sockaddr *)(&client_addr), sizeof(client_addr)); //连接服务器
			if(server_sockfd == -1) {
				perror("Connect failed");
				return 1;
			}
			role = 2; 
			break;
		case 'E':case 'e':
			return 0;
		default:
			role = 0;
		}
	} while(!role);
    
	do {
		printtable(); 
		if(player == role) { 
			do {
				printf("Player %s (INPUT:Ln or (Q)uit):", tablechar[player]);
				scanf("%s", inputbuf);
				ipttoop(inputbuf, &x, &y, &over);
			} while((!over) && (x < 0 || y < 0));
			send(client_sockfd, inputbuf, sizeof(inputbuf), 0); 
		} else { 
			printf("Waiting Player %s ...\n", tablechar[player]);
			recv(client_sockfd, recbuf, sizeof(recbuf), 0); 
			ipttoop(recbuf, &x, &y, &over);
		}
		if(over)break;
		if(set(x, y, player)) { 
			printf("Set Error!\n"); 
		} else {
			player = 3 - player; 
		}
		final = check(); 
	} while(!final && !over);
	if(!over) {
		printtable();
		switch(final) {  
		case 1:
			printf("Player () WIN!\n");
			break;
		case 2:
			printf("Player >< WIN!\n");
			break;
		case 3:
			printf("DRAW!\n");
			break;
		default:
			printf("ERROR");
		}
	}
	close(server_sockfd);
	close(client_sockfd);
	return 0;
}