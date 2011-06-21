#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>
#include <libgen.h>

#include "giles.h"
#include "rip.h"
#include "misc.h"

#define directory "~/music/test"

void rip_track_from_disc(const cddb_disc_t *disc, cddb_track_t *track) {
    const char *disc_title = cddb_disc_get_title(disc);
    const char *artist = cddb_track_get_artist(track);
    const char *track_title = cddb_track_get_title(track);
    int track_num = cddb_track_get_number(track);
    int track_count = cddb_disc_get_track_count(disc);
    int track_count_width = 0;
    char *track_num_str;
    char *expanded_directory;
    char *dirname_str;
    char *wav_filename_format;
    char *wav_filename;
    pid_t child_pid;

    asprintf(&track_num_str, "%d", track_num);

    if (track_count < 10) {
        track_count_width = 1;
    } else if (track_count < 100) {
        track_count_width = 2;
    } else {
        track_count_width = 3;
    }

    if ((directory[0] == '~') && (directory[1] == '/')) {
        asprintf(&expanded_directory, "%s/%s", getenv("HOME"), directory + 1);
    } else {
        expanded_directory = directory;
    }

    asprintf(&wav_filename_format, "%s/%%s/%%s/%%0%dd - %%s.wav", expanded_directory, track_count_width);
    asprintf(&wav_filename, wav_filename_format, artist, disc_title, track_num, track_title);

    dirname_str = strdup(wav_filename);
    dirname(dirname_str);
    mkdir_p(dirname_str);

    child_pid = fork();

    if (child_pid < 0) {
        perror("giles: Failed to fork to execute cdparanoia");
    } else if (child_pid == 0) {
        execlp("cdparanoia", "cdparanoia", track_num_str, wav_filename, NULL);
        perror("giles: Failed to execute cdparanoia");
    } else {
        waitpid(child_pid, NULL, 0);
    }
}
