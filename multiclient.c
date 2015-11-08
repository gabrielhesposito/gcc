/*
 ** client.c -- a stream socket client demo
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include <arpa/inet.h>

#define PORT "50000" // the port client will be connecting to 

#define MAXDATASIZE 100 // max number of bytes we can get at once 

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int obey(int sockfd, char* buf) {

	int numbytes;

	while (1) {
		if ((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1) {
			perror("recv");
			close(sockfd);
			exit(1);
		}

		if (numbytes == 0) {
			close(sockfd);
			printf("closed\n");
			return 0;
		}

		buf[numbytes] = '\0';

		printf("client: received '%s'\n",buf);
		//system(buf);

		if (strcmp(buf, "close") == 0) {
			close(sockfd);
			printf("closed");
			return 0;
		}
	}
}

int main(int argc, char *argv[])
{
	int sockfd, numbytes;  
	char buf[MAXDATASIZE];
	struct addrinfo hints, *servinfo, *p;
	int rv;
	char s[INET6_ADDRSTRLEN];

	char* ipaddrs[32];
	size_t linebufN = 256;
	char* linebuf = (char*) malloc(linebufN);
	int numipaddr = 0;
	int numchar;
	FILE* iplist;

	if (argc == 1) {
		iplist = fopen("iplist.txt", "r");
		while ((numchar = getline(&linebuf, &linebufN, iplist)) != -1) {
			ipaddrs[numipaddr] = (char*) malloc(numchar + 1);
			strcpy(ipaddrs[numipaddr], linebuf);
			ipaddrs[numipaddr][numchar - 1] = '\0'; // remove '\n'
			numipaddr++;
		}
		fclose(iplist);
	} 
	else if (argc == 2) {
		ipaddrs[0] = argv[1];
		numipaddr++;
	}
	else {	
		fprintf(stderr,"usage: client hostname\n");
		exit(1);
	}

	printf("%d\n", numipaddr);
	int i;
	for (i = 0; i < numipaddr; i++) {
		printf("%s\n", ipaddrs[i]);

		memset(&hints, 0, sizeof hints);
		hints.ai_family = AF_INET;
		hints.ai_socktype = SOCK_STREAM;

		if ((rv = getaddrinfo(ipaddrs[i], PORT, &hints, &servinfo)) != 0) {
			fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
			return 1;
		}

		// loop through all the results and connect to the first we can
		for(p = servinfo; p != NULL; p = p->ai_next) {
			if ((sockfd = socket(p->ai_family, p->ai_socktype,
							p->ai_protocol)) == -1) {
				perror("client: socket");
				continue;
			}

			if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
				close(sockfd);
				perror("client: connect");
				continue;
			}

			break;
		}

		if (p != NULL) break;
	}

	if (p == NULL) {
		fprintf(stderr, "client: failed to connect\n");
		return 2;
	}

	// found parent

	inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
			s, sizeof s);
	printf("client: connecting to %s\n", s);

	freeaddrinfo(servinfo); // all done with this structure

	obey(sockfd, buf);
	if ((sockfd = socket(p->ai_family, p->ai_socktype,
					p->ai_protocol)) == -1) {
		perror("client: socket");
	}

	if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
		close(sockfd);
		perror("client: connect");
	}
	obey(sockfd, buf);

	return 0;
}
