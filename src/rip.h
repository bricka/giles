#ifndef INCLUDE_RIP_H
#define INCLUDE_RIP_H

#include <gtk/gtk.h>
#include <cddb/cddb.h>

void rip_tracks_from_disc_thread(GtkWidget *progress_bar, const cddb_disc_t *disc, int *tracks, int num_tracks);

#endif
