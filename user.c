/*
 * Module for creating and recording the user-related information
 * Created by Xiande Wen (xiandew, 905003)
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <strings.h>

#include "user.h"

// record all users throughout the whole running session of the server
user_t **users = NULL;
// number of users that can be stored in the list
int max_users = MIN_USERS;
// number of users that have been recorded
int num_users = 0;

user_t *new_user() {
    user_t *user = malloc(sizeof(user_t));

    user->id = num_users;
    user->other = NOT_PAIRED;
    user->state = NOT_PAIRED;
    user->round = 1;
    user->num_keywords = 0;

    return user;
}

user_t *add_new_user() {
    user_t *user = new_user();

    if (users == NULL) {
        users = malloc(sizeof(user_t*) * MIN_USERS);
    }
    if (num_users >= max_users) {
        max_users *= 2;
        users = realloc(users, sizeof(user_t*) * max_users);
    }
    users[num_users++] = user;

    return user;
}

void paired_up(user_t *user) {
    for (int i = 0; i < num_users; i++) {
        user_t *other = users[i];
        if (other != user && other->state == STARTED && other->other == NOT_PAIRED) {
            user->other = i;
            other->other = user->id;
            user->state = other->state = PLAYING;
            break;
        }
    }
}

void reset_user(user_t *user) {
    if (user->other == NOT_PAIRED) {
        return;
    }
    if (user->state == SUCCEED) {
        user->round++;
    }
    if (user->state == QUITED) {
        user->other = NOT_PAIRED;
        user->round = 1;
    }
    for (int i = 0; i < user->num_keywords; i++) {
        bzero(user->keywords[i], MAX_INPUT_LEN + 1);
    }
    user->num_keywords = 0;
}

void add_name_to_user(user_t *user, char *name) {
    bzero(user->name, MAX_INPUT_LEN + 1);
    strncpy(user->name, name, MAX_INPUT_LEN);
}

void add_keyword_to_user(user_t *user, char *keyword) {
    char *thekeyword = user->keywords[user->num_keywords++];
    bzero(thekeyword, MAX_INPUT_LEN + 1);
    strncpy(thekeyword, keyword, MAX_INPUT_LEN);
}

void free_users() {
    for (int i = 0; i < num_users; i++) {
        free(users[i]);
    }
    free(users);
}
