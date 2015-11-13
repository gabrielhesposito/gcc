/*
 ** server.c -- a stream socket server demo
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

#include <sys/stat.h>

#define PORT "50000"  // the port users will be connecting to

#define BACKLOG 10     // how many pending connections queue will hold

void sigchld_handler(int s)
{
	// waitpid() might overwrite errno, so we save and restore it:
	int saved_errno = errno;

	while(waitpid(-1, NULL, WNOHANG) > 0);

	errno = saved_errno;
}


// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

void talk_to_child(int id, int childfd) {

	char messagebuffer[256];
	int i;
	int n;
	int sent;

	//for (i = 0; i < 3; i++) {
		n = sprintf(messagebuffer, "echo \"%d - %d\"", id, i);
		messagebuffer[n] = '\0';

		if ((sent = send(childfd, messagebuffer, strlen(messagebuffer) + 1, 0)) == -1)
			perror("send");

		printf("data sent: %d bytes\n", sent);
	//}

	close(childfd);
	exit(0);
}

/*void sendf(int id, int childfd, char* fname) {
	FILE *f = fopen(fname, "r");
	int size = 0;
	char sendsize[1];
	while (!feof(f))
	{
		fgetc(f);
		size++;
	}
	printf("\t%d\n", size);
	
	char buffer[256];

	sendsize[0] = size;

	fread((void*) buffer, 1, size, f);
	send(childfd, sendsize, 1, 0);
	int n = send(childfd, buffer, size, 0);
	printf("\t%d\n", n);

	fclose(f);
}*/

void sendf(int id, int childfd, char* fname) {
	FILE* f = fopen(fname, "r");
	char byte;
	char buf[1];
	
	while (1) {
		byte = fgetc(f);
		if (feof(f))
			break;
		buf[0] = byte;
		send(childfd, buf, 1, 0);
	}

	close(childfd);

	fclose(f);
}

int find(int arr[], int len, int n) {
	int i;
	for (i = 0; i < len; ++i)
		if (arr[i] == n)
			return 1;
	return 0;
}

int forkid = 0;

int children[32];
int nchildren = 0;

int avail[32];
int navail = 0;

char* commands[] = {"sh clientrun.sh", "sleep 5", "sleep 10", "close"};
int ncommands = 4;

void nextcommand() {
	;
}

void initnode(int fd) {
	sendf(forkid, fd, "payload.tar");
}

int startserver(void)
{
	int sockfd, new_fd;  // listen on sock_fd, new connection on new_fd
	struct addrinfo hints, *servinfo, *p;
	struct sockaddr_storage their_addr; // connector's address information
	socklen_t sin_size;
	struct sigaction sa;
	int yes=1;
	char s[INET6_ADDRSTRLEN];
	int rv;

	int i;
//	char buf[256];

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE; // use my IP

	if ((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}

	// loop through all the results and bind to the first we can
	for(p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype,
						p->ai_protocol)) == -1) {
			perror("server: socket");
			continue;
		}

		if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
					sizeof(int)) == -1) {
			perror("setsockopt");
			exit(1);
		}

		if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			close(sockfd);
			perror("server: bind");
			continue;
		}

		break;
	}

	freeaddrinfo(servinfo); // all done with this structure

	if (p == NULL)  {
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
		perror("sigaction");
		exit(1);
	}

	printf("server: waiting for connections...\n");

	while(1) {  // main accept() loop
		sin_size = sizeof their_addr;
		new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
		if (new_fd == -1) {
			perror("accept");
			continue;
		}

		inet_ntop(their_addr.ss_family,
				get_in_addr((struct sockaddr *)&their_addr),
				s, sizeof s);
		printf("server: got connection from %s\n", s);

		//if (!fork()) { // this is the child process
		//	close(sockfd); // child doesn't need the listener
			//talk_to_child(forkid, new_fd);

		if (find(children, nchildren, new_fd)) {

			clone(NULL, (void*) 0, 0, (void*) 0);
			//avail[navail++] = new_fd;

			if (forkid <= ncommands)
				send(new_fd, commands[forkid - 1], strlen(commands[forkid - 1]) + 1, 0);
			//recv(new_fd, buf, 256, 0);
		}
		else { //first run
			// should fork here
			initnode(new_fd); 
			// end fork
			children[nchildren++] = new_fd;
			//close(new_fd);
		}

		//	exit(0);
		//}
		

		close(new_fd);
		forkid++;
		if (forkid > ncommands) return 0;

		for (i = 0; i < navail; i++) {
			printf("%d\t", avail[i]);
		}
		printf("\n");
		//close(new_fd);  // parent doesn't need this
	}

}

int main(int argc, char** argv) {
	startserver();

	return 0;
}
