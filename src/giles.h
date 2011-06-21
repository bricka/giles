#ifndef INCLUDE_GILES_H
#define INCLUDE_GILES_H

#ifdef DEBUG
#define DPRINTF(args...) (fprintf(stderr, args))
#else
#define DPRINTF(args...)
#endif

#endif
