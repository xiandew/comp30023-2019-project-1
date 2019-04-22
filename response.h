#ifndef RESPONSE_H
#define RESPONSE_H

#define MAX_PATH_LEN 128
#define HTML_PATH "html/"
#define HTML_PATH_FORMAT HTML_PATH "%s.html"

#define HTML_INTRO      "1_intro"
#define HTML_START      "2_start"
#define HTML_FIRST_TURN "3_first_turn"
#define HTML_ACCEPTED   "4_accepted"
#define HTML_DISCARDED  "5_discarded"
#define HTML_ENDGAME    "6_endgame"
#define HTML_GAMEOVER   "7_gameover"

#define BUFFER_SIZE 2048

#define HTTP_200_FORMAT "HTTP/1.1 200 OK\r\n\
Content-Type: text/html\r\n\
Content-Length: %ld\r\n\r\n%s"

#define HTTP_400 "HTTP/1.1 400 Bad Request\r\n\
Content-Length: 0\r\n\r\n"

#define HTTP_404 "HTTP/1.1 404 Not Found\r\n\
Content-Length: 0\r\n\r\n"

#define GET "GET / "
#define POST "POST / "

char *get_response(char *request);

#endif
