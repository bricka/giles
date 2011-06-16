#include <pthread.h>
#include <gtk/gtk.h>
#include <cddb/cddb.h>
#include <stdio.h>
#include <stdlib.h>

#include "ui.h"

static GtkWidget *loading_screen = NULL;
static GtkWidget *disc_title_entry = NULL;
static GtkWidget *disc_artist_entry = NULL;
static pthread_t loading_screen_thread;
static GtkWidget **track_title_entries = NULL;

static void *loading_screen_thread_func(void *);

/**
 * Display the UI loading screen.  Note that this simply displays the screen and
 * returns.
 */
void ui_loading_screen(void) {
    GtkWidget *content_area, *label;

    loading_screen = gtk_dialog_new();
    gtk_window_set_title(GTK_WINDOW(loading_screen), "Loading CD information");

    content_area = gtk_dialog_get_content_area(GTK_DIALOG(loading_screen));

    label = gtk_label_new("Scanning the CD and looking it up in CDDB.");

    gtk_container_add(GTK_CONTAINER(content_area), label);
    gtk_window_set_position(GTK_WINDOW(loading_screen), GTK_WIN_POS_CENTER);
    gtk_widget_show_all(loading_screen);

    pthread_create(&loading_screen_thread, NULL, loading_screen_thread_func, NULL);
}

/**
 * Gets rid of the UI loading screen.
 */
void ui_loading_screen_done(void) {
    gtk_main_quit();
    pthread_join(loading_screen_thread, NULL);
    gtk_widget_destroy(loading_screen);
}

/**
 * Displays the disc information in the given CDDB disc object.
 *
 * @param disc the disc information to display
 */
void ui_show_disc_info(const cddb_disc_t *disc) {
    GtkWidget *disc_info_window, *main_vbox, *disc_info_grid, *disc_title_label, *disc_artist_label, *track_info_frame, *track_info_scrollable, *track_info_grid, *track_title_label;
    int i, track_count, track_count_width;
    char *track_title_label_text_format;
    char *track_title_label_text;

    track_count = cddb_disc_get_track_count(disc);

    disc_info_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(disc_info_window), "Disc Info");
    gtk_container_set_border_width(GTK_CONTAINER(disc_info_window), 10);

    main_vbox = gtk_vbox_new(TRUE, 0);
    gtk_box_set_homogeneous(GTK_BOX(main_vbox), FALSE);
    gtk_container_add(GTK_CONTAINER(disc_info_window), main_vbox);

    disc_info_grid = gtk_grid_new();
    gtk_box_pack_start(GTK_BOX(main_vbox), disc_info_grid, FALSE, FALSE, 0);

    disc_title_label = gtk_label_new("Disc Title:");
    gtk_grid_attach(GTK_GRID(disc_info_grid), disc_title_label, 0, 0, 1, 1);

    disc_title_entry = gtk_entry_new();
    gtk_grid_attach_next_to(GTK_GRID(disc_info_grid), disc_title_entry, disc_title_label, GTK_POS_RIGHT, 1, 1);

    disc_artist_label = gtk_label_new("Disc Artist:");
    gtk_grid_attach(GTK_GRID(disc_info_grid), disc_artist_label, 0, 1, 1, 1);

    disc_artist_entry = gtk_entry_new();
    gtk_grid_attach_next_to(GTK_GRID(disc_info_grid), disc_artist_entry, disc_artist_label, GTK_POS_RIGHT, 1, 1);

    track_info_frame = gtk_frame_new("Track Information");
    gtk_box_pack_start(GTK_BOX(main_vbox), track_info_frame, TRUE, TRUE, 0);

    track_info_scrollable = gtk_scrolled_window_new(NULL, NULL);
    gtk_container_add(GTK_CONTAINER(track_info_frame), track_info_scrollable);

    track_info_grid = gtk_grid_new();
    gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(track_info_scrollable), track_info_grid);

    if (track_count < 10) {
        track_count_width = 1;
    } else if (track_count < 100) {
        track_count_width = 2;
    } else {
        track_count_width = 3;
    }

    asprintf(&track_title_label_text_format, "Track %%%dd:", track_count_width);

    track_title_entries = malloc(track_count * sizeof(GtkWidget *));

    for (i = 0; i < track_count; i++) {
        asprintf(&track_title_label_text, track_title_label_text_format, i);
        track_title_label = gtk_label_new(track_title_label_text);
        gtk_grid_attach(GTK_GRID(track_info_grid), track_title_label, 0, i, 1, 1);
        track_title_entries[i] = gtk_entry_new();
        gtk_grid_attach_next_to(GTK_GRID(track_info_grid), track_title_entries[i], track_title_label, GTK_POS_RIGHT, 1, 1);
    }

    free(track_title_label_text_format);

    gtk_widget_show_all(disc_info_window);
}

/**
 * The function run by the loading screen thread.
 *
 * @param arg IGNORED
 * 
 * @return NULL
 */
static void *loading_screen_thread_func(void *arg) {
    gtk_main();

    return NULL;
}
