/* csstest.c - test program for libdvdcss
 * 
 * Samuel Hocevar <sam@zoy.org> - June 2001
 * Updated on Nov 13th 2001 for libdvdcss version 1.0.0
 *
 * This piece of code is public domain */

#include <stdlib.h>

#include <dvdcss/dvdcss.h>

/* Macro to check if a sector is scrambled */
#define IsSectorScrambled(buf) (((unsigned char*)(buf))[0x14] & 0x30)

/* Print parts of a 2048 bytes buffer */
void dumpsector( unsigned char * );

int main( int i_argc, char *ppsz_argv[] )
{
    dvdcss_handle dvdcss;
    unsigned char p_buffer[ DVDCSS_BLOCK_SIZE ];
    unsigned int  i_sector;

    /* Print version number */
    printf( "cool, I found libdvdcss version %s\n", dvdcss_interface_2 );

    /* Check for 2 arguments */
    if( i_argc != 3 )
    {
        printf( "usage: %s <device> <sector>\n", ppsz_argv[0] );
        return -1;
    }

    /* Save the requested sector */
    i_sector = atoi( ppsz_argv[2] );

    /* Initialize libdvdcss */
    dvdcss = dvdcss_open( ppsz_argv[1] );
    if( dvdcss == NULL )
    {
        printf( "argh ! couldn't open DVD (%s)\n", ppsz_argv[1] );
        return -1;
    }

    /* Set the file descriptor at sector i_sector */
    dvdcss_seek( dvdcss, i_sector, DVDCSS_NOFLAGS );

    /* Read one sector */
    dvdcss_read( dvdcss, p_buffer, 1, DVDCSS_NOFLAGS );

    /* Print the sector */
    printf( "requested sector:\n" );
    dumpsector( p_buffer );

    /* Check if sector was encrypted */
    if( IsSectorScrambled( p_buffer ) )
    {
        /* Set the file descriptor position to the previous location */
        /* ... and get the appropriate key for this sector */
        dvdcss_seek( dvdcss, i_sector, DVDCSS_SEEK_KEY );

        /* Read sector again, and decrypt it on the fly */
        dvdcss_read( dvdcss, p_buffer, 1, DVDCSS_READ_DECRYPT );

        /* Print the decrypted sector */
        printf( "unscrambled sector:\n" );
        dumpsector( p_buffer );
    }
    else
    {
        printf( "sector is not scrambled\n" );
    }

    /* Close the device */
    dvdcss_close( dvdcss );

    return 0;
}

/* Print parts of a 2048 bytes buffer */
void dumpsector( unsigned char *p_buffer )
{
    int i_amount = 10;
    for( ; i_amount ; i_amount--, p_buffer++ ) printf( "%.2x", *p_buffer );
    printf( " ... " );
    i_amount = 25;
    p_buffer += 200;
    for( ; i_amount ; i_amount--, p_buffer++ ) printf( "%.2x", *p_buffer );
    printf( " ...\n" );
}

