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

#include "retro_disk_control.h"
#include "retro_strings.h"
#include "retro_files.h"
#include "libretro.h"

#include "file/file_path.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define COMMENT '#'
#define M3U_SPECIAL_COMMAND "#COMMAND:"
#define M3U_NONSTD_LABEL    "#LABEL:"
#define M3U_EXTSTD_LABEL    "#EXTINF:" // Title should be following comma
#define VFL_UNIT_ENTRY      "UNIT "

#define MAX_LABEL_LEN     27 // Max of disk (27) and tape (24)

#define D64_NAME_POS      0x16590 // Sector 18/0, offset 0x90
#define D64_FULL_NAME_LEN 27      // Including id, dos version and paddings
#define D64_NAME_LEN      16

#define T64_NAME_POS      40
#define T64_NAME_LEN      24

#undef  DISK_LABEL_RELAXED         // Use label even if it doesn't look sane (mostly for testing)
#undef  DISK_LABEL_FORBID_SHIFTED  // Reject label if has shifted chars

static const char flip_file_header[] = "# Vice fliplist file";

extern retro_log_printf_t log_cb;

// Return the directory name of 'filename' without trailing separator.
// Allocates returned string.
static char* dirname_int(const char* filename)
{
    if (filename == NULL)
        return NULL;

    // Find last separator
    char* right = find_last_slash(filename);
    if (right)
        return strleft(filename, right - filename);

    // Not found
    return NULL;
}

// Add known disk labels from conversion tools, famous collectors etc.
static const char const* rude_words[] = {
    "semprini",
    "ass presents",
    "ass presents:"
};

// Filter out this annoying "ASS PRESENTS"
static bool is_ugly(const char* label)
{
    size_t i;
    for (i = 0; i < sizeof(rude_words) / sizeof(rude_words[0]); ++i)
    {
        if (strcasecmp(label, rude_words[i]) == 0)
            return true;
    }
    return false;
}

#define PETSCII_SHIFTED_BIT       0x20

#define PETSCII_SPACE             0x20
#define PETSCII_NBSP              0xA0
#define PETSCII_UNSHIFTED_A       0x40
#define PETSCII_UNSHIFTED_Z       0x5A
#define PETSCII_SHIFTED_A         0x60
#define PETSCII_SHIFTED_Z         0x7A

