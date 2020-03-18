/*
 * Adversary methods
 * Fredrik Pihlqvist  2020-03-XX
 */ 

#include "qemu/osdep.h"
#include "hw/block/block.h"
#include "hw/pci/pci.h"
#include "qemu/error-report.h"

#include "adversary.h"

/*
 * Init the adversary structure. Setting the method that the adversery will prevent aswell
 * as the size of the buffer the adversary will use when sitting between the victim and 
 * the actual storage.
 */
void adversary_init(Adversary *adv, unsigned long nbytes)
{
    assert(!adv->buffer);

    adv->method = ADVERSARY_ZERO_FILL;
    adv->ON     = 0;    // Start adversary in 'off' mode
    adv->size   = nbytes;
    adv->buffer = g_malloc(nbytes);
    adversary_init_data(adv);

    if (adv->buffer == NULL)
    {
        error_report("FEMU: cannot allocate %" PRId64 " bytes for emulating Adversary"
                "buffer, make sure you have enough free DRAM in your host\n", nbytes);
        abort();        
    }

    if (mlock(adv->buffer, nbytes) == -1)
    {
        error_report("FEMU: failed to pin %" PRId64 " bytes ...\n", nbytes);
        g_free(adv->buffer);
        abort();
    }
}

/*
 * Free up the allocated memory that Adversary holds.
 */
void adversary_destroy(Adversary *adv)
{
    if (adv->buffer) {
        munlock(adv->buffer, adv->size);
        g_free(adv->buffer);
    }
}

/*
 * Initiate the adversaries data structure depedning on the method used
 */
void adversary_init_data(Adversary *adv)
{
    switch (adv->method)
    {
    case ADVERSARY_ZERO_FILL:
        // Nothing need to be done
        break;
    case ADVERSARY_STATIC_FILL:
        adv->data.sf.set = 0;
        adv->data.sf.pattern = '\00';
        break;
    case ADVERSARY_PRNG_MT:
        adv->data.mt.status = NOT_STARTED;
        adv->data.mt.sadr = 0;
        break;
    default:
        fprintf(stderr, "(adversary_feed) Unkown Adversary method [%u]\n", adv->method);
        break;
    }
}

/*
 * TODO: Implement this method
 * This method gives the adversary the buffer that was inteneded to be written 
 * memory. This should be used to `train` the adversary so it's predictions
 * can match what would be written.
 */
void adversary_feed(Adversary *adv, unsigned long adr, unsigned long len)
{
    printf("ADVERSARY: (adversary_feed)\t");
    printf("adr: 0x%08X\t", adr);
    printf("len: %lu\n", len);
    unsigned char *buf;

    assert(adv->size >= len);

    switch (adv->method)
    {
    case ADVERSARY_ZERO_FILL:
        // We don't need to do anything
        break;
    case ADVERSARY_STATIC_FILL:
        buf = adv->buffer;
        if (adv->data.sf.set == 0) {
            adv->data.sf.set = 1;
            adv->data.sf.pattern = buf[0];
            // printf("adversary_feed:\t adv->data.set=%d\n", adv->data.sf.set);
            // printf("adversary_feed:\t adv->data.set=%c\n", adv->data.sf.pattern);
        }
        for (unsigned long i = 0; i < len; i++) {
            if ( buf[i] != adv->data.sf.pattern ) {
                fprintf(stderr, "(adversary_feed) Static Fill got diffirent patterns\tbuf[i]=%c\tadv->data.pattern=%c\n", buf[i], adv->data.sf.pattern);
                return;
            }
        }
        break;
    case ADVERSARY_PRNG_MT:
        if (adv->data.mt.status == DONE)
            return;
        if (adv->data.mt.status == NOT_STARTED) {
            adv->data.mt.sadr = adr;    
        }
        // We assume the buffer is 4k
        // We should get 1024 numbers from one buffer
        // TODO: generate 624 u32 numbers 
        mtp_feed(&adv->data.mt.mtp, adv->data.mt.sample);

        break;
    default:
        fprintf(stderr, "(adversary_feed) Unkown Adversary method [%u]\n", adv->method);
        break;
    }
}

/*
 * Predict what would be stored in the given memory region given by `adr`
 * and `len`. Store this in the adversarys buffer.
 */
void adversary_predict(Adversary *adv, unsigned long adr, unsigned long len)
{
    printf("ADVERSARY: (adversary_predict)\t");
    printf("adr: 0x%08X\t", adr);
    printf("len: %lu\n", len);

    assert(adv->size >= len);

    // TODO: Make sure to fill the buffer with a prediction.
    switch (adv->method)
    {
    case ADVERSARY_ZERO_FILL:
        memset(adv->buffer, 0, len);
        break;
    case ADVERSARY_STATIC_FILL:
        if (adv->data.sf.set) {
            memset(adv->buffer, adv->data.sf.pattern, len);
        }
        else {
            fprintf(stderr, "(adversary_predict) Static Fill is not 'set'\n");
        }
        break;
    case ADVERSARY_PRNG_MT:
        mtp_predict_buffer(&adv->data.mt.mtp, adv->buffer, adr, len);
        break;
    default:
        fprintf(stderr, "(adversary_predict) Unkown Adversary method [%u]\n", adv->method);
        break;
    }
    
}

/*
 * Toggle the adversarys activity. When off the adversary will not 
 * act on victim interactions.
 */
void adversary_toggle(Adversary *adv)
{
    adv->ON = adv->ON ? 0 : 1;
}

/*
 * Set any given method to active given a Adversary
 */
int adversary_set_method(Adversary *adv, long method)
{
    if (method >= _NUMBER_OF_VALUES)
        return -1;

    adv->method = (enum AdversaryMethod) method;
    adversary_init_data(adv);
    return 0;
}