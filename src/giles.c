#include <cdio/cdio.h>
#include <cddb/cddb.h>

#include "giles.h"
#include "ui.h"
#include "cddb.h"

int main(int argc, char **argv) {
    gtk_init(&argc, &argv);
    g_thread_init(NULL);

#ifdef LIB_DEBUG
    cddb_log_set_level(CDDB_LOG_DEBUG);
#endif

    CdIo_t *p_cdio = cdio_open(NULL, DRIVER_DEVICE);

    if (p_cdio == NULL) {
        fputs("Unable to read the CD drive!\n", stderr);
        return 1;
    }

    ui_loading_screen();

    cddb_disc_t *disc = cddb_disc_new();
    
    enum giles_cddb_err get_cddb_info_err = get_cddb_info_for_device(p_cdio, disc);

    switch (get_cddb_info_err) {
        case giles_cddb_err_no_conn:
            fputs("Could not connect to CDDB database.\n", stderr);
            break;
        case giles_cddb_err_no_match:
            fputs("Could not find a match for the CD in the CDDB database.\n", stderr);
            break;
        default:
            break;
    }

    cdio_destroy(p_cdio);

    ui_loading_screen_done();

    ui_show_disc_info(disc);

    gtk_main();

    return 0;
}
