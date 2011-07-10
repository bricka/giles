#ifndef INCLUDE_RIP_H
#define INCLUDE_RIP_H

#include <gtk/gtk.h>

void rip_tracks_from_disc_thread(GtkWidget *progress_bar, int track_count_on_disc, const char *disc_title, const char *disc_artist, int *tracks, const char **track_titles, int num_tracks);

#endif
