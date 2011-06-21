#include "giles.h"
#include "misc.h"

#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <stdio.h>

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
