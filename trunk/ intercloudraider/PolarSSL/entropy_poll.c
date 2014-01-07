

#include "polarssl/entropy.h"
#include "polarssl/entropy_poll.h"

#if defined(POLARSSL_TIMING_C)
#include "polarssl/timing.h"
#endif
#if defined(POLARSSL_HAVEGE_C)
#include "polarssl/havege.h"
#endif

#if !defined(POLARSSL_NO_PLATFORM_ENTROPY)
#if defined(_WIN32)

#if !defined(_WIN32_WINNT)
#define _WIN32_WINNT 0x0400
#endif
#include <windows.h>
#include <wincrypt.h>

int platform_entropy_poll( void *data, unsigned char *output, size_t len,
                           size_t *olen )
{
    HCRYPTPROV provider;
    ((void) data);
    *olen = 0;
 
    if( CryptAcquireContext( &provider, NULL, NULL,
                              PROV_RSA_FULL, CRYPT_VERIFYCONTEXT ) == FALSE )
    {
        return POLARSSL_ERR_ENTROPY_SOURCE_FAILED;
    }

    if( CryptGenRandom( provider, len, output ) == FALSE )
        return POLARSSL_ERR_ENTROPY_SOURCE_FAILED;

    CryptReleaseContext( provider, 0 );
    *olen = len;

    return( 0 );
}
#else
 
#include <stdio.h>

int platform_entropy_poll( void *data,
                           unsigned char *output, size_t len, size_t *olen )
{
    FILE *file;
    size_t ret;
    ((void) data);

    *olen = 0;

    file = fopen( "/dev/urandom", "rb" );
    if( file == NULL )
        return POLARSSL_ERR_ENTROPY_SOURCE_FAILED;
    
    ret = fread( output, 1, len, file );
    if( ret != len )
    {
        fclose( file );
        return POLARSSL_ERR_ENTROPY_SOURCE_FAILED;
    }

    fclose( file );
    *olen = len;

    return( 0 );
}
#endif
#endif

#if defined(POLARSSL_TIMING_C)
int hardclock_poll( void *data,
                    unsigned char *output, size_t len, size_t *olen )
{
    unsigned long timer = hardclock();
    ((void) data);
    *olen = 0;

   if( len < sizeof(unsigned long) )
        return( 0 );

    memcpy( output, &timer, sizeof(unsigned long) );
    *olen = sizeof(unsigned long);

    return( 0 );
}
#endif

#if defined(POLARSSL_HAVEGE_C)
int havege_poll( void *data,
                 unsigned char *output, size_t len, size_t *olen )
{
    havege_state *hs = (havege_state *) data;
    *olen = 0;

    if( havege_random( hs, output, len ) != 0 )
        return POLARSSL_ERR_ENTROPY_SOURCE_FAILED;

    *olen = len;

    return( 0 );
}
#endif

