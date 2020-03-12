#ifndef __FEMU_MEM_BACKEND__
#define __FEMU_MEM_BACKEND__

#include <stdint.h>
#include "adversary/adversary.h"


/* Coperd: FEMU memory backend structure */
struct femu_mbe {
    void *mem_backend;
    int64_t size; /* in bytes */
    int femu_mode;
};

void femu_init_mem_backend(struct femu_mbe *mbe, int64_t nbytes);
void femu_destroy_mem_backend(struct femu_mbe *mbe);
int femu_rw_mem_backend_bb(struct femu_mbe *mbe, QEMUSGList *qsg,
        uint64_t data_offset, bool is_write);
int femu_rw_mem_backend_oc(struct femu_mbe *mbe, QEMUSGList *qsg,
        uint64_t *data_offset, bool is_write);
int adversary_rw_mem_backend(Adversary *adv, QEMUSGList *qsg,
        uint64_t *data_offset, bool is_write);

#endif
