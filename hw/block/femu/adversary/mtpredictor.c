/*
 * mtpredictor.c
 * Fredrik Pihlqvist  2020-03-XX
 * 
 * Mereseen Twister Predictor "class" file
 */

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

void mtp_feed(MTPredictor *mtp, u32 *sample)
{
    for (unsigned int i = 0; i < 624; i++)
        mtp->state[i] = _untempering(sample[i]);
    mtp->index = 0;
}

u32 mtp_get_next(MTPredictor *mtp)
{
    _mtp_generate(mtp->state, mtp->index);
    u32 r = _tempering(mtp->state[mtp->index]);
    mtp->index = (mtp->index + 1) % 624;
    return r;
}

// TODO:
int mtp_predict_buffer(MTPredictor *mtp, void *buffer, unsigned long adr, unsigned long len)
{
    // I want to know if the length can be divided into 32 bit numbers
    assert(len % 4 == 0);
    unsigned char *buf = buffer;

    for (unsigned long i = 0; i < len/4; i++) {
        u32 r = mtp_get_next(mtp);
        u32tobuffer(r, &buf[i*4]);
    }
}