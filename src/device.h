/*****************************************************************************
 * device.h: DVD device access
 *****************************************************************************
 * Copyright (C) 1998-2002 VideoLAN
 * $Id: device.h,v 1.1 2002/08/09 14:10:43 sam Exp $
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
 * iovec structure: vectored data entry
 *****************************************************************************/
#if defined( WIN32 )
#   include <io.h>                                                 /* read() */
#else
#   include <sys/uio.h>                                      /* struct iovec */
#endif

#if defined( WIN32 )
struct iovec
{
    void *iov_base;     /* Pointer to data. */
    size_t iov_len;     /* Length of data.  */
};
#endif

/*****************************************************************************
 * Device reading prototypes
 *****************************************************************************/
int _dvdcss_use_ioctls ( dvdcss_handle );
int _dvdcss_open       ( dvdcss_handle );
int _dvdcss_close      ( dvdcss_handle );
int _dvdcss_readv      ( dvdcss_handle, struct iovec *, int );

/*****************************************************************************
 * Device reading prototypes, win32 specific
 *****************************************************************************/
#ifdef WIN32
int _win32_dvdcss_readv  ( int, struct iovec *, int, char * );
int _win32_dvdcss_aopen  ( char, dvdcss_handle );
int _win32_dvdcss_aclose ( int );
int _win32_dvdcss_aseek  ( int, int, int );
int _win32_dvdcss_aread  ( int, void *, int );
#endif

/*****************************************************************************
 * Device reading prototypes, raw-device specific
 *****************************************************************************/
#ifndef WIN32
int _dvdcss_raw_open     ( dvdcss_handle, char * );
#endif

