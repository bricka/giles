#ifndef INCLUDE_RIP_H
#define INCLUDE_RIP_H

#include <cddb/cddb.h>

void rip_track_from_disc(const cddb_disc_t *disc, cddb_track_t *track);

#endif
