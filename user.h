#ifndef USER_H
#define USER_H

#include <stdbool.h>

#define MIN_USERS 2
#define MAX_KEYWORDS 20
#define MAX_INPUT_LEN 20

typedef enum {
	NOT_PAIRED = -1,
	STARTED,
	SUCCEED,
	QUITED
} STATE;

typedef struct {
	// id of the other player
	int other;
	int id;
	int round;
	STATE state;
	char name[MAX_INPUT_LEN + 1];
	char keywords[MAX_KEYWORDS][MAX_INPUT_LEN + 1];
	int num_keywords;
} user_t;

extern user_t** users;
extern int num_users;

user_t *add_new_user();
void paired_up(user_t *user);
void reset_user(user_t *user);
void add_name_to_user(user_t *user, char *name);
void add_keyword_to_user(user_t *user, char *keyword);
void free_users();

#endif
