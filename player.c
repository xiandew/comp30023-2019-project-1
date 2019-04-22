#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include "player.h"

// record all players throughout the whole running session of the server
player_t **players = NULL;
// number of players that can be stored in the list
int max_players = MIN_PLAYERS;
// number of players that have been recorded
int num_players = 0;

player_t *new_player(int id) {
    player_t *player = malloc(sizeof(player_t));

    player->id = id;
    player->other = -1;
    for (int i = 0; i < num_players; i++) {
        if (players[i]->other < 0) {
            player->other = i;
            break;
        }
    }

    player->keywords = malloc(sizeof(char*) * MIN_KEYWORD);
    player->max_keywords = MIN_KEYWORD;
    player->num_keywords = 0;

    return player;
}

void add_player(player_t *player) {
    if (players == NULL) {
        players = malloc(sizeof(player_t*) * MIN_PLAYERS);
    }
    if (num_players >= max_players) {
        max_players *= 2;
        players = realloc(players, sizeof(player_t*) * max_players);
    }
    players[num_players++] = player;
}

void add_name_to_player(int id, char *name) {
    player_t *player = players[id];
    player->name = malloc(sizeof(char) * strlen(name) + 1);
    bzero(player->name, strlen(name) + 1);
    strcpy(player->name, name);
}

void free_players() {
    for (int i = 0; i < num_players; i++) {
        player_t *player = players[i];
        free(player->name);
        for (int j = 0; j < player->num_keywords; j++) {
            free(player->keywords[j]);
        }
        free(player->keywords);
        free(player);
    }
    free(players);
}
