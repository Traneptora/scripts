#include <string.h>
#include <stdlib.h>
#include "tbz_shell_escape.h"

char *tbz_shell_escape(const char *arg){
	size_t pos = 0, size = 3, offset;
	while (arg[pos] != '\0'){
		if (arg[pos] == '\'')
			size += 4;
		else
			++size;

		++pos;
	}
	
	char *ret = (char *) malloc(size * sizeof(char));
	*ret = '\'';

	pos = 0;
	offset = 1;
	
	while (arg[pos] != '\0'){
		if (arg[pos] == '\''){
			ret[pos + offset] = '\'';
			ret[pos + offset + 1] = '\\';
			ret[pos + offset + 2] = '\'';
			ret[pos + offset + 3] = '\'';
			offset += 3;
		} else {
			ret[pos + offset] = arg[pos];
		}
		++pos;
	}

	ret[pos + offset] = '\'';
	ret[pos + offset + 1] = '\0';
	return ret;
}

char *tbz_escape_argv(const char *argv0, int argc, char *const argv[]){
	
	char **escaped = (char **) malloc(argc * sizeof (char *));
	size_t *lengths = (size_t *) malloc(argc * sizeof(size_t));
	size_t totallen = 1;

	for (int i = 0; i < argc; ++i){
		escaped[i] = tbz_shell_escape(i == 0 ? argv0 : argv[i]);
		lengths[i] = strlen(escaped[i]);
		totallen += lengths[i] + 1;
	}

	char *ret = (char *) malloc(totallen * sizeof(char));

	totallen = 0;

	for (int i = 0; i < argc; ++i){
		memcpy(ret + totallen, escaped[i], lengths[i]);
		free(escaped[i]);
		ret[totallen + lengths[i]] = ' ';
		totallen += lengths[i] + 1;
	}

	ret[totallen] = '\0';

	free(escaped);
	free(lengths);

	return ret;

}
