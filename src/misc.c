#include "giles.h"
#include "misc.h"

#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/**
 * Create the given directory, and all parent directories as necessary.
 *
 * @param directory the directory to create
 */
void mkdir_p(const char *directory) {
    pid_t child_pid = fork();

    if (child_pid == 0) {
        DPRINTF ("Running: mkdir -p %s\n", directory);
        execlp("mkdir", "mkdir", "-p", directory, NULL);
    } else {
        waitpid(child_pid, NULL, 0);
    }
}

/**
 * Returns a new string with all instances of to_replace replaced with replace_with.
 *
 * @param str the string to replace characters in
 * @param to_replace the character to replace
 * @param replace_with the character to replace with
 *
 * @return a new string that is a copy of str with the necessary replacements made
 */
char *replace_all(const char *str, const char to_replace, const char replace_with) {
    char *new_s = malloc(strlen(str) + 1);
    char *s = new_s;

    while (*str++) {
        if (*str == to_replace) {
            *s = replace_with;
        } else {
            *s = *str;
        }

        s++;
    }

    *s = '\0';

    return new_s;
}
