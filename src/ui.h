#ifndef INCLUDE_UI_H
#define INCLUDE_UI_H

#include <cddb/cddb.h>

void ui_loading_screen(void);
void ui_loading_screen_done(void);
void ui_show_disc_info(const cddb_disc_t *disc);

#endif
