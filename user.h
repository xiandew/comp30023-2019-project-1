#ifndef USER_H
#define USER_H

#define MIN_USERS 2
#define MIN_KEYWORD 1

// id: starts from 0.
// other: id of the other user. -1 means the user is not using.
// name: user's name.
// keywords: user's guessed words.
// max_keywords: number of keywords that can be stored in the list.
// num_keywords: number of keywords that have been recorded.
typedef struct {
	int id;
	int other;
	char *name;
	char **keywords;
	int max_keywords;
	int num_keywords;
} user_t;

extern user_t** users;
extern int num_users;

user_t *new_user(int id);
void add_user(user_t *user);
void add_name_to_user(int id, char *name);
void free_users();

#endif
