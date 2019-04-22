#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "response.h"
#include "reg_match.h"
#include "player.h"

static char html_buffer[BUFFER_SIZE];

char *read_file(char *file_name) {
	File *fp = open(file_name, "r");
	bzero(html_buffer, BUFFER_SIZE);
	int n = read(fp, html_content, BUFFER_SIZE - 1);
	if (n <= 0) {
		perror("read")
	}
	return html_buffer;
}

void response(char *buffer) {
	if (!strncmp(buffer, "GET /", 5)) {
		html_content = read_file(PAGE_INTRO);
		snprintf(buffer, BUFFER_SIZE,
			"HTTP/1.1 200 OK\r\n"
			"Content-Type: text/html\r\n"
			"Content-Length: %ld\r\n\r\n%s",
			strlen(html_content), html_content);
	}
	if (!strncmp(buffer, "POST /", 6)) {
		char **matches = malloc(sizeof(char **));
		reg_match("user=[a-zA-Z]+", buffer, matches);
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

		snprintf(buffer, BUFFER_SIZE
			"HTTP/1.1 200 OK\r\n"
			"Content-Type: text/html\r\n"
			"Content-Length: %ld\r\n\r\n%s",
			strlen(html_content), html_content);
	}
}
