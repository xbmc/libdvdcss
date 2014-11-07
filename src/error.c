/*****************************************************************************
 * error.c: error management functions
 *****************************************************************************
 * Copyright (C) 1998-2002 VideoLAN
 *
 * Author: Sam Hocevar <sam@zoy.org>
 *
 * libdvdcss is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * libdvdcss is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with libdvdcss; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *****************************************************************************/

#include <stdio.h>

#include "libdvdcss.h"

/*****************************************************************************
 * Error messages
 *****************************************************************************/
void print_error( dvdcss_t dvdcss, const char *psz_string )
{
    if( dvdcss->b_errors )
    {
        fprintf( stderr, "libdvdcss error: %s\n", psz_string );
    }

    dvdcss->psz_error = psz_string;
}