// Try to read disk or tape name from image
// Allocates returned string
static char* get_label(const char* filename)
{
    unsigned char label[MAX_LABEL_LEN + 1];
    bool have_disk_label = false;
    bool have_tape_label = false;
    bool have_shifted = false;
    int i;

    label[0] = '\0';
    // Disk image which we can read name from
    if (strendswith(filename, "d64") || strendswith(filename, "d71"))
    {
        FILE* fd = fopen(filename, "rb");

        if (fd != NULL)
        {
            if (fseek(fd, D64_NAME_POS, SEEK_SET) == 0
                && fread(label, D64_FULL_NAME_LEN, 1, fd) == 1)
            {
                label[D64_FULL_NAME_LEN] = '\0';
                have_disk_label = true;
            }
            fclose(fd);
        }
    }

    // Tape image which we can read name from
    if (strendswith(filename, "t64"))
    {
        FILE* fd = fopen(filename, "rb");

        if (fd != NULL)
        {
            if (fseek(fd, T64_NAME_POS, SEEK_SET) == 0
                && fread(label, T64_NAME_LEN, 1, fd) == 1)
            {
                label[T64_NAME_LEN] = '\0';
                have_tape_label = true;
            }
            fclose(fd);
        }
    }

    // Nothing found
    if (!have_disk_label && !have_tape_label)
        return NULL;

    // Special processing for disk label - sanity check and trimming
    if (have_disk_label)
    {
#if !defined(DISK_LABEL_RELAXED)
        // Chack if all characters in disk name and padding areas look valid
        // that may be too picky, but it's better to show nothing than garbage
        for (i = 0; i < D64_FULL_NAME_LEN; ++i)
        {
            unsigned char c = label[i];
            if (c != PETSCII_NBSP && (c < PETSCII_SPACE || c > PETSCII_SHIFTED_Z))
            {
                return NULL;
            }
        }
#endif
        label[D64_NAME_LEN - 1] = '\0';
    }

    // Remove trailing spaces
    i = strlen((char*)label) - 1;
    while (i > 0 && (label[i] == PETSCII_NBSP || label[i] == PETSCII_SPACE))
        label[i--] = '\0';

    // Replace other nbsp with spaces
    while (i > 0)
    {
        if (label[i] == PETSCII_NBSP)
            label[i] = ' ';
        --i;
    }

    // Check for shifted chars
    for (i=0; label[i] != '\0'; ++i)
    {
        unsigned char c = label[i];
        if (c >= PETSCII_SHIFTED_A)
        {
#if defined(DISK_LABEL_FORBID_SHIFTED)
            return NULL;
#endif
            // Have shifted chars
            have_shifted = true;
            break;
        }
    }

    int mode = disk_label_mode;
    if (have_shifted 
        && (mode == DISK_LABEL_MODE_ASCII_OR_UPPERCASE || mode == DISK_LABEL_MODE_ASCII_OR_CAMELCASE))
    {
        mode = DISK_LABEL_MODE_ASCII;
    }

    bool was_space = true;
    // Convert petscii to ascii
    for (i=0; label[i] != '\0'; ++i)
    {
        unsigned char c = label[i];
        if (c == PETSCII_SPACE)
        {
            was_space = true;
        }
        else
        {
            if (c >= PETSCII_UNSHIFTED_A && c <= PETSCII_UNSHIFTED_Z)
            {
                // Unshifted - starts as uppercase, toggles to lowercase
                if (mode == DISK_LABEL_MODE_ASCII || mode == DISK_LABEL_MODE_LOWERCASE
                    || (mode == DISK_LABEL_MODE_ASCII_OR_CAMELCASE && !was_space))
                {
                    label[i] = c ^ PETSCII_SHIFTED_BIT;
                }
            }
            else if (c >= PETSCII_SHIFTED_A && c <= PETSCII_SHIFTED_Z)
            {
                // Shifted - starts as lowercase, toggles to uppercase
                if (mode == DISK_LABEL_MODE_ASCII || mode == DISK_LABEL_MODE_UPPERCASE)
                {
                    label[i] = c ^ PETSCII_SHIFTED_BIT;
                }
            }
            was_space = false;
        }
    }

    if (is_ugly((char*)label))
    {
        return NULL;
    }

    return strdup((char*)label);
}

// Search for image file relative to M3U
// Allocates returned string
static char* m3u_search_file(const char* basedir, const char* dskName)
{
    // If basedir was provided and path is relative, search relative to M3U file location
    if (basedir != NULL && !path_is_absolute(dskName))
    {
        // Join basedir and dskName
        char* dskPath = path_join_dup(basedir, dskName);

        // Verify if this item is a relative filename (append it to the m3u path)
        if (file_exists(dskPath))
        {
            // Return
            return dskPath;
        }
        free(dskPath);
    }

    // Verify if this item is an absolute pathname (or the file is in working dir)
    if (file_exists(dskName))
    {
        // Copy and return
        return strdup(dskName);
    }

    // File not found
    return NULL;
}

void dc_reset(dc_storage* dc)
{
    // Verify
    if (dc == NULL)
        return;

    // Clean the command
    free(dc->command);
    dc->command = NULL;

    // Clean the struct
    for (unsigned i=0; i < dc->count; i++)
    {
        free(dc->files[i]);
        dc->files[i] = NULL;
        free(dc->labels[i]);
        dc->labels[i] = NULL;
    }

    dc->unit = 0;
    dc->count = 0;
    dc->index = 0; // index should never be -1
    dc->eject_state = true;
}

