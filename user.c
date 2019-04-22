#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include "user.h"

// record all users throughout the whole running session of the server
user_t **users = NULL;
// number of users that can be stored in the list
int max_users = MIN_USERS;
// number of users that have been recorded
int num_users = 0;

user_t *new_user(int id) {
    user_t *user = malloc(sizeof(user_t));

    user->id = id;
    user->other = -1;
    for (int i = 0; i < num_users; i++) {
        if (users[i]->other < 0) {
            user->other = i;
            break;
        }
    }

    user->keywords = malloc(sizeof(char*) * MIN_KEYWORD);
    user->max_keywords = MIN_KEYWORD;
    user->num_keywords = 0;

    return user;
}

void add_user(user_t *user) {
    if (users == NULL) {
        users = malloc(sizeof(user_t*) * MIN_USERS);
    }
    if (num_users >= max_users) {
        max_users *= 2;
        users = realloc(users, sizeof(user_t*) * max_users);
    }
    users[num_users++] = user;
}

void add_name_to_user(int id, char *name) {
    user_t *user = users[id];
    user->name = malloc(sizeof(char) * (strlen(name) + 1));
    bzero(user->name, strlen(name) + 1);
    strcpy(user->name, name);
}

void free_users() {
    for (int i = 0; i < num_users; i++) {
        user_t *user = users[i];
        free(user->name);
        for (int j = 0; j < user->num_keywords; j++) {
            free(user->keywords[j]);
        }
        free(user->keywords);
        free(user);
    }
    free(users);
}
