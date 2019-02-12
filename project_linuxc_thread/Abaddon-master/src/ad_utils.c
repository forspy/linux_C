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

#include <ctype.h>
#include <string.h>
#include <stdlib.h>

#include "ad_utils.h"

/* Indicates whether a given path string is
 * for a directory or not.
 *
 * @param path the path that is to be tested.
 * @return     integer indicating the directory status.
 */
int ad_utils_is_directory(const char *path)
{
    return (strrchr(path, '/') - path) == (strlen(path) - 1);
}

/* Compares two strings ignoring lowercase, uppercase.
 *
 * @param str1 first string to be compared.
 * @param str2 second string to be compared.
 * @return     if two strings are identical ignoring
 *             cases of the strings, zero.
 *             otherwise the value difference of
 *             the first two different characters.
 */
int ad_utils_strcmp_ic(register const char *str1, register const char *str2)
{
    unsigned char c1, c2;

    while ( str1 && str2 )
    {
        c1 = tolower(*str1++);
        c2 = tolower(*str2++);

        if (c1 != 0 && c1 == c2) { continue; }
        else                     { break; }
    }
    return c1 - c2;
}

/* Counts the number of occurrences of the given substring
 * in the given string.
 *
 * @param str    the string to be searched on.
 * @param substr the substring to be counted.
 * @return       count of the occurrences of the substring.
 */
size_t ad_utils_substr_exists(const char *str, const char *substr)
{
    size_t substr_count = 0;
    const char *p, *s = str;

    while(s < str && (p = strstr(s, substr)))
    {
        s = p + strlen(substr);
        substr_count++;
    }

    return substr_count;
}

/* Splits the given string into tokens with the given delimiters.
 *
 * @param str    the string to be split.
 * @param delims the delimiters to be used for splitting.
 * @return       array of parts split according to the delimiters.
 */
char **ad_utils_split_str(const char *str, const char *delims)
{
    const char *p, *s = str;
    char **tokens = NULL;
    size_t i, tkn_len;

    if (str != NULL)
    {
        for (i = 0; (p = strpbrk(s, delims)); i++)
        {
            tokens = realloc(tokens, sizeof(char *) * (i + 3));

            tkn_len = p - s;
            tokens[i] = malloc(sizeof(char) * (tkn_len + 1));

            strncpy(tokens[i], s, tkn_len);
            tokens[i][tkn_len] = '\0';

            s = p + 1;
        }

        /* Copy the last token */
        tkn_len = strlen(str) - (s - str);
        tokens[i] = malloc(sizeof(char) * (tkn_len + 1));

        strncpy(tokens[i], s, tkn_len);
        tokens[i][tkn_len] = '\0';

        tokens[i + 1] = NULL; 
    }

    return tokens;
}
