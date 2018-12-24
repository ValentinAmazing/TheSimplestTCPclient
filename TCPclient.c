#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>


#define SIZE_BUFF 510

void destroy(int *sockfd, char *msg);
void *listener(void* sockfd);


int main(int argc, char* argv[])
{
	if(argc < 2)
	{
		printf("Usage:\n%s host <port>\n by default port 23\n", argv[0]);
		return 0;
	}


	int sd = socket(AF_INET, SOCK_STREAM, 0);
	if(0 > sd)
		destroy(&sd, "socket");

	struct sockaddr_in addr;
	if(1 != inet_pton(AF_INET, argv[1], (void*) &addr.sin_addr) )
		destroy(&sd, "inet_pton");

	if(argc > 2)
		addr.sin_port = htons(atoi(argv[2]));
	else
		addr.sin_port = htons(IPPORT_TELNET);
	addr.sin_family = AF_INET;

	if(0 > connect(sd, (struct sockaddr*) &addr, sizeof(addr)) )
		destroy(&sd, "connect");

	pthread_t thr;
	if(0 != (errno = pthread_create(&thr, NULL, &listener, (void*) &sd)) )
		destroy(&sd, "Error pthread_create");

	char buff[SIZE_BUFF] = {0};
	int rxtx;
	while(1)
	{
		if(NULL == fgets(buff, SIZE_BUFF, stdin) )
			perror("Error of fgets function ");

		rxtx = send(sd, &buff, strlen(buff), 0);
		if (0 < rxtx)
			continue;
		else if(0 == rxtx)
			destroy(&sd, "connection closed by foreign host");
		else if(0 > rxtx )
			perror("Error of send function ");
		else 
			fprintf(stderr, "impossible sent value: %d\n", rxtx);

	}

	destroy(&sd, NULL);
	printf("---end program?!\n");
}


void *listener(void* sockfd)
{
	int sd = *((int*) sockfd);
	int rxtx = 1;
	char buff[SIZE_BUFF] = {0};
	while(1)
	{
		rxtx = recv(sd, buff, SIZE_BUFF, 0);
		if(0 < rxtx)
		{
			buff[rxtx] = '\0';
			if(0 > puts(buff) )
				perror("Error of puts function ");
		}
		else if(0 == rxtx)
			destroy(&sd, "connection closed by foreign host");
		else if(0 > rxtx)
			perror("Error of recv function ");
		else
			fprintf(stderr, "impossible received value: %d\n", rxtx);
	}

}

void destroy(int *sockfd, char *msg)
{
	perror(msg);
	if(0 < *sockfd)
		close(*sockfd);
	exit(EXIT_FAILURE);
}
