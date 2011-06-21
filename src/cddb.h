#ifndef INCLUDE_CDDB_H
#define INCLUDE_CDDB_H

#include <cdio/cdio.h>
#include <cddb/cddb.h>

enum giles_cddb_err {
    giles_cddb_err_none,
    giles_cddb_err_no_conn,
    giles_cddb_err_no_match
};

enum giles_cddb_err get_cddb_info_for_device(const CdIo_t *p_cdio, cddb_disc_t *disc);

#endif
