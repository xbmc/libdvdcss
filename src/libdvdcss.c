/*****************************************************************************
 * libdvdcss.c: DVD reading library.
 *****************************************************************************
 * Copyright (C) 1998-2001 VideoLAN
 * $Id: libdvdcss.c,v 1.14 2002/08/09 14:10:43 sam Exp $
 *
 * Authors: Stéphane Borel <stef@via.ecp.fr>
 *          Samuel Hocevar <sam@zoy.org>
 *          Håkan Hjort <d95hjort@dtek.chalmers.se>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111, USA.
 *****************************************************************************/

/*****************************************************************************
 * Preamble
 *****************************************************************************/
#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#ifdef HAVE_UNISTD_H
#   include <unistd.h>
#endif

#include "dvdcss/dvdcss.h"

#include "common.h"
#include "css.h"
#include "libdvdcss.h"
#include "ioctl.h"
#include "device.h"

/*****************************************************************************
 * dvdcss_interface_?: the current libdvdcss version and interface version
 *****************************************************************************/
char * dvdcss_interface_2 = VERSION;

/*****************************************************************************
 * dvdcss_open: initialize library, open a DVD device, crack CSS key
 *****************************************************************************/
extern dvdcss_handle dvdcss_open ( char *psz_target )
{
    int i_ret;

    char *psz_method = getenv( "DVDCSS_METHOD" );
    char *psz_verbose = getenv( "DVDCSS_VERBOSE" );
#ifndef WIN32
    char *psz_raw_device = getenv( "DVDCSS_RAW_DEVICE" );
#endif

    dvdcss_handle dvdcss;

    /*
     *  Allocate the library structure
     */
    dvdcss = malloc( sizeof( struct dvdcss_s ) );
    if( dvdcss == NULL )
    {
        return NULL;
    }

    /*
     *  Initialize structure with default values
     */
#ifndef WIN32
    dvdcss->i_raw_fd = -1;
#endif
    dvdcss->p_titles = NULL;
    dvdcss->psz_device = (char *)strdup( psz_target );
    dvdcss->psz_error = "no error";
    dvdcss->i_method = DVDCSS_METHOD_KEY;
    dvdcss->b_debug = 0;
    dvdcss->b_errors = 0;

    /*
     *  Find verbosity from DVDCSS_VERBOSE environment variable
     */
    if( psz_verbose != NULL )
    {
        switch( atoi( psz_verbose ) )
        {
        case 2:
            dvdcss->b_debug = 1;
        case 1:
            dvdcss->b_errors = 1;
        case 0:
            break;
        }
    }

    /*
     *  Find method from DVDCSS_METHOD environment variable
     */
    if( psz_method != NULL )
    {
        if( !strncmp( psz_method, "key", 4 ) )
        {
            dvdcss->i_method = DVDCSS_METHOD_KEY;
        }
        else if( !strncmp( psz_method, "disc", 5 ) )
        {
            dvdcss->i_method = DVDCSS_METHOD_DISC;
        }
        else if( !strncmp( psz_method, "title", 5 ) )
        {
            dvdcss->i_method = DVDCSS_METHOD_TITLE;
        }
        else
        {
            _dvdcss_error( dvdcss, "unknown decrypt method, please choose "
                                   "from 'title', 'key' or 'disc'" );
            free( dvdcss->psz_device );
            free( dvdcss );
            return NULL;
        }
    }

    /*
     *  Open device
     */
    i_ret = _dvdcss_open( dvdcss );
    if( i_ret < 0 )
    {
        free( dvdcss->psz_device );
        free( dvdcss );
        return NULL;
    }
    
    dvdcss->b_scrambled = 1; /* Assume the worst */
    dvdcss->b_ioctls = _dvdcss_use_ioctls( dvdcss );

    if( dvdcss->b_ioctls )
    {
        i_ret = _dvdcss_test( dvdcss );
	if( i_ret < 0 )
	{
	    /* Disable the CSS ioctls and hope that it works? */
            _dvdcss_debug( dvdcss,
                           "could not check whether the disc was scrambled" );
	    dvdcss->b_ioctls = 0;
	}
	else
	{
            _dvdcss_debug( dvdcss, i_ret ? "disc is scrambled"
                                         : "disc is unscrambled" );
	    dvdcss->b_scrambled = i_ret;
	}
    }

    /* If disc is CSS protected and the ioctls work, authenticate the drive */
    if( dvdcss->b_scrambled && dvdcss->b_ioctls )
    {
        i_ret = _dvdcss_disckey( dvdcss );

        if( i_ret < 0 )
        {
            _dvdcss_close( dvdcss );
            free( dvdcss->psz_device );
            free( dvdcss );
            return NULL;
        }
    }

#ifndef WIN32
    if( psz_raw_device != NULL )
    {
        _dvdcss_raw_open( dvdcss, psz_raw_device );
    }
#endif

    return dvdcss;
}

/*****************************************************************************
 * dvdcss_error: return the last libdvdcss error message
 *****************************************************************************/
extern char * dvdcss_error ( dvdcss_handle dvdcss )
{
    return dvdcss->psz_error;
}

