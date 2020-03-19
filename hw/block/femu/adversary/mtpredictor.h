/*
 * mtpredictor.h
 * Fredrik Pihlqvist  2020-03-XX
 * 
 * Mereseen Twister Predictor "class" file
 */

#ifndef _MT_PREDICTOR_H_
#define _MT_PREDICTOR_H_

#define MATRIX_A 0x9908b0df
#define UPPER_MASK 0x80000000
#define LOWER_MASK 0x7fffffff

typedef unsigned int u32;
typedef unsigned char u8;

typedef struct MTPredictor {
    u32 first[624]; // First 624 numbers
    u32 state[624]; // MT state 
    u32 index;
    unsigned long curadr; // Current address to state.
} MTPredictor;

static inline u32 _tempering(u32 y)
{
    y ^= (y >> 11);
    y ^= (y <<  7) & 0x9d2c5680;
    y ^= (y << 15) & 0xefc60000;
    y ^= (y >> 18);
    return y;
}

static inline u32 _untempering(u32 y)
{
    y ^= (y >> 18);
    y ^= (y << 15) & 0xefc60000;
    y ^= ((y <<  7) & 0x9d2c5680) ^ ((y << 14) & 0x94284000) ^ ((y << 21) & 0x14200000) ^ ((y << 28) & 0x10000000);
    y ^= (y >> 11) ^ (y >> 22);
    return y;
}

u32 buffertou32(u8 *buffer);
int u32tobuffer(u32 r, u8 *buffer);

void _mtp_generate(u32 *mt, u32 kk);
int mtp_feed(MTPredictor *mtp, u8 *buffer, unsigned long len, unsigned long adr);
u32 mtp_get_next(MTPredictor *mtp);
int mtp_predict_buffer(MTPredictor *mtp, void *buffer, unsigned long adr, unsigned long len);
int mtp_move_state(MTPredictor *mtp, unsigned long adr);

#endif /* _MT_PREDICTOR_H_ */