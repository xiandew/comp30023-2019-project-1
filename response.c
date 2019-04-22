#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <fcntl.h>

#include "response.h"
#include "user.h"

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

int get_user_id(char *request) {
	char *cookie_payload= strstr(request, NEEDLE_COOKIE) + strlen(NEEDLE_COOKIE);
	cookie_payload[strcspn(cookie_payload, "\r\n")] = 0;
	return atoi(cookie_payload);
}

void append_name_to_html(char *name) {
	char *rest_ptr = strstr(htmlbuff, "<form method=\"GET\">");
	char rest[strlen(rest_ptr)];
	strcpy(rest, rest_ptr);
	char *ptr = rest_ptr;
	strcpy(ptr, "<p>");
	ptr += 3;
	strcpy(ptr, name);
	ptr += strlen(name);
	strcpy(ptr, "</p>");
	ptr += 4;
	strcpy(ptr, rest);
}

char *get_response(char *request) {
	bzero(response, BUFFER_SIZE);
	bzero(htmlbuff, BUFFER_SIZE);

	// response of the intro page
	if (!strncmp(request, GET_INTRO, strlen(GET_INTRO))) {
		// add a new user. Response with Set-cookie.
		if (!strstr(request, NEEDLE_COOKIE)) {
			read_html(HTML_INTRO);

			int user_id = num_users;
			add_user(new_user(user_id));
			snprintf(response, BUFFER_SIZE,
				HTTP_200_SET_COOKIE_FORMAT,
				user_id, strlen(htmlbuff), htmlbuff);
		}
	}

	// response to submission of the user name
	if (!strncmp(request, POST_NAME, strlen(POST_NAME))) {
		if (strstr(request, NEEDLE_NAME)) {
			char *name = strstr(request, NEEDLE_NAME) + strlen(NEEDLE_NAME);
			add_name_to_user(get_user_id(request), name);

			read_html(HTML_START);
			append_name_to_html(name);
		}
	}

	// response to press of the start button
	if (!strncmp(request, GET_START, strlen(GET_START))) {
		read_html(HTML_FIRST_TURN);
	}

	// response to submission of a keyword
	// if () {
	//
	// }

	// response to invalid requests
	if (!strlen(htmlbuff)) {
		if (!strncmp(request, GET, strlen(GET))) {
			strcpy(response, HTTP_404);
		} else {
			strcpy(response, HTTP_400);
		}
	}

	// response to users with cookies
	else if (!strlen(response)) {
		snprintf(response, BUFFER_SIZE, HTTP_200_FORMAT, strlen(htmlbuff), htmlbuff);
	}

	return response;
}
