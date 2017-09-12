#ifndef TBZ_SHELL_ESCAPE_H_
#define TBZ_SHELL_ESCAPE_H_

char *tbz_shell_escape(const char *arg);
char *tbz_escape_argv(const char *argv0, int argc, char *const argv[]);

#endif
