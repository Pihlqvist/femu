#ifndef _ADVERSARY_H_
#define _ADVERSARY_H_

#include <stdint.h>

enum AdversaryMethod {
    ADVERSARY_ZERO_FILL = 0,
    ADVERSARY_PRNG_MT   = 1,
};

typedef struct Adversary {
    enum AdversaryMethod    method;
    uint16_t                ON;
    void                    *buffer;
    unsigned long           size;
} Adversary;

void adversary_init(Adversary *adv, unsigned long nbytes);
void adversary_destroy(Adversary *adv);
void adversary_feed(Adversary *adv, char *buff);
void adversary_predict(Adversary *adv, unsigned long adr, unsigned long len);
void adversary_toggle(Adversary *adv);

#endif /* _ADVERSARY_H_ */