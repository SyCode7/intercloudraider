#ifndef POLARSSL_ENTROPY_POLL_H
#define POLARSSL_ENTROPY_POLL_H

#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Default thresholds for built-in sources
 */
#define ENTROPY_MIN_PLATFORM    128     
#define ENTROPY_MIN_HAVEGE      128     
#define ENTROPY_MIN_HARDCLOCK    32     
#if !defined(POLARSSL_NO_PLATFORM_ENTROPY)

int platform_entropy_poll( void *data,
                           unsigned char *output, size_t len, size_t *olen );
#endif

#if defined(POLARSSL_HAVEGE_C)

int havege_poll( void *data,
                 unsigned char *output, size_t len, size_t *olen );
#endif

#if defined(POLARSSL_TIMING_C)

int hardclock_poll( void *data,
					unsigned char *output, size_t len, size_t *olen );
#endif

#ifdef __cplusplus
}
#endif

#endif /* entropy_poll.h */
