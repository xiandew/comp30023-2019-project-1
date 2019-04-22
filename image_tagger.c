/**
* Xiande Wen, 905003
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <signal.h>

#include <arpa/inet.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <strings.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "response.h"
#include "player.h"

static char buffer[BUFFER_SIZE];

int main(int argc, char * argv[])
{
	signal(SIGINT, free_players);

	if (argc < 3) {
		fprintf(stderr, "usage %s hostname port\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	// create TCP socket which only accept IPv4
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) {
		perror("socket");
		exit(EXIT_FAILURE);
	}

	// reuse the socket if possible
	int const reuse = 1;
	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(int)) < 0) {
		perror("setsockopt");
		exit(EXIT_FAILURE);
	}

	// create and initialise address will be listened on
	struct sockaddr_in serv_addr;
	bzero(&serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(atoi(argv[2]));
	inet_pton(AF_INET, argv[1], &(serv_addr.sin_addr));

	// bind address to socket
	if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
		perror("bind");
		exit(EXIT_FAILURE);
	}

	// listen on the socket
	listen(sockfd, 5);

	// initialise an active file descriptors set
	fd_set masterfds;
	FD_ZERO(&masterfds);
	FD_SET(sockfd, &masterfds);

	// record the maximum socket number
	int maxfd = sockfd;

	while (1) {
		// monitor file descriptors
		fd_set readfds = masterfds;
		if (select(FD_SETSIZE, &readfds, NULL, NULL, NULL) < 0) {
			perror("select");
			exit(EXIT_FAILURE);
		}

		// loop all possible descriptor
		for (int i = 0; i <= maxfd; ++i) {
			// determine if the current file descriptor is active
			if (FD_ISSET(i, &readfds)) {
				// create new socket if there is new incoming connection request
				if (i == sockfd) {
					int newsockfd = accept(sockfd, NULL, NULL);
					if (newsockfd < 0) {
						perror("accept");
					} else {
						// add the socket to the set
						FD_SET(newsockfd, &masterfds);
						// update the maximum tracker
						if (newsockfd > maxfd) {
							maxfd = newsockfd;
						}
					}
				}
				// a message is sent from the client
				else {
					bzero(buffer, BUFFER_SIZE);
					int n = read(i, buffer, BUFFER_SIZE - 1);
					if (n <= 0) {
						if (n < 0) {
							perror("read");
						} else {
							printf("socket %d close the connection\n", i);
						}
						close(i);
						FD_CLR(i, &masterfds);
					} else {
						// create reponse message and write into buffer
						response(buffer);

						if (write(i, buffer, strlen(buffer)) < 0) {
							perror("write");
							close(i);
							FD_CLR(i, &masterfds);
						}
					}
				}
			}
		}
	}
	return 0;
}
