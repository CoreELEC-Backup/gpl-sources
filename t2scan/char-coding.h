/*
 * Simple MPEG/DVB parser to achieve network/service information without initial tuning data
 *
 * Copyright (C) 2006, 2007, 2008, 2009, 2010, 2011 Winfried Koehler 
 * Copyright (C) 2017 - 2020 mighty-p 
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 * Or, point your browser to http://www.gnu.org/licenses/old-licenses/gpl-2.0.html
 *
 * The project's page is https://github.com/mighty-p/t2scan
 */

#ifndef __CHAR_CODING_H__
#define __CHAR_CODING_H__

int get_user_codepage(void);
int get_codepage_index(const char * codepage);

/*
 * set the default charset that is used if a string does not include a charset definition in the first byte
 */
void set_char_coding_default_charset();

/*
 * reset default charset to the reset_to_charset
 */
void reset_char_coding_default_charset();

/*
 * set the charset to which reset_char_coding_default_charset() should set back the default charset
 */
void set_reset_to_charset(char* new_charset);

/*
 * handle character set correctly (via libiconv),
 * ISO/EN 300 468 annex A 
 *
 * WARNING: do NOT pass pointers to temporarly allocated memory here, which should be freed afterwards.
 * *inbuf && *outbuf will point to *different* memory afterwards.
 */
void char_coding(char ** inbuf, size_t * inbytesleft, char ** outbuf, size_t * outbytesleft, unsigned user_charset_id);

#endif
