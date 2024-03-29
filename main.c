/*
 * Create and listen on the TCP socket, respond any request to it
 * Created by Xiande Wen (xiandew, 905003)
 * Referenced from lab5/6, comp30023
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
#include "user.h"

/*----------------------------------------------------------------------------*/

int setup_socket(char *ip, char *port);
static char request[BUFFER_SIZE];

/*----------------------------------------------------------------------------*/

int main(int argc, char * argv[]) {

	if (argc < 3) {
		fprintf(stderr, "usage %s hostname port\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	// free allocated memory when terminating the program
	signal(SIGINT, free_users);

	char *ip = argv[1];
	char *port = argv[2];
	int sockfd = setup_socket(ip, port);

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
					bzero(request, BUFFER_SIZE);
					int n = read(i, request, BUFFER_SIZE - 1);
					if (n <= 0) {
						close(i);
						FD_CLR(i, &masterfds);
					} else {
						// create reponse message
						char *response = get_response(request);
						if (write(i, response, strlen(response)) < 0) {
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

int setup_socket(char *ip, char *port) {
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
	serv_addr.sin_port = htons(atoi(port));
	inet_pton(AF_INET, ip, &(serv_addr.sin_addr));

	// bind address to socket
	if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
		perror("bind");
		exit(EXIT_FAILURE);
	}

	// listen on the socket
	listen(sockfd, 5);

    printf("image_tagger server is now running at IP: %s on port %s\n", ip, port);
	return sockfd;
}