dc_storage* dc_create(void)
{
    // Initialize the struct
    dc_storage* dc = NULL;

    if((dc = malloc(sizeof(dc_storage))) != NULL)
    {
        dc->unit = 0;
        dc->count = 0;
        dc->index = 0; // index should never be -1
        dc->eject_state = true;
        dc->command = NULL;
        for(int i = 0; i < DC_MAX_SIZE; i++)
        {
            dc->files[i] = NULL;
            dc->labels[i] = NULL;
        }
    }

    return dc;
}

bool dc_add_file_int(dc_storage* dc, char* filename, char* label)
{
    // Verify
    if (filename == NULL || dc == NULL || dc->count > DC_MAX_SIZE)
    {
        free(filename);
        free(label);

        return false;
    }

    // Add the file
    dc->count++;
    dc->files[dc->count-1] = filename;
    dc->labels[dc->count-1] = label;
    return true;
}

bool dc_add_file(dc_storage* dc, const char* filename)
{
    // Verify
    if (dc == NULL || filename == NULL)
        return false;

    // Determine if tape or disk fliplist from first entry
    if (dc->unit == 0)
    {
        if (strendswith(filename, "tap") || strendswith(filename, "t64"))
        {
            dc->unit = 1;
        }
        else
        {
            dc->unit = 8;
        }
    }

    // Copy and return
    return dc_add_file_int(dc, strdup(filename), get_label(filename));
}

bool dc_remove_file(dc_storage* dc, int index)
{
    if (dc == NULL)
        return false;

    if (index < 0 || index >= dc->count)
        return false;

    // "If ptr is a null pointer, no action occurs"
    free(dc->files[index]);
    dc->files[index] = NULL;
    free(dc->labels[index]);
    dc->labels[index] = NULL;

    // Shift all entries after index one slot up
    if (index != dc->count - 1)
        memmove(dc->files + index, dc->files + index + 1, (dc->count - 1 - index) * sizeof(dc->files[0]));

    dc->count--;

    // Reset fliplist unit after removing last entry
    if (dc->count == 0)
    {
        dc->unit = 0;
    }

    return true;
}

bool dc_replace_file(dc_storage* dc, int index, const char* filename)
{
    if (dc == NULL)
        return false;

    if (index < 0 || index >= dc->count)
        return false;

    // "If ptr is a null pointer, no action occurs"
    free(dc->files[index]);
    dc->files[index] = NULL;
    free(dc->labels[index]);
    dc->labels[index] = NULL;

    if (filename == NULL)
    {
        dc_remove_file(dc, index);
    }
    else
    {
        dc->files[index] = strdup(filename);
        dc->labels[index] = get_label(filename);
    }

    return true;
}

