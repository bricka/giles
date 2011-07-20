#ifndef INCLUDE_ENCODE_H
#define INCLUDE_ENCODE_H

#include <gtk/gtk.h>
#include <pthread.h>
#include <limits.h>

struct wav_to_encode {
    int done;
    char wav_filename[PATH_MAX];
    char artist[BUFSIZ];
    char album[BUFSIZ];
    char track_num[5];
    char track_title[BUFSIZ];
};

void encode_tracks_thread(GtkWidget *progress_bar, int num_tracks, struct wav_to_encode **wav_list, pthread_mutex_t *wav_list_mutex, pthread_cond_t *new_wav_cond);

#endif
