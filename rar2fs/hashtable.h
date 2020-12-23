/*
    Copyright (C) 2009 Hans Beckerus (hans.beckerus@gmail.com)

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

    This program take use of the freeware "Unrar C++ Library" (libunrar)
    by Alexander Roshal and some extensions to it.

    Unrar source may be used in any software to handle RAR archives
    without limitations free of charge, but cannot be used to re-create
    the RAR compression algorithm, which is proprietary. Distribution
    of modified Unrar source in separate form or as a part of other
    software is permitted, provided that it is clearly stated in
    the documentation and source comments that the code may not be used
    to develop a RAR (WinRAR) compatible archiver.
*/

#ifndef HASHTABLE_H_
#define HASHTABLE_H_

#include "platform.h"
#include "hash.h"

struct hash_table_ops {
        void *(*alloc)();
        void (*free)(void *);
};

struct hash_table_entry {
        char *key;
        uint32_t hash;
        void *user_data;
        struct hash_table_entry *next;
};

struct hash_table {
        struct hash_table_entry *bucket;
        size_t size;
        struct hash_table_ops ops;
};

void *hashtable_init(size_t size, struct hash_table_ops *ops);
void hashtable_destroy(void *h);
struct hash_table_entry *hashtable_entry_alloc(void *h, const char *key);
struct hash_table_entry *hashtable_entry_get(void *h, const char *key);
void hashtable_entry_delete(void *h, const char *key);

#endif

