/**
 * \file dvdcss.h
 * \author Stéphane Borel <stef@via.ecp.fr>
 * \author Samuel Hocevar <sam@zoy.org>
 * \brief The \e libdvdcss public header.
 *
 * This header contains the public types and functions that applications
 * using \e libdvdcss may use.
 */

/*
 * Copyright (C) 1998-2002 VideoLAN
 * $Id: dvdcss.h,v 1.3 2002/08/10 12:21:28 sam Exp $
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
 */

#ifndef _DVDCSS_DVDCSS_H
#ifndef _DOXYGEN_SKIP_ME
#define _DVDCSS_DVDCSS_H 1
#endif

#ifdef __cplusplus
extern "C" {
#endif

/** Library instance handle, to be used for each library call. */
typedef struct dvdcss_s* dvdcss_handle;


/** The block size of a DVD. */
#define DVDCSS_BLOCK_SIZE      2048

/** The default flag to be used by \e libdvdcss functions. */
#define DVDCSS_NOFLAGS         0

/** Flag to ask dvdcss_read() to decrypt the data it reads. */
#define DVDCSS_READ_DECRYPT    (1 << 0)

/** Flag to tell dvdcss_seek() it is seeking in MPEG data. */
#define DVDCSS_SEEK_MPEG       (1 << 0)

/** Flag to ask dvdcss_seek() to check the current title key. */
#define DVDCSS_SEEK_KEY        (1 << 1)


/*
 * Our version number. The variable name contains the interface version.
 */
extern char *        dvdcss_interface_2;


/*
 * Exported prototypes.
 */
extern dvdcss_handle dvdcss_open  ( char *psz_target );
extern int           dvdcss_close ( dvdcss_handle );
extern int           dvdcss_title ( dvdcss_handle,
                                    int i_block );
extern int           dvdcss_seek  ( dvdcss_handle,
                                    int i_blocks,
                                    int i_flags );
extern int           dvdcss_read  ( dvdcss_handle,
                                    void *p_buffer,
                                    int i_blocks,
                                    int i_flags );
extern int           dvdcss_readv ( dvdcss_handle,
                                    void *p_iovec,
                                    int i_blocks,
                                    int i_flags );
extern char *        dvdcss_error ( dvdcss_handle );

#ifdef __cplusplus
}
#endif

#endif /* <dvdcss/dvdcss.h> */
