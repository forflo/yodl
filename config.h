/* config.h.  Generated from config.h.in by configure.  */
#ifndef IVL_config_H                                           /* -*- c++ -*- */
#define IVL_config_H
/*
 * Copyright (c) 2001-2015 Stephen Williams (steve@icarus.com)
 *
 *    This source code is free software; you can redistribute it
 *    and/or modify it in source code form under the terms of the GNU
 *    General Public License as published by the Free Software
 *    Foundation; either version 2 of the License, or (at your option)
 *    any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with this program; if not, write to the Free Software
 *    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#if defined(__cplusplus)
#  if !defined(__GNUC__)
     using namespace std;
#  elif (__GNUC__ == 3)
     using namespace std;
#  endif
#endif

/* # undef NEED_LU */
/* # undef NEED_TU */
/* # undef WLU */
/* # undef WTU */
# define HAVE_TIMES 1
# define HAVE_IOSFWD 1
# define HAVE_GETOPT_H 1
# define HAVE_INTTYPES_H 1
/* # undef HAVE_LIBIBERTY_H */
# define HAVE_DLFCN_H 1
/* # undef HAVE_DL_H */
/* # undef HAVE_FCHMOD */
# define HAVE_LIBREADLINE 1
# define HAVE_LIBZ 1
# define HAVE_LIBBZ2 1
# define HAVE_LROUND 1
# define HAVE_SYS_WAIT_H 1
/* # undef WORDS_BIGENDIAN */

#ifdef HAVE_INTTYPES_H
# include  <inttypes.h>
#endif

/* These two are needed by the lxt and lxt2 files (copied from GTKWave). */
# define HAVE_ALLOCA_H 1
# define HAVE_FSEEKO 1
/* And this is needed by the fst files (copied from GTKWave). */
# define HAVE_LIBPTHREAD 1
# define HAVE_REALPATH 1

/*
 * Define this if you want to compile vvp with memory freeing and
 * special valgrind hooks for the memory pools.
 */
/* # undef CHECK_WITH_VALGRIND */

#endif /* IVL_config_H */
