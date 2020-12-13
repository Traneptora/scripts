#include <stdlib.h>
#include <stdio.h>
#include <tbz_shell_escape.h>

int main(int argc, char *const argv[]){
    printf("Playing %s...\n", argv[0]);
    char *command = tbz_escape_argv("mpv", argc, argv);
    return system(command);
}

