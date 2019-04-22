#ifndef PLAYER_H
#define PLAYER_H

#define MIN_PLAYERS 2
#define MIN_KEYWORD 1

// id: starts from 0.
// other: id of the other player. -1 means the player is not playing.
// name: player's name.
// keywords: player's guessed words.
// max_keywords: number of keywords that can be stored in the list.
// num_keywords: number of keywords that have been recorded.
typedef struct {
	int id;
	int other;
	char *name;
	char **keywords;
	int max_keywords;
	int num_keywords;
} player_t;

extern player_t** players;
extern int max_players;
extern int num_players;

player_t *new_player(int id, char *name);
void free_players();

#endif
