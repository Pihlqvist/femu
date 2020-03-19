/*
 * mtpredictor.c
 * Fredrik Pihlqvist  2020-03-XX
 * 
 * Mereseen Twister Predictor "class" file
 */

#include <stdio.h>
#include <stdlib.h>
#include "mtpredictor.h"

/*
 * Convert buffer of 4 bytes to a u32 number. Little Endien
 */ 
u32 buffertou32(u8 *buffer)
{
    u32 result = 0;

    result = (result | buffer[0]);
    result = (result | (buffer[1] << 8));
    result = (result | (buffer[2] << 16));
    result = (result | (buffer[3] << 24));

    return result;
}

/*
 * Converts 32 bit number to 4 bytes, Little Endien
 */ 
int u32tobuffer(u32 r, u8 *buffer)
{
    int i;
    u8 c;  // single char

    for( i = 0; i < 4; i++ )
    {
        c = r & 0xFFUL;
        r = r >> 8;
        buffer[i] = c;
    }

    return 0;
}

void _mtp_generate(u32 *mt, u32 kk)
{
    u32 mag01[2] = {0x0, MATRIX_A};
    u32 y = (mt[kk] & UPPER_MASK | (mt[(kk + 1) % 624] & LOWER_MASK));
    mt[kk] = mt[(kk + 397) % 624] ^ (y >> 1) ^ mag01[y & 0x1];
}

/*
 * Generate a MT state given a buffer of 624 32 bit numbers.
 */
int mtp_feed(MTPredictor *mtp, u8 *buffer, unsigned long len, unsigned long adr)
{
    if (len < 624*4) {
        fprintf(stderr, "(mtp_feed) buffer to small to get a sample.\n");
        return -1;
    }

    mtp->curadr = adr;

    // Find 624 32 bit numbers and store them untemperd in the state array
    for (unsigned int i = 0; i < 624; i++) {
        u32 smp = buffertou32(&buffer[i*4]);
        mtp->state[i] = _untempering(smp);
    }
    mtp->index = 0;

    // TODO: Should probably confirm that we have succsseded by generating
    // the next number in the sequences and verifying that it's the same 
    // as the next number in the buffer

    return 0;
}

/*
 * generate the next number in the sequences and move the state forward
 */
u32 mtp_get_next(MTPredictor *mtp)
{
    _mtp_generate(mtp->state, mtp->index);
    u32 r = _tempering(mtp->state[mtp->index]);
    mtp->index = (mtp->index + 1) % 624;
    return r;
}

/*
 * Move MT state to a specific address
 */
int mtp_move_state(MTPredictor *mtp, unsigned long adr)
{
    long move = mtp->curadr - adr;

    if (move == 0) {
        // State is already correct
        return 0;
    }

    if (move < 0) {
        // We have past the address

        // Reset MT
        for (unsigned int i = 0; i < 624; i++) {
            mtp->state[i] = _untempering(mtp->first[i]);
        }
        mtp->index = 0;

        move = adr;
    }

    if (move > 0) {
        while (move > 0) {
            mtp_get_next(mtp);
            move--;
        }
    }

    mtp->curadr = adr;
    return 0;
}

/*
 * Fills a buffer with predicted datat generated from MTPredictor
 */ 
int mtp_predict_buffer(MTPredictor *mtp, void *buffer, unsigned long adr,
unsigned long len)
{
    mtp_move_state(mtp, adr);

    unsigned char *buf = buffer;

    for (unsigned long i = 0; i < len/4; i++) {
        u32 r = mtp_get_next(mtp);
        u32tobuffer(r, &buf[i*4]);
    }
}