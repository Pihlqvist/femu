/*
 * adversary.h
 * Fredrik Pihlqvist  2020-03-XX
 * 
 * Adversary header file containing structure and methods used by an adversary.
 */

#ifndef _ADVERSARY_H_
#define _ADVERSARY_H_

#include <stdint.h>
#include "mtpredictor.h"

enum AdversaryMethod {
    ADVERSARY_ZERO_FILL = 0,
    ADVERSARY_STATIC_FILL,
    ADVERSARY_PRNG_MT,
    _NUMBER_OF_VALUES,
};

enum MTStatus {
    NOT_STARTED = 0,
    STARTED,
    DONE,
};

typedef union AdversaryMethodData {
    struct {
        int set;
        unsigned char pattern;
    } sf; /* ADVERSARY_ZERO_FILL */
    struct {
        enum MTStatus status;   // TODO: Could probably replace with a bool
        MTPredictor mtp;
        unsigned long sadr;
        unsigned int sample[624];   // TODO: Is it needed?
        unsigned char buf[4];
    } mt; /* ADVERSARY_PRNG_MT */
} AdversaryMethodData;

typedef struct Adversary {
    enum AdversaryMethod  method;
    uint16_t              ON;
    void                  *buffer;
    unsigned long         size;
    AdversaryMethodData   data;
    uint16_t              debug;
} Adversary;

void adversary_init(Adversary *adv, unsigned long nbytes);
void adversary_destroy(Adversary *adv);
void adversary_init_data(Adversary *adv);
void adversary_feed(Adversary *adv, unsigned long adr, unsigned long len);
void adversary_predict(Adversary *adv, unsigned long adr, unsigned long len);
void adversary_toggle(Adversary *adv);
void adversary_toggle_debug(Adversary *adv);
int adversary_set_method(Adversary *adv, long method);

#endif /* _ADVERSARY_H_ */