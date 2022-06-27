
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 3490

void send_file(FILE *fp, int sockfd){
	char data[1024] = {0};
	
	send(sockfd, "logs", 5, 0);
	while(fgets(data, 1024, fp) != NULL){
		if(send(sockfd, data, sizeof(data), 0) == -1){
			perror("error in sending data.\n");
			exit(1);
		}
		bzero(data, 1024);
	}
	close(sockfd);
	return;
}

int main(){
	int clientSocket, ret;
	struct sockaddr_in serverAddr;
	char buffer[1024];
	FILE *fp;
	char *filename = "logs_client.txt";
	
	clientSocket = socket(AF_INET, SOCK_STREAM,0);
	if(clientSocket < 0){
		printf("Error in connection.\n");
		exit(1);
	}
	printf("clientLogs: socket create\n");
	
	memset(&serverAddr, '\0', sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(PORT);
	serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	
	ret = connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
	if(ret < 0){
		printf("Error in connection.\n");
		exit(1);
	}
	printf("clientLogs: connect\n");
	
	fp = fopen(filename, "r");
	if(fp == NULL){
		perror("error in reading file. \n");
		exit(1);
	}
	
	send_file(fp, clientSocket);
	printf("file data send sucessfully.\n");
	
	close(clientSocket);
	
	return 0;
}