/*****************************************************************************
 * dvdcss_seek: seek into the device
 *****************************************************************************/
extern int dvdcss_seek ( dvdcss_handle dvdcss, int i_blocks, int i_flags )
{
    /* title cracking method is too slow to be used at each seek */
    if( ( ( i_flags & DVDCSS_SEEK_MPEG )
             && ( dvdcss->i_method != DVDCSS_METHOD_TITLE ) ) 
       || ( i_flags & DVDCSS_SEEK_KEY ) )
    {
        /* check the title key */
        if( _dvdcss_title( dvdcss, i_blocks ) ) 
        {
            return -1;
        }
    }

    return _dvdcss_seek( dvdcss, i_blocks );
}

/*****************************************************************************
 * dvdcss_title: crack or decrypt the current title key if needed
 *****************************************************************************
 * This function should only be called by dvdcss_seek and should eventually
 * not be external if possible.
 *****************************************************************************/
extern int dvdcss_title ( dvdcss_handle dvdcss, int i_block )
{
    fprintf( stderr, "WARNING: dvdcss_title() is DEPRECATED\n" );
    if( ! dvdcss->b_scrambled )
    {
        return 0;
    }

    return _dvdcss_title( dvdcss, i_block );
}

/*****************************************************************************
 * dvdcss_read: read data from the device, decrypt if requested
 *****************************************************************************/
extern int dvdcss_read ( dvdcss_handle dvdcss, void *p_buffer,
                                               int i_blocks,
                                               int i_flags )
{
    int i_ret, i_index;

    i_ret = _dvdcss_read( dvdcss, p_buffer, i_blocks );

    if( i_ret <= 0
         || !dvdcss->b_scrambled
         || !(i_flags & DVDCSS_READ_DECRYPT) )
    {
        return i_ret;
    }

    if( ! memcmp( dvdcss->css.p_title_key, "\0\0\0\0\0", 5 ) )
    {
        /* For what we believe is an unencrypted title, 
	 * check that there are no encrypted blocks */
        for( i_index = i_ret; i_index; i_index-- )
        {
            if( ((u8*)p_buffer)[0x14] & 0x30 )
            {
                _dvdcss_error( dvdcss, "no key but found encrypted block" );
                /* Only return the initial range of unscrambled blocks? */
                /* or fail completely? return 0; */
		break;
            }
            p_buffer = (void *) ((u8 *)p_buffer + DVDCSS_BLOCK_SIZE);
        }
    }
    else 
    {
        /* Decrypt the blocks we managed to read */
        for( i_index = i_ret; i_index; i_index-- )
	{
	    _dvdcss_unscramble( dvdcss->css.p_title_key, p_buffer );
	    ((u8*)p_buffer)[0x14] &= 0x8f;
            p_buffer = (void *) ((u8 *)p_buffer + DVDCSS_BLOCK_SIZE);
	}
    }
    
    return i_ret;
}

/*****************************************************************************
 * dvdcss_readv: read data to an iovec structure, decrypt if requested
 *****************************************************************************/
extern int dvdcss_readv ( dvdcss_handle dvdcss, void *_p_iovec,
                                                int i_blocks,
                                                int i_flags )
{
    struct iovec *p_iovec = (struct iovec *)_p_iovec;
    int i_ret, i_index;
    void *iov_base;
    size_t iov_len;

    i_ret = _dvdcss_readv( dvdcss, p_iovec, i_blocks );

    if( i_ret <= 0
         || !dvdcss->b_scrambled
         || !(i_flags & DVDCSS_READ_DECRYPT) )
    {
        return i_ret;
    }

    /* Initialize loop for decryption */
    iov_base = p_iovec->iov_base;
    iov_len = p_iovec->iov_len;

    /* Decrypt the blocks we managed to read */
    for( i_index = i_ret; i_index; i_index-- )
    {
        /* Check that iov_len is a multiple of 2048 */
        if( iov_len & 0x7ff )
        {
            return -1;
        }

        while( iov_len == 0 )
        {
            p_iovec++;
            iov_base = p_iovec->iov_base;
            iov_len = p_iovec->iov_len;
        }

        _dvdcss_unscramble( dvdcss->css.p_title_key, iov_base );
        ((u8*)iov_base)[0x14] &= 0x8f;

        iov_base = (void *) ((u8*)iov_base + DVDCSS_BLOCK_SIZE);
        iov_len -= DVDCSS_BLOCK_SIZE;
    }

    return i_ret;
}

/*****************************************************************************
 * dvdcss_close: close the DVD device and clean up the library
 *****************************************************************************/
extern int dvdcss_close ( dvdcss_handle dvdcss )
{
    dvd_title_t *p_title;
    int i_ret;

    /* Free our list of keys */
    p_title = dvdcss->p_titles;
    while( p_title )
    {
        dvd_title_t *p_tmptitle = p_title->p_next;
        free( p_title );
        p_title = p_tmptitle;
    }

    i_ret = _dvdcss_close( dvdcss );

    if( i_ret < 0 )
    {
        return i_ret;
    }

    free( dvdcss->psz_device );
    free( dvdcss );

    return 0;
}

