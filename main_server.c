/*
** main_server.c
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

#define PORT "3490"

#define BACKLOG 10


void sigchld_handler(int s)
{
	// waitpid() might overwrite errno, so we save and restore it:
	int saved_errno = errno;
	
	while(waitpid(-1, NULL, WNOHANG) > 0);
	
	errno = saved_errno;
}


// get sockaddr, IPv4 or IPv6
void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET){
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}
	
	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

void write_file(int sockfd){
	int n;
	FILE *fp;
	char *filename = "logs_server.txt";
	char buffer[1024];
	
	fp = fopen(filename, "w");
	if(fp == NULL){
		perror("error in reading file.\n");
		exit(1);
	}
	
	while(1){
		n = recv(sockfd, buffer, 1024, 0);
		if(n <= 0){
			break;
			return;
		}
		fprintf(fp, "%s", buffer);
		bzero(buffer, 1024);
	}
	printf("server: data was write\n");
	return;
}

void light(char* massage){
	int serverSocket, ret;
	struct sockaddr_in serverAddr;
	char buffer[1024];
	
	serverSocket = socket(AF_INET, SOCK_STREAM,0);
	if(serverSocket < 0){
		printf("Error in connection.\n");
		exit(1);
	}
	printf("server: socket create\n");
	
	memset(&serverAddr, '\0', sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(4444);
	serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	
	ret = connect(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
	if(ret < 0){
		printf("Error in connection.\n");
		exit(1);
	}
	printf("sever: connect to clientLigth\n");
	
	send(serverSocket, massage, 15 , 0);
		
	if (strcmp(buffer, "exit") == 0){
		close(serverSocket);
		printf("disconnect from server");
		exit(1);
	}
	
	if(recv(serverSocket, buffer, 1024, 0) < 0) 
		printf("Error in recv data\n");
	else
		printf("clientLigth : \t%s\n", buffer);
}

int main(void)
{
	int sockfd, new_fd, ret;
	struct addrinfo hints, *servinfo, *p;
	socklen_t sin_size;
	struct sigaction sa;
	int yes = 1;
	char s[INET6_ADDRSTRLEN];
	int rv;
	char temp[100];
	int numbytes, count = 0;
	char newcount[10];
	
	struct sockaddr_in serverAddr;
	int newSocket;
	struct sockaddr_in newAddr;
	char buffer[1024];
	pid_t childpid;
	
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	
	memset(&serverAddr, '\0', sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(3490);
	serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	
	if ((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0){
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}
	
	// loop through all the results and bind to the first we can
	for(p = servinfo;  p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1){
			perror("server: socket");
			continue;
		}
		
		if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1){
			perror("setsockopt");
			exit(1);
		}
		
		if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1){
			close(sockfd);
			perror("server: bind");
			continue;
		}
		
		break;
	}
	
	freeaddrinfo(servinfo); // all done with this structure
	
	if (p == NULL) {
		fprintf(stderr, "server: failed to bind\n");
		exit(1);
	}
	
	if (listen(sockfd, BACKLOG) == -1) {
		perror("listen");
		exit(1);
	}
	
	sa.sa_handler = sigchld_handler; // reap all dead processes
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	if (sigaction(SIGCHLD, &sa, NULL) == -1) {
		perror("sigachtion");
		exit(1);
	}

	printf("server: waiting for connections...\n");
	
	int run = 1;
	while(run == 1) { // main accept() loop
		sin_size = sizeof newAddr;
		new_fd = accept(sockfd, (struct sockaddr *)&newAddr, &sin_size);
		if (new_fd == -1) {
			perror("accept");
			continue;
		}
		
		printf("server: got connectiong form %s:%d\n", inet_ntoa(newAddr.sin_addr), ntohs(newAddr.sin_port));
	
		if((childpid = fork()) == 0){
			close(sockfd);
			while(1){
				recv(new_fd, buffer, 1024,0);
				if(strcmp(buffer, "exit") == 0){
					printf("server: disconnect from clientWrite\n");
					exit(1);
				}
				else if(strcmp(buffer, "person-move") == 0){
					printf("clientMove: %s\n", "person-move");	
					send(new_fd, "ok", strlen(buffer) ,0);
					
				}
				else if(strcmp(buffer, "logs") == 0){
					write_file(new_fd);
					exit(1);
				}
				else if(strcmp(buffer, "light-on") == 0){
					light("light-on");
					bzero(buffer,sizeof(buffer));
					exit(0);
				}
				else if(strcmp(buffer, "light-off") == 0){
					light("light-off");
					bzero(buffer,sizeof(buffer));
					exit(0);
				}
				else if(strcmp(buffer, "shutdown") == 0){
					printf("server: doing shutdown to all clients\n");
					exit(1);
					run = 0;
				}
				else{
					printf("clientWrite: %s\n", buffer);
					send(new_fd, buffer, strlen(buffer),0);
					bzero(buffer,sizeof(buffer));
				}
			}
		}			
		close(new_fd);
	}	
	return 0;
}
