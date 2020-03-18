/*
 * adversary.h
 * Fredrik Pihlqvist  2020-03-XX
 * 
 * Adversary header file containing structure and methods used by an adversary.
 */

#ifndef _ADVERSARY_H_
#define _ADVERSARY_H_

#include <stdint.h>

enum AdversaryMethod {
    ADVERSARY_ZERO_FILL = 0,
    ADVERSARY_STATIC_FILL,  // Fill the eniter device with a static pattern. Example write 'a' to every byte in memory
    ADVERSARY_PRNG_MT,
    _NUMBER_OF_VALUES,
};

typedef union AdversaryMethodData {
    struct {
        int set;
        unsigned char pattern;
    } sf;
} AdversaryMethodData;

typedef struct Adversary {
    enum AdversaryMethod  method;
    uint16_t              ON;
    void                  *buffer;
    unsigned long         size;
    AdversaryMethodData   data;
} Adversary;

void adversary_init(Adversary *adv, unsigned long nbytes);
void adversary_destroy(Adversary *adv);
void adversary_init_data(Adversary *adv);
void adversary_feed(Adversary *adv, unsigned long adr, unsigned long len);
void adversary_predict(Adversary *adv, unsigned long adr, unsigned long len);
void adversary_toggle(Adversary *adv);
int adversary_set_method(Adversary *adv, long method);

#endif /* _ADVERSARY_H_ */