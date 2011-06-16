#include "giles.h"
#include "cddb.h"

static int query_matches(cddb_disc_t *disc);

/**
 * Given a CDIO device, calculates the necessary information about the device
 * and contacts a CDDB server to get disk information.
 *
 * @param p_cdio a CDIO device
 * @param disk the disk structure to store the information in
 *
 * @return an error status about whether information was obtained
 */
enum giles_cddb_err get_cddb_info_for_device(const CdIo_t *p_cdio, cddb_disc_t *disc) {
    lsn_t last_lsn;
    msf_t last_msf;
    unsigned disc_len_in_s, i, matches;
    track_t num_tracks;
    cddb_track_t *track;
    cddb_conn_t *conn;

    num_tracks = cdio_get_num_tracks(p_cdio);

    DPRINTF ("Disc contains %d tracks\n", num_tracks);

    for (i = 1; i <= num_tracks; i++) {
        track = cddb_track_new();
        
        cddb_track_set_frame_offset(track, cdio_get_track_lba(p_cdio, i));
        DPRINTF ("Offset for track %d is %d\n", i, cdio_get_track_lba(p_cdio, i));
        cddb_disc_add_track(disc, track);
    }

    last_lsn = cdio_get_disc_last_lsn(p_cdio);
    cdio_lsn_to_msf(last_lsn, &last_msf);
    disc_len_in_s = cdio_audio_get_msf_seconds(&last_msf);

    cddb_disc_set_length(disc, disc_len_in_s);

    conn = cddb_new();

    if (conn == NULL) {
        cddb_error_print(cddb_errno(conn));
        return giles_cddb_err_no_conn;
    }

    matches = cddb_query(conn, disc);

    if (matches == -1) {
        return giles_cddb_err_no_conn;
    } else if (matches == 0) {
        return giles_cddb_err_no_match;
    }

    cddb_read(conn, disc);

    cddb_destroy(conn);

    return giles_cddb_err_none;
}
