#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <regex.h>

#include "preg_match_all.h"

char *getsubstr(char *s, regmatch_t *pmatch)
{
	char *buff = malloc(sizeof(char) * 1024);
	bzero(buff, 1024);
	memcpy(buff, s + pmatch->rm_so, pmatch->rm_eo - pmatch->rm_so);
	return buff;
}

void preg_match_all(char *pattern, char *buffer, char **matches)
{
	regmatch_t pmatch;
	regex_t reg;
	regcomp(&reg, pattern, REG_EXTENDED);

	int offset = 0;
	int matches_found = 0;
	int matches_len = 1;
	while(offset < strlen(buffer))
	{
		int status = regexec(&reg, buffer + offset, 1, &pmatch, 0);
		if(status != REG_NOMATCH && pmatch.rm_so != -1)
		{
			if (matches_found >= matches_len)
			{
				matches_len *= 2;
				matches = realloc(matches, sizeof(char*)*matches_len);
			}
			char *match = getsubstr(buffer + offset, &pmatch);
			matches[matches_found++] = match;
		}
		offset += pmatch.rm_eo;
	}
	regfree(&reg);
}