void dc_parse_list(dc_storage* dc, const char* list_file, bool is_vfl)
{
    // Verify
    if (dc == NULL)
        return;

    // Reset
    dc_reset(dc);

    if (list_file == NULL)
        return;

    FILE* fp = NULL;

    // Try to open the file
    if ((fp = fopen(list_file, "r")) == NULL)
    {
        log_cb(RETRO_LOG_ERROR, "Failed to open list file %s\n", list_file);
        return;
    }

    // Read the lines while there is line to read and we have enough space
    char buffer[2048];

    // Enforce standard compatibility to avoid invalid vfl files on the loose
    if (is_vfl)
    {
        // Read and verify header
        if (fgets(buffer, sizeof(buffer), fp) == NULL
            || strncmp(buffer, flip_file_header, strlen(flip_file_header)) != 0)
        {
            log_cb(RETRO_LOG_ERROR, "File %s is not a fliplist file\n", list_file);
            fclose(fp);
            return;
        }
    }

    // Get the list base dir for resolving relative path
    char* basedir = dirname_int(list_file);

    // Label for the following file
    char* label = NULL;

    while ((dc->count <= DC_MAX_SIZE) && (fgets(buffer, sizeof(buffer), fp) != NULL))
    {
        char* string = trimwhitespace(buffer);

        if (string[0] == '\0')
            continue;

        // Is it a m3u special key? (#COMMAND:)
        if (!is_vfl && strstartswith(string, M3U_SPECIAL_COMMAND))
        {
            dc->command = strright(string, strlen(string) - strlen(M3U_SPECIAL_COMMAND));
        }
        // Disk name / label (#LABEL:)
        else if (!is_vfl && strstartswith(string, M3U_NONSTD_LABEL))
        {
            char* newlabel = trimwhitespace(buffer + strlen(M3U_NONSTD_LABEL));
            free(label);
            label = newlabel[0] ? strdup(newlabel) : NULL;
        }
        // Disk name / label - EXTM3U standard compiant (#EXTINF)
        else if (!is_vfl && strstartswith(string, M3U_EXTSTD_LABEL))
        {
            // "Track" description should be following comma (https://en.wikipedia.org/wiki/M3U#Extended_M3U)
            char* newlabel = strchr(buffer + strlen(M3U_EXTSTD_LABEL), ',');
            if (newlabel)
                newlabel = trimwhitespace(newlabel);
            free(label);
            label = (newlabel && newlabel[0]) ? strdup(newlabel) : NULL;
        }
        // VFL UNIT number (UNIT)
        else if (is_vfl && strstartswith(string, VFL_UNIT_ENTRY))
        {
            int unit = strtol(string + strlen(VFL_UNIT_ENTRY), NULL, 10);
            // VICE doesn't allow flip list for tape (unit 1),
            // but let's not split hairs
            if (unit != 1 && !(unit >= 8 && unit <= 11))
            {
                // Invalid unit number - just ignore the list
                log_cb(RETRO_LOG_ERROR, "Invalid unit number %d in fliplist %s", unit, list_file);
                break;
            }
            if (dc->unit != 0 && dc->unit != unit && dc->count != 0)
            {
                // VFL file could theoretically contain flip lists for multliple drives - read only the first one.
                log_cb(RETRO_LOG_WARN, "Ignored entries for other unit(s) in fliplist %s", list_file);
                break;
            }
            dc->unit = unit;
        }
        // Vice doesn't allow comments in vfl files - enforce the standard
        // Not a comment
        else if (is_vfl || string[0] != COMMENT)
        {
            // Search the file (absolute, relative to m3u)
            char* filename;
            if ((filename = m3u_search_file(basedir, string)) != NULL)
            {
                // Add the file to the struct
                dc_add_file_int(dc, filename, label ? label : get_label(filename));
                label = NULL;
            }
            else
            {
                log_cb(RETRO_LOG_WARN, "File %s from list %s not found\n", list_file);
                // Throw away the label
                free(label);
                label = NULL;
            }
        }
    }

    // If it's vfl, we have to reverse it
    if (is_vfl)
    {
        int idx = 0;
        int ridx = dc->count - 1;
        while (idx < ridx)
        {
            char* tmp = dc->files[idx];
            dc->files[idx] = dc->files[ridx];
            dc->files[ridx] = tmp;
            tmp = dc->labels[idx];
            dc->labels[idx] = dc->labels[ridx];
            dc->labels[ridx] = tmp;
            ++idx; --ridx;
        }
    }

    free(basedir);
    free(label);

    // Close the file 
    fclose(fp);

    // M3U - Determine if tape or disk fliplist from first entry
    if (dc->unit == 0 && dc->count !=0)
    {
        if (strendswith(dc->files[0], "tap") || strendswith(dc->files[0], "t64"))
        {
            dc->unit = 1;
        }
        else
        {
            dc->unit = 8;
        }
    }

}

void dc_parse_m3u(dc_storage* dc, const char* m3u_file)
{
    dc_parse_list(dc, m3u_file, false);
}

void dc_parse_vfl(dc_storage* dc, const char* vfl_file)
{
    dc_parse_list(dc, vfl_file, true);
}

void dc_free(dc_storage* dc)
{
    // Clean the struct
    dc_reset(dc);
    free(dc);
    dc = NULL;
    return;
}
