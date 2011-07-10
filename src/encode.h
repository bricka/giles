#ifndef INCLUDE_ENCODE_H
#define INCLUDE_ENCODE_H

#include <gtk/gtk.h>

void encode_tracks_from_disc_thread(GtkWidget *progress_bar, int track_count_on_disc, const char *disc_title, const char *disc_artist, int *tracks, const char **track_titles, const char **wav_filenames, int num_tracks);

#endif
