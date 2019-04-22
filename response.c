#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <fcntl.h>

#include "response.h"
#include "reg_match.h"
#include "player.h"

/*----------------------------------------------------------------------------*/

static char htmlbuff[BUFFER_SIZE];
static char response[BUFFER_SIZE];

/*----------------------------------------------------------------------------*/

// read the html contents into htmlbuff
void read_html(char *htmlname) {
	// get the path of the html file
	char htmlpath[MAX_PATH_LEN];
	bzero(htmlpath, MAX_PATH_LEN);
	snprintf(htmlpath, MAX_PATH_LEN, HTML_PATH_FORMAT, htmlname);

	// read html file
	int fd = open(htmlpath, O_RDONLY);
	int n = read(fd, htmlbuff, BUFFER_SIZE - 1);
	if (n <= 0) {
		perror("read");
	}
	close(fd);
}

char *get_response(char *request) {
	bzero(response, BUFFER_SIZE);
	bzero(htmlbuff, BUFFER_SIZE);

	if (!strncmp(request, GET_INTRO, strlen(GET_INTRO))) {
		read_html(HTML_INTRO);
	}
	if (!strncmp(request, GET_START, strlen(GET_START))) {
		read_html(HTML_FIRST_TURN);
	}
	if (!strncmp(request, POST, strlen(POST))) {
		char *username = strstr(request, "user=") + 5;

		read_html(HTML_START);
		char *rest_ptr = strstr(htmlbuff, "<form method=\"GET\">");
		char rest[strlen(rest_ptr)];
		strcpy(rest, rest_ptr);
		char *ptr = rest_ptr;
		strcpy(ptr, "<p>");
		ptr += 3;
		strcpy(ptr, username);
		ptr += strlen(username);
		strcpy(ptr, "</p>");
		ptr += 4;
		strcpy(ptr, rest);
	}

	if (!strlen(htmlbuff)) {
		if (!strncmp(request, GET, strlen(GET))) {
			strcpy(response, HTTP_404);
		} else {
			strcpy(response, HTTP_400);
		}
	} else {
		snprintf(response, BUFFER_SIZE, HTTP_200_FORMAT, strlen(htmlbuff), htmlbuff);
	}
	return response;
}
