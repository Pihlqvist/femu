#include "qemu/osdep.h"
#include "hw/block/block.h"
#include "hw/pci/pci.h"
#include "qemu/error-report.h"

#include "adversary.h"

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

void adversary_destroy(Adversary *adv)
{
    if (adv->buffer) {
        munlock(adv->buffer, adv->size);
        g_free(adv->buffer);
    }
}


void adversary_feed(Adversary *adv, char *buff)
{
    printf("ADVERSARY: feed_adversary\n");
}


void adversary_predict(Adversary *adv, unsigned long adr, unsigned long len)
{
    printf("ADVERSARY: adversary_predict\n");
    printf("ADVERSARY: adr: %lu\t0x%08X\n", adr, adr);
    printf("ADVERSARY: len: %lu\n", len);

    assert(adv->size >= len);

    // TODO: Make sure to fill the buffer with a prediction.
    memset(adv->buffer, 'a', len);
}

void adversary_toggle(Adversary *adv)
{
    adv->ON = adv->ON ? 0 : 1;
}