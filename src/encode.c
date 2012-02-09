#include <errno.h>
#include <libgen.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include "encode.h"
#include "giles.h"
#include "misc.h"

#define directory "~/music/test"

struct encode_tracks_thread_arguments {
    GtkWidget *progress_bar;
    int num_tracks;
    struct wav_to_encode **wav_list;
    pthread_mutex_t *wav_list_mutex;
    pthread_cond_t *new_wav_cond;
};

static void *encode_tracks_thread_func(void *data);

/**
 * A function that launches a thread that encodes the requested tracks from the
 * disc.  Note that the function will launch the thread but will not wait for it
 * to finish before returning.
 *
 * @param progress_bar a GtkProgressBar that the encoding thread should update
 *      as it encodes
 * @param num_tracks the total number of tracks on the disc
 * @param wav_list a list that WAV files ready for encoding will be stored in
 * @param wav_list_mutex a mutex that must be locked before accessing wav_list
 * @param new_wav_cond a cond that will be signalled when a new WAV is ready
 */
void encode_tracks_thread(GtkWidget *progress_bar, int num_tracks, struct wav_to_encode **wav_list, pthread_mutex_t *wav_list_mutex, pthread_cond_t *new_wav_cond) {
    struct encode_tracks_thread_arguments *args = malloc(sizeof(struct encode_tracks_thread_arguments));
    args->progress_bar = progress_bar;
    args->num_tracks = num_tracks;
    args->wav_list = wav_list;
    args->wav_list_mutex = wav_list_mutex;
    args->new_wav_cond = new_wav_cond;

    pthread_t encoding_thread;
    pthread_attr_t encoding_thread_attrs;

    pthread_attr_init(&encoding_thread_attrs);
    pthread_attr_setdetachstate(&encoding_thread_attrs, PTHREAD_CREATE_DETACHED);

    pthread_create(&encoding_thread, &encoding_thread_attrs, encode_tracks_thread_func, args);
}

/**
 * An entry function for a thread that encodes tracks from a disc.
 *
 * @param data a pointer to a struct encode_tracks_thread_arguments that
 *      contains the parameters for the thread
 *
 * @return NULL
 */
static void *encode_tracks_thread_func(void *data) {
    struct encode_tracks_thread_arguments *args = (struct encode_tracks_thread_arguments *) data;
    GtkWidget *progress_bar = args->progress_bar;
    int num_tracks = args->num_tracks;
    struct wav_to_encode **wav_list = args->wav_list;
    pthread_mutex_t *wav_list_mutex = args->wav_list_mutex;
    pthread_cond_t *new_wav_cond = args->new_wav_cond;
    double frac_completed = 0.0;
    char *progress_bar_text = malloc(BUFSIZ);
    char mp3_filename_format[PATH_MAX];
    char mp3_filename[PATH_MAX];

    char expanded_directory[PATH_MAX];

    if ((directory[0] == '~') && (directory[1] == '/')) {
        snprintf(expanded_directory, PATH_MAX, "%s/%s", getenv("HOME"), directory + 1);
    } else {
        strcpy(expanded_directory, directory);
    }

    int i;

    snprintf(mp3_filename_format, PATH_MAX, "%s/%%s/%%s/%%s - %%s.mp3", expanded_directory);

    gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(progress_bar), frac_completed);
    snprintf(progress_bar_text, BUFSIZ, "0 of %d tracks encoded", num_tracks);
    gtk_progress_bar_set_text(GTK_PROGRESS_BAR(progress_bar), progress_bar_text);

    struct wav_to_encode *encode_me;

    for (i = 0; ; i++) {
        pthread_mutex_lock(wav_list_mutex);

        if (wav_list[i] == NULL) {
            pthread_cond_wait(new_wav_cond, wav_list_mutex);
        }

        encode_me = wav_list[i];

        pthread_mutex_unlock(wav_list_mutex);

        if (encode_me->done) {
            free(encode_me);
            break;
        }

        char *artist_no_slash = replace_all(encode_me->artist, '/', '-');
        char *album_no_slash = replace_all(encode_me->album, '/', '-');
        char *track_num_no_slash = replace_all(encode_me->track_num, '/', '-');
        char *track_title_no_slash = replace_all(encode_me->track_title, '/', '-');

        char *dirname_str;
        pid_t child_pid;

        snprintf(mp3_filename, PATH_MAX, mp3_filename_format, artist_no_slash, album_no_slash, track_num_no_slash, track_title_no_slash);

        dirname_str = strdup(mp3_filename);
        dirname(dirname_str);
        mkdir_p(dirname_str);

        child_pid = fork();

        if (child_pid < 0) {
            perror("giles: Failed to fork to execute lame");
            return NULL;
        } else if (child_pid == 0) {
            execlp("lame", "lame", "--tt", encode_me->track_title, "--ta", encode_me->artist, "--tl", encode_me->album, "--tn", encode_me->track_num, encode_me->wav_filename, mp3_filename, NULL);
            perror("giles: Failed to execute lame");
            return NULL;
        } else {
            waitpid(child_pid, NULL, 0);
        }

        /* Delete the source WAV file */
        if (unlink(encode_me->wav_filename) != 0) {
            fprintf(stderr, "giles: Failed to delete source file %s: %s\n", encode_me->wav_filename, strerror(errno));
        }

        /* Update progress */
        frac_completed = (double) (i+1) / (double) num_tracks;
        DPRINTF ("Fraction of work completed: %f\n", frac_completed);
        gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(progress_bar), frac_completed);
        snprintf(progress_bar_text, BUFSIZ, "%d of %d tracks encoded", i+1, num_tracks);
        gtk_progress_bar_set_text(GTK_PROGRESS_BAR(progress_bar), progress_bar_text);

        free(encode_me);
        free(artist_no_slash);
        free(album_no_slash);
        free(track_num_no_slash);
        free(track_title_no_slash);
    }

    return NULL;
}
