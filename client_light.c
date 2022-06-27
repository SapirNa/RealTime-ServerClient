/*
** client_ligth.c
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>

#define PORT "4444"

int main(){
	int sockfd, ret;
	struct sockaddr_in serverAddr;
	socklen_t sin_size;
	
	int newSocket;
	struct sockaddr_in newAddr;
	
	char buffer[1024];
	pid_t childpid;
	
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd < 0){
		printf("error in connection\n");
		exit(1);		
	}
	printf("clientLigth: socket create\n");
	
	memset(&serverAddr, '\0', sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(4444);
	serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	
	ret = bind(sockfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
	if(ret < 0){
		printf("error in binding\n");
		exit(1);
	}
	printf("clientLigth: binding\n");
	
	if(listen(sockfd, 10) == 0)
		printf("listening....\n");
	else
		printf("error in binding\n");
	
	while(1){
		sin_size = sizeof newAddr;
		newSocket = accept(sockfd, (struct sockaddr *)&newAddr, &sin_size);
		if (newSocket == -1) {
			perror("accept");
			continue;
		}
		
		printf("clientLigth: got connectiong form %s:%d\n", inet_ntoa(newAddr.sin_addr), ntohs(newAddr.sin_port));
		if((childpid = fork()) == 0){
			close(sockfd);
			while(1){
				recv(newSocket, buffer, 1024,0);
				if(strcmp(buffer, "exit") == 0){
					printf("clientLigth: disconnect from client\n");
					exit(1);
				}
				else if (strcmp(buffer, "light-on") == 0){
					printf("server: %s\n", "light-on");
					send(newSocket, "ok", strlen(buffer) ,0);
				}
				else{
					printf("server: %s\n", "light-off");
					send(newSocket, "ok", strlen(buffer) ,0);
				}
			}
		}
	}
	return 0;
}
