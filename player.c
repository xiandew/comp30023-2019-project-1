#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include "player.h"

// record all players throughout the whole running session of the server
player_t **players = NULL;
// number of players that can be stored in the list
int max_players = 2;
// number of players that have been recorded
int num_players = 0;

player_t *new_player(int id, char *name) {
    if (players == NULL) {
        players = malloc(sizeof(player_t*) * MIN_PLAYERS);
    }
    player_t *player = malloc(sizeof(player_t));
    player->name = malloc(sizeof(char) * strlen(name) + 1);
    bzero(player->name, strlen(name) + 1);
    strcpy(player->name, name);
    player->id = id;
    player->other = -1;
    player->keywords = malloc(sizeof(char*) * MIN_KEYWORD);
    player->max_keywords = MIN_KEYWORD;
    player->num_keywords = 0;
    return player;
}

void free_players() {

}
