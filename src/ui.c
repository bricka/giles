#include <pthread.h>
#include <gtk/gtk.h>
#include <cddb/cddb.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "giles.h"
#include "ui.h"
#include "misc.h"
#include "rip.h"
#include "encode.h"

static void handle_rip_button_clicked(GtkButton *button, cddb_disc_t *disc);

static GtkWidget *loading_screen = NULL;
static GtkWidget *disc_title_entry = NULL;
static GtkWidget *disc_artist_entry = NULL;
static pthread_t loading_screen_thread;
static GtkWidget *disc_info_window = NULL;
static GtkWidget **track_title_entries = NULL;
static GtkWidget **track_do_rip_check_buttons = NULL;

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
    gtk_window_set_modal(GTK_WINDOW(loading_screen), 1);
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
    GtkWidget *main_vbox, *disc_info_grid, *disc_title_label,
              *disc_artist_label, *track_info_frame, *track_info_scrollable,
              *track_info_grid, *track_title_label, *button_box, *rip_button;
    int i, track_count, track_count_width;
    cddb_track_t *track;
    char *track_title_label_text_format;
    char *track_title_label_text;
    const char *track_title;

    track_count = cddb_disc_get_track_count(disc);

    /* the overall window */

    disc_info_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(disc_info_window), "Disc Info");
    gtk_container_set_border_width(GTK_CONTAINER(disc_info_window), 10);
    g_signal_connect(disc_info_window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    main_vbox = gtk_vbox_new(TRUE, 0);
    gtk_box_set_homogeneous(GTK_BOX(main_vbox), FALSE);
    gtk_container_add(GTK_CONTAINER(disc_info_window), main_vbox);

    /* all-disc info */

    disc_info_grid = gtk_grid_new();
    gtk_box_pack_start(GTK_BOX(main_vbox), disc_info_grid, FALSE, FALSE, 0);

    disc_title_label = gtk_label_new("Disc Title:");
    gtk_grid_attach(GTK_GRID(disc_info_grid), disc_title_label, 0, 0, 1, 1);

    disc_title_entry = gtk_entry_new();
    if (cddb_disc_get_title(disc) != NULL) {
        gtk_entry_set_text(GTK_ENTRY(disc_title_entry), cddb_disc_get_title(disc));
    }
    gtk_grid_attach_next_to(GTK_GRID(disc_info_grid), disc_title_entry, disc_title_label, GTK_POS_RIGHT, 1, 1);

    disc_artist_label = gtk_label_new("Disc Artist:");
    gtk_grid_attach(GTK_GRID(disc_info_grid), disc_artist_label, 0, 1, 1, 1);

    disc_artist_entry = gtk_entry_new();
    if (cddb_disc_get_artist(disc) != NULL) {
        gtk_entry_set_text(GTK_ENTRY(disc_artist_entry), cddb_disc_get_artist(disc));
    }
    gtk_grid_attach_next_to(GTK_GRID(disc_info_grid), disc_artist_entry, disc_artist_label, GTK_POS_RIGHT, 1, 1);

    /* per-track information */

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
    track_do_rip_check_buttons = malloc(track_count * sizeof(GtkWidget *));

    for (i = 0; i < track_count; i++) {
        track = cddb_disc_get_track(disc, i);
        track_title = cddb_track_get_title(track);
        asprintf(&track_title_label_text, track_title_label_text_format, i + 1);
        track_title_label = gtk_label_new(track_title_label_text);
        gtk_grid_attach(GTK_GRID(track_info_grid), track_title_label, 0, i, 1, 1);

        track_title_entries[i] = gtk_entry_new();
        if (track_title != NULL) {
            gtk_entry_set_text(GTK_ENTRY(track_title_entries[i]), track_title);
        }
        gtk_grid_attach_next_to(GTK_GRID(track_info_grid), track_title_entries[i], track_title_label, GTK_POS_RIGHT, 1, 1);

        track_do_rip_check_buttons[i] = gtk_check_button_new();
        gtk_grid_attach_next_to(GTK_GRID(track_info_grid), track_do_rip_check_buttons[i], track_title_entries[i], GTK_POS_RIGHT, 1, 1);
    }

    free(track_title_label_text_format);

    /* buttons at the bottom */

    button_box = gtk_hbutton_box_new();
    gtk_box_pack_start(GTK_BOX(main_vbox), button_box, FALSE, FALSE, 0);

    rip_button = gtk_button_new_with_label("Rip");
    g_signal_connect(rip_button, "clicked", G_CALLBACK(handle_rip_button_clicked), (void *) disc);
    gtk_box_pack_start(GTK_BOX(button_box), rip_button, FALSE, FALSE, 0);

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

/**
 * The function executed when the Rip button is clicked.
 *
 * @param button the button that was clicked
 * @param disc the CDDB disc that we are using
 */
static void handle_rip_button_clicked(GtkButton *button, cddb_disc_t *disc) {
    GtkWidget *ripping_progress_dialog, *content_area, *content_area_vbox, *ripping_progress_bar, *encoding_progress_bar, *check_button;
    int track_count = cddb_disc_get_track_count(disc);
    int *tracks_to_rip = NULL;
    int num_tracks = 0;
    int i;

    ripping_progress_dialog = gtk_dialog_new_with_buttons("Ripping CD", GTK_WINDOW(disc_info_window), GTK_DIALOG_MODAL, GTK_STOCK_CANCEL, NULL);

    content_area = gtk_dialog_get_content_area(GTK_DIALOG(ripping_progress_dialog));

    content_area_vbox = gtk_vbox_new(1, 0);
    gtk_container_add(GTK_CONTAINER(content_area), content_area_vbox);

    ripping_progress_bar = gtk_progress_bar_new();
    gtk_box_pack_start(GTK_BOX(content_area_vbox), ripping_progress_bar, 0, 0, 0);
    gtk_progress_bar_set_show_text(GTK_PROGRESS_BAR(ripping_progress_bar), 1);

    encoding_progress_bar = gtk_progress_bar_new();
    gtk_box_pack_start(GTK_BOX(content_area_vbox), encoding_progress_bar, 0, 0, 0);
    gtk_progress_bar_set_show_text(GTK_PROGRESS_BAR(encoding_progress_bar), 1);

    gtk_widget_show_all(ripping_progress_dialog);

    tracks_to_rip = malloc(track_count * sizeof(int));

    for (i = 0; i < track_count; i++) {
        check_button = track_do_rip_check_buttons[i];
        if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(check_button))) {
            tracks_to_rip[num_tracks] = i;
            num_tracks++;
        }
    }

    if (num_tracks > 0) {
        const char *disc_title = gtk_entry_get_text(GTK_ENTRY(disc_title_entry));
        const char *disc_artist = gtk_entry_get_text(GTK_ENTRY(disc_artist_entry));

        const char **track_titles = malloc(num_tracks * sizeof(char *));
        for (i = 0; i < num_tracks; i++) {
            track_titles[i] = gtk_entry_get_text(GTK_ENTRY(track_title_entries[tracks_to_rip[i]]));
        }

        struct wav_to_encode **wav_list = calloc(num_tracks + 1, sizeof(struct wav_to_encode *));

        pthread_mutex_t *wav_list_mutex = malloc(sizeof(pthread_mutex_t));
        pthread_mutex_init(wav_list_mutex, NULL);

        pthread_cond_t *new_wav_cond = malloc(sizeof(pthread_cond_t));
        pthread_cond_init(new_wav_cond, NULL);

        rip_tracks_from_disc_thread(ripping_progress_bar, track_count, disc_title, disc_artist, tracks_to_rip, track_titles, num_tracks, wav_list, wav_list_mutex, new_wav_cond);
        encode_tracks_thread(encoding_progress_bar, num_tracks, wav_list, wav_list_mutex, new_wav_cond);
    }
}
