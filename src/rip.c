#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>
#include <libgen.h>
#include <pthread.h>

#include "giles.h"
#include "rip.h"
#include "misc.h"

#define directory "~/music/test"

struct rip_tracks_from_disc_thread_arguments {
    GtkWidget *progress_bar;
    int track_count_on_disc;
    const char *disc_title;
    const char *disc_artist;
    int *tracks;
    char **track_titles;
    int num_tracks;
};

static void *rip_tracks_from_disc_thread_func(void *data);

/**
 * A function that launches a thread that rips the requested tracks from the
 * disc.  Note that the function will launch the thread but will not wait for it
 * to finish before returning.
 *
 * @param progress_bar a GtkProgressBar that the ripping thread should update as
 *      it rips
 * @param track_count_on_disc the total number of tracks on the disc (not the
 *      number that the user has selected for ripping)
 * @param disc_title the title of the disc
 * @param disc_artist the artist of the disc
 * @param tracks an array of track numbers to rip, where the first track of the
 *      disc is track 0
 * @param track_titles an array of track titles corresponding to the tracks
 *      array
 * @param num_tracks the total number of tracks to rip (length of tracks and
 *      track_titles arrays)
 */
void rip_tracks_from_disc_thread(GtkWidget *progress_bar, int track_count_on_disc, const char *disc_title, const char *disc_artist, int *tracks, const char **track_titles, int num_tracks) {
    struct rip_tracks_from_disc_thread_arguments *args = malloc(sizeof(struct rip_tracks_from_disc_thread_arguments));
    args->progress_bar = progress_bar;
    args->track_count_on_disc = track_count_on_disc;
    args->disc_title = disc_title;
    args->disc_artist = disc_artist;
    args->tracks = tracks;
    args->track_titles = track_titles;
    args->num_tracks = num_tracks;

    pthread_t ripping_thread;

    pthread_create(&ripping_thread, NULL, rip_tracks_from_disc_thread_func, args);
}

/**
 * An entry function for a thread that rips tracks from a disc.
 *
 * @param data a pointer to a struct rip_tracks_from_disc_thread_arguments that
 *      contains the parameters for the thread
 *
 * @return NULL
 */
static void *rip_tracks_from_disc_thread_func(void *data) {
    struct rip_tracks_from_disc_thread_arguments *args = (struct rip_tracks_from_disc_thread_arguments *) data;
    GtkWidget *progress_bar = args->progress_bar;
    int track_count = args->track_count_on_disc;
    const char *disc_title = args->disc_title;
    const char *artist = args->disc_artist;
    int *tracks = args->tracks;
    int num_tracks = args->num_tracks;
    char **track_titles = args->track_titles;
    double frac_completed = 0.0;
    char *progress_bar_text = malloc(BUFSIZ);
    char *wav_filename_format;

    int track_count_width = 0;
    char *expanded_directory;

    int i;

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

    gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(progress_bar), frac_completed);
    snprintf(progress_bar_text, BUFSIZ, "0 of %d tracks ripped", num_tracks);
    gtk_progress_bar_set_text(GTK_PROGRESS_BAR(progress_bar), progress_bar_text);

    for (i = 0; i < num_tracks; i++) {
        int track_num = tracks[i] + 1; // 0-indexed in the array
        char *track_num_str;
        char *dirname_str;
        char *wav_filename;
        pid_t child_pid;

        asprintf(&track_num_str, "%d", track_num);

        asprintf(&wav_filename, wav_filename_format, artist, disc_title, track_num, track_titles[i]);

        dirname_str = strdup(wav_filename);
        dirname(dirname_str);
        mkdir_p(dirname_str);

        child_pid = fork();

        if (child_pid < 0) {
            perror("giles: Failed to fork to execute cdparanoia");
            return NULL;
        } else if (child_pid == 0) {
            execlp("cdparanoia", "cdparanoia", track_num_str, wav_filename, NULL);
            perror("giles: Failed to execute cdparanoia");
            return NULL;
        } else {
            waitpid(child_pid, NULL, 0);
        }

        frac_completed = (double) (i+1) / (double) num_tracks;
        DPRINTF ("Fraction of work completed: %f\n", frac_completed);
        gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(progress_bar), frac_completed);
        snprintf(progress_bar_text, BUFSIZ, "%d of %d tracks ripped", i+1, num_tracks);
        gtk_progress_bar_set_text(GTK_PROGRESS_BAR(progress_bar), progress_bar_text);

        free(track_num_str);
        free(wav_filename);
    }

    free(wav_filename_format);

    return NULL;
}
