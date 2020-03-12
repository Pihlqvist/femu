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
 * TODO: Implement this method
 * This method gives the adversary the buffer that was inteneded to be written 
 * memory. This should be used to `train` the adversary so it's predictions
 * can match what would be written.
 */
void adversary_feed(Adversary *adv, char *buff)
{
    printf("ADVERSARY: feed_adversary\n");
}

/*
 * Predict what would be stored in the given memory region given by `adr`
 * and `len`. Store this in the adversarys buffer.
 */
void adversary_predict(Adversary *adv, unsigned long adr, unsigned long len)
{
    printf("ADVERSARY: adversary_predict\n");
    printf("ADVERSARY: adr: %lu\t0x%08X\n", adr, adr);
    printf("ADVERSARY: len: %lu\n", len);

    assert(adv->size >= len);

    // TODO: Make sure to fill the buffer with a prediction.
    switch (adv->method)
    {
    case ADVERSARY_ZERO_FILL:
        memset(adv->buffer, 0, len);
        break;
    
    default:
        fprintf(stderr, "Unkown Adversary method\n");
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