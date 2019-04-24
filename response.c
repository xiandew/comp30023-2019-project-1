/*
 * Module for creating response message according to the request
 * Created by Xiande Wen (xiandew, 905003)
 */

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

char *get_response(char *request) {
	bzero(response, BUFFER_SIZE);
	bzero(htmlbuff, BUFFER_SIZE);

	char *method = NULL;

	// parse methods
	if (!strncmp(request, GET, strlen(GET))) {
		request += strlen(GET);
		method = GET;
	} else if(!strncmp(request, POST, strlen(POST))) {
		request += strlen(POST);
		method = POST;
	}
	// invalid methods
	if (!method) {
		return HTTP_400;
	}

	char *route = NULL;

	// parse routes
	if (!strncmp(request, ROUTE_INTRO, strlen(ROUTE_INTRO))) {
		request += strlen(ROUTE_INTRO);
		route = ROUTE_INTRO;
	} else if(!strncmp(request, ROUTE_START, strlen(ROUTE_START))) {
		request += strlen(ROUTE_START);
		route = ROUTE_START;
	}
	// invalid routes
	if (!route) {
		return HTTP_404;
	}

	int user_id = get_user_id(request);
	// add a new user. Response with Set-cookie.
	if (user_id < 0) {
		user_t *new_user = add_new_user();
		read_html(HTML_INTRO);
		snprintf(response, BUFFER_SIZE,
			HTTP_200_SET_COOKIE_FORMAT, new_user->id, strlen(htmlbuff), htmlbuff);
		return response;
	}

	user_t *user = users[user_id];
	user_t *other = (user->other == NOT_PAIRED) ? NULL : users[user->other];

	// if user clicks the quit button or the other user has quited
	if (strstr(request, NEEDLE_QUIT) || (other && other->other != user->id)) {
		user->state = QUITED;
		reset_user(user);
		read_html(HTML_GAMEOVER);
		snprintf(response, BUFFER_SIZE, HTTP_200_FORMAT, strlen(htmlbuff), htmlbuff);
		return response;
	}

	if (!strcmp(method, GET)){
		// if users with cookies try to get the intro page
		if (!strcmp(route, ROUTE_INTRO)) {
			// proceed them to the start page
			read_html(HTML_START);
			append_name_to_html(user);
		}
		// if the user clicks the start button
		if (!strcmp(route, ROUTE_START)) {
			user->state = STARTED;
			read_html(HTML_FIRST_TURN);
			// first round
			if (user->other == NOT_PAIRED) {
				paired_up(user);
			}
			// next round
			else {
				user->state = PLAYING;
			}
		}
	}

	if (!strcmp(method, POST)) {
		// the user submits a name from intro page
		if (!strcmp(route, ROUTE_INTRO)) {
			char *payload = strstr(request, NEEDLE_NAME);
			if (payload) {
				char *name = payload + strlen(NEEDLE_NAME);
				add_name_to_user(user, name);
				read_html(HTML_START);
				append_name_to_html(user);
			}
		}
		// the user submits a keyword
		if (!strcmp(route, ROUTE_START)) {
			if (!other || user->round > other->round ||
				(user->round == other->round && other->state != PLAYING)) {
				read_html(HTML_DISCARDED);
			} else {
				*strstr(request, NEEDLE_GUESS) = 0;
				char *keyword =
					strstr(request, NEEDLE_KEYWORD) + strlen(NEEDLE_KEYWORD);
				add_keyword_to_user(user, keyword);
				if (user->round < other->round || has_submitted(other, keyword)) {
					user->state = SUCCEED;
					reset_user(user);
					read_html(HTML_ENDGAME);
				} else {
					read_html(HTML_ACCEPTED);
					append_keywords_to_html(user);
				}
			}
		}
	}

	// alternating images for 3_first_turn.html, 4_accepted.html and
	// 5_discarded.html each round.
	if (strstr(htmlbuff, "Keyword: <input")) {
		char *change_ptr = strstr(htmlbuff,
			".jpg\" alt=\"HTML5 Icon\" style=\"width:700px;height:400px;\">") - 1;
		if (user->round % 2) {
			*change_ptr = '2';
		} else {
			*change_ptr = '4';
		}
	}

	snprintf(response, BUFFER_SIZE, HTTP_200_FORMAT, strlen(htmlbuff), htmlbuff);
	return response;
}

/*----------------------------------------------------------------------------*/
//helper functions

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
	char *cookie_payload= strstr(request, NEEDLE_COOKIE);
	if (cookie_payload) {
		cookie_payload += strlen(NEEDLE_COOKIE);
		char cpy[strlen(cookie_payload)];
		strcpy(cpy, cookie_payload);
		cpy[strcspn(cookie_payload, "\r\n")] = 0;
		int user_id = atoi(cpy);
		if (user_id < num_users) {
			return user_id;
		}
	}
	return -1;
}

void append_name_to_html(user_t *user) {
	char *name = user->name;
	char *insert_ptr = strstr(htmlbuff, "<form method=\"GET\">");
	char remaining[strlen(insert_ptr)];
	strcpy(remaining, insert_ptr);
	strcpy(insert_ptr, "<p>");
	insert_ptr += 3;
	strcpy(insert_ptr, name);
	insert_ptr += strlen(name);
	strcpy(insert_ptr, "</p>");
	insert_ptr += 4;
	strcpy(insert_ptr, remaining);
}

void append_keywords_to_html(user_t *user) {
	char *insert_ptr = strstr(htmlbuff, "<form method=\"POST\">");
	char remaining[strlen(insert_ptr)];
	strcpy(remaining, insert_ptr);
	strcpy(insert_ptr, "<p>");
	insert_ptr += 3;
	for (int i = 0; i < user->num_keywords; i++) {
		char *keyword = user->keywords[i];
		strcpy(insert_ptr, keyword);
		insert_ptr += strlen(keyword);
		if (i < (user->num_keywords - 1)) {
			strcpy(insert_ptr, ", ");
			insert_ptr += 2;
		}
	}
	strcpy(insert_ptr, "</p>");
	insert_ptr += 4;
	strcpy(insert_ptr, remaining);
}

bool has_submitted(user_t *other, char *keyword) {
	for (int i = 0; i < other->num_keywords; i++) {
		if (!strcmp(other->keywords[i], keyword)) {
			return true;
		}
	}
	return false;
}
