/**************************************************************************
 *  Abaddon HTTP Server
 *  Copyright (C) 2014  Tamer Tas
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *************************************************************************/

#ifndef AD_METHOD_H
#define AD_METHOD_H

#define _MIN_HASH_VALUE 3
#define _MAX_HASH_VALUE 12
#define _MIN_METHOD_LENGTH 3
#define _MAX_METHOD_LENGTH 7

unsigned int _ad_method_char_lookup(register const char *str, register unsigned int len);

int ad_method_is_valid(register const char *input);

#endif

