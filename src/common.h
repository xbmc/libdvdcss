/*****************************************************************************
 * common.h: common definitions
 * Collection of useful common types and macros definitions
 *****************************************************************************
 * Copyright (C) 1998, 1999, 2000 VideoLAN
 * $Id: common.h,v 1.1 2001/12/22 00:08:13 sam Exp $
 *
 * Authors: Samuel Hocevar <sam@via.ecp.fr>
 *          Vincent Seguin <seguin@via.ecp.fr>
 *          Gildas Bazin <gbazin@netcourrier.com>
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
 * Basic types definitions
 *****************************************************************************/

/* Basic types definitions */
typedef unsigned char           u8;
typedef signed char             s8;
typedef unsigned short          u16;
typedef signed short            s16;
typedef unsigned int            u32;
typedef signed int              s32;
#if defined( _MSC_VER )
typedef unsigned __int64        u64;
typedef signed __int64          s64;
#else
typedef unsigned long long      u64;
typedef signed long long        s64;
#endif

typedef u8                  byte_t;

/* Boolean type */
#ifdef BOOLEAN_T_IN_SYS_TYPES_H
#   include <sys/types.h>
#elif defined(BOOLEAN_T_IN_PTHREAD_H)
#   include <pthread.h>
#elif defined(BOOLEAN_T_IN_CTHREADS_H)
#   include <cthreads.h>
#else
typedef int                 boolean_t;
#endif
#ifdef SYS_GNU
#   define _MACH_I386_BOOLEAN_H_
#endif

#if defined( WIN32 )

/* several type definitions */
#   if defined( __MINGW32__ )
#       if !defined( _OFF_T_ )
typedef long long _off_t;
typedef _off_t off_t;
#           define _OFF_T_
#       else
#           define off_t long long
#       endif
#   endif

#   if defined( _MSC_VER )
#       if !defined( _OFF_T_DEFINED )
typedef __int64 off_t;
#           define _OFF_T_DEFINED
#       else
#           define off_t __int64
#       endif
#       define stat _stati64
#   endif

#   ifndef snprintf
#       define snprintf _snprintf  /* snprintf not defined in mingw32 (bug?) */
#   endif

#endif

