/* Copyright (C) 2018 
 *
 * Permission is hereby granted, free of charge,
 * to any person obtaining a copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef RETRO_DISK_CONTROL_H__
#define RETRO_DISK_CONTROL_H__

#include <stdbool.h>

// See which looks best in most cases and tweak (or make configurable)
#define DISK_LABEL_MODE_ASCII              1 // Convert to ascii - unshifted chars are lowercase
#define DISK_LABEL_MODE_UPPERCASE          2 // All characters are always uppercase
#define DISK_LABEL_MODE_LOWERCASE          3 // All characters are always lowercase
#define DISK_LABEL_MODE_ASCII_OR_UPPERCASE 4 // If all chars are unshifted - convert to uppercase
#define DISK_LABEL_MODE_ASCII_OR_CAMELCASE 5 // If all chars are unshifted - convert to camelcase
#define DISK_LABEL_MODE_PETSCII            6 // Unmodified petscii codes - case is inverted

extern int disk_label_mode;

//*****************************************************************************
// Disk control structure and functions
#define DC_MAX_SIZE 20

struct dc_storage
{
	char* command;
	char* files[DC_MAX_SIZE];
    char* labels[DC_MAX_SIZE];
	unsigned unit;
	unsigned count;
	int index;
	bool eject_state;
};

typedef struct dc_storage dc_storage;
dc_storage* dc_create(void);
void dc_parse_m3u(dc_storage* dc, const char* m3u_file);
void dc_parse_vfl(dc_storage* dc, const char* vfl_file);
bool dc_add_file(dc_storage* dc, const char* filename);
void dc_free(dc_storage* dc);
void dc_reset(dc_storage* dc);
bool dc_replace_file(dc_storage* dc, int index, const char* filename);
bool dc_remove_file(dc_storage* dc, int index);

#endif
