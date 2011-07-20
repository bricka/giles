#ifndef INCLUDE_RIP_H
#define INCLUDE_RIP_H

#include <gtk/gtk.h>
#include <pthread.h>

#include "encode.h"

void rip_tracks_from_disc_thread(GtkWidget *progress_bar, int track_count_on_disc, const char *disc_title, const char *disc_artist, int *tracks, const char **track_titles, int num_tracks, struct wav_to_encode **wav_list, pthread_mutex_t *wav_list_mutex, pthread_cond_t *new_wav_cond);

#endif
