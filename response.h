#ifndef RESPONSE_H
#define RESPONSE_H

#define HTML_PATH "html_pages/"
#define HTML_PATH_FORMAT HTML_PATH "%s.html"

#define PAGE_INTRO      "1_intro"
#define PAGE_START      "2_start"
#define PAGE_FIRST_TURN "3_first_turn"
#define PAGE_ACCEPTED   "4_accepted"
#define PAGE_DISCARDED  "5_discarded"
#defien PAGE_ENDGAME    "6_endgame"
#define PAGE_GAMEOVER   "7_gameover"

#define BUFFER_SIZE 2048

void response(char *buff);
