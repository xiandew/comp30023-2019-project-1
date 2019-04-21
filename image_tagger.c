/**
 * Xiande Wen, 905003
 */

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <arpa/inet.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <strings.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "preg_match_all.h"

char *read_file(char *file_name);

int main(int argc, char * argv[])
{
	if (argc < 3)
	{
		fprintf(stderr, "usage %s hostname port\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	// create TCP socket which only accept IPv4
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
	{
		perror("socket");
		exit(EXIT_FAILURE);
	}

	// reuse the socket if possible
	int const reuse = 1;
	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(int)) < 0)
	{
		perror("setsockopt");
		exit(EXIT_FAILURE);
	}

	// create and initialise address we will listen on
	struct sockaddr_in serv_addr;
	bzero(&serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
	serv_addr.sin_port = htons(atoi(argv[2]));

	// bind address to socket
	if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
	{
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

	while (1)
	{
		// monitor file descriptors
		fd_set readfds = masterfds;
		if (select(FD_SETSIZE, &readfds, NULL, NULL, NULL) < 0)
		{
			perror("select");
			exit(EXIT_FAILURE);
		}

		// loop all possible descriptor
		for (int i = 0; i <= maxfd; ++i)
		{
			// determine if the current file descriptor is active
			if (FD_ISSET(i, &readfds))
			{
				// create new socket if there is new incoming connection request
				if (i == sockfd)
				{
					struct sockaddr_in cliaddr;
					socklen_t clilen = sizeof(cliaddr);
					int newsockfd = accept(sockfd, (struct sockaddr *)&cliaddr, &clilen);
					if (newsockfd < 0)
					perror("accept");
					else
					{
						// add the socket to the set
						FD_SET(newsockfd, &masterfds);
						// update the maximum tracker
						if (newsockfd > maxfd)
						maxfd = newsockfd;
						// print out the IP and the socket number
						char ip[INET_ADDRSTRLEN];
						printf(
							"new connection from %s on socket %d\n",
							// convert to human readable string
							inet_ntop(cliaddr.sin_family, &cliaddr.sin_addr, ip, INET_ADDRSTRLEN),
							newsockfd
						);
					}
				}
				// a message is sent from the client
				else
				{
					char buff[1024];
					bzero(buff, 1024);
					int n = read(i, buff, 1023);
					if (n <= 0)
					{
						if (n < 0)
						{
							perror("read");
						}
						else
						{
							printf("socket %d close the connection\n", i);
						}
						close(i);
						FD_CLR(i, &masterfds);
					}
					else
					{
						char *html_content = NULL;
						if (!strncmp(buff, "GET /", 5))
						{
							html_content = read_file("lab6-GET.html");
							sprintf(buff,
								"HTTP/1.1 200 OK\n"
								"Content-Type: text/html\n"
								"Content-Length: %ld\n\n%s",
								strlen(html_content), html_content
							);
						}
						if (!strncmp(buff, "POST /", 6))
						{
							char **matches = malloc(sizeof(char **));
							preg_match_all("user=[a-zA-Z]+", buff, matches);
							char *username_ptr = matches[0] + 5;
							char username[strlen(username_ptr)];
							strcpy(username, username_ptr);
							free(matches[0]);
							free(matches);

							html_content = read_file("lab6-POST.html");
							char *rest_ptr = strstr(html_content, "Thank you!");
							char rest[strlen(rest_ptr)];
							strcpy(rest, rest_ptr);
							char *ptr = rest_ptr;
							strcpy(ptr, username);
							ptr += strlen(username);
							strcpy(ptr++, ",");
							strcpy(ptr++, " ");
							strcpy(ptr, rest);

							sprintf(buff,
								"HTTP/1.1 200 OK\n"
								"Content-Type: text/html\n"
								"Content-Length: %ld\n\n%s",
								strlen(html_content), html_content
							);
						}

						if (write(i, buff, strlen(buff)) < 0)
						{
							perror("write");
							close(i);
							FD_CLR(i, &masterfds);
						}
						free(html_content);
					}
				}
			}
		}
	}

	return 0;
}

char *read_file(char *file_name)
{
	FILE *fp = fopen(file_name, "r");
	char ch, *content = malloc(sizeof(char) * 1024);
	bzero(content, 1024);
	while((ch = fgetc(fp)) != EOF) {
		content[strlen(content)] = ch;
	}
	fclose(fp);
	return content;
}
