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
	bzero(htmlbuff, BUFFER_SIZE);

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

METHOD get_method(char *request) {
	if (!strncmp(request, "GET /", 5)) {
		return GET;
	}
	if (!strncmp(request, "POST /", 6)) {
		return POST;
	}
	return UNKNOWN;
}

char *get_response(char *request) {

	METHOD method = get_method(request);

	bzero(response, BUFFER_SIZE);

	if (method == GET) {
		read_html(HTML_INTRO);
		snprintf(response, BUFFER_SIZE,
			"HTTP/1.1 200 OK\r\n"
			"Content-Type: text/html\r\n"
			"Content-Length: %ld\r\n\r\n%s",
			strlen(htmlbuff), htmlbuff);
	}
	// if (!strncmp(request, "POST /", 6)) {
	// 	char **matches = malloc(sizeof(char **));
	// 	reg_match("user=[a-zA-Z]+", request, matches);
	// 	char *username_ptr = matches[0] + 5;
	// 	char username[strlen(username_ptr)];
	// 	strcpy(username, username_ptr);
	// 	free(matches[0]);
	// 	free(matches);
	//
	// 	html_content = read_html("lab6-POST.html");
	// 	char *rest_ptr = strstr(html_content, "Thank you!");
	// 	char rest[strlen(rest_ptr)];
	// 	strcpy(rest, rest_ptr);
	// 	char *ptr = rest_ptr;
	// 	strcpy(ptr, username);
	// 	ptr += strlen(username);
	// 	strcpy(ptr++, ",");
	// 	strcpy(ptr++, " ");
	// 	strcpy(ptr, rest);
	//
	// 	snprintf(response, BUFFER_SIZE
	// 		"HTTP/1.1 200 OK\r\n"
	// 		"Content-Type: text/html\r\n"
	// 		"Content-Length: %ld\r\n\r\n%s",
	// 		strlen(html_content), html_content);
	// }
	return response;
}
