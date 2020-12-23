/*
    Copyright (C) 2009 Hans Beckerus (hans.beckerus#AT#gmail.com)

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

#include <platform.h>
#include <string.h>
#include "dirlist.h"
#include "hash.h"

#define DIR_LIST_HEAD_ ((void*)-1)

/*!
 *****************************************************************************
 *
 ****************************************************************************/
static inline int swap(struct dir_entry_list *A, struct dir_entry_list *B)
{
        int swap = strcmp(A->entry.name, B->entry.name);
        swap = !swap ? A->entry.type > B->entry.type : swap;
        if (swap > 0) {
                const struct dir_entry TMP = B->entry;
                B->entry = A->entry;
                A->entry = TMP;
                return 1;
        }
        return 0;
}

/*!
 *****************************************************************************
 *
 ****************************************************************************/
void dir_list_open(struct dir_entry_list *root)
{
        root->next = NULL;
        root->entry.name = NULL;
        root->entry.head_flag = DIR_LIST_HEAD_;
}

/*!
 *****************************************************************************
 *
 ****************************************************************************/
void dir_list_close(struct dir_entry_list *root)
{
        /* Simple bubble sort of directory entries in alphabetical order */
        if (root && root->next) {
                int n;
                struct dir_entry_list *next;
                do {
                        n = 0;
                        next = root->next;
                        while (next->next) {
                                n += swap(next, next->next);
                                next = next->next;
                        }
                } while (n != 0);       /* while swaps performed */

                /* Make sure entries are unique. Duplicates will be removed. */
                next = root->next;
                while (next->next) {
                        if ((next->entry.type == DIR_E_NRM || /* no hash */
                                    next->entry.hash == next->next->entry.hash) &&
                                    !strcmp(next->entry.name, next->next->entry.name)) {
                                /* 
                                 * A duplicate. Rare but possible.
                                 * Make sure the current entry is kept marked
                                 * as valid since regular fs entries should
                                 * always have priority.
                                 */
                                next->next->entry.valid = 0;
                        } 
                        next = next->next;
                }
        }
}

/*!
 *****************************************************************************
 *
 ****************************************************************************/
void dir_list_free(struct dir_entry_list *root)
{
        struct dir_entry_list *next = root->next;
        while (next) {
                struct dir_entry_list *tmp = next;
                next = next->next;
                free(tmp->entry.name);
                free(tmp);
        }
}

/*!
 *****************************************************************************
 *
 ****************************************************************************/
struct dir_entry_list *dir_entry_add(struct dir_entry_list *l, const char *key,
                struct stat *st, int type)
{
        uint32_t hash = get_hash(key, 0);
        if (l->entry.head_flag != DIR_LIST_HEAD_) {
                if (hash == l->entry.hash)
                        if (!strcmp(key, l->entry.name))
                                return l;
        }
        l->next = malloc(sizeof(struct dir_entry_list));
        if (l->next) {
                l = l->next;
                l->entry.name = strdup(key);
                l->entry.hash = hash;
                l->entry.st = st;
                l->entry.type = type;
                l->entry.valid = 1; /* assume entry is valid */
                l->next = NULL;
        }
        return l;
}

/*!
 *****************************************************************************
 *
 ****************************************************************************/
struct dir_entry_list *dir_list_dup(const struct dir_entry_list *src)
{
        dir_entry_list *root = malloc(sizeof(struct dir_entry_list));
        dir_entry_list *l = root;
        if (l) {
                struct dir_entry_list *next = src->next;
                dir_list_open(root);
                while (next) {
                        l->next = malloc(sizeof(struct dir_entry_list));
                        l = l->next;
                        l->entry.name = strdup(next->entry.name);
                        l->entry.hash = next->entry.hash;
                        l->entry.type = next->entry.type;
                        l->entry.valid = next->entry.valid;
                        l->entry.st = next->entry.st;
                        next = next->next;
                }
                l->next = NULL;
        }
        return root;
}

/*!
 *****************************************************************************
 *
 ****************************************************************************/
struct dir_entry_list *dir_list_append(struct dir_entry_list *list1,
                const struct dir_entry_list *list2)
{
        /* TODO: Make sure list1/list2 are heads */
        struct dir_entry_list *next1 = list1->next;
        struct dir_entry_list *next2 = list2->next;
        struct dir_entry_list *prev_next = list1;
        while (next1) {
                prev_next = next1;
                next1 = next1->next;
        }
        next1 = prev_next;

        while (next2) {
                next1->next = malloc(sizeof(struct dir_entry_list));
                memcpy(next1->next, next2, sizeof(struct dir_entry_list));
                next1 = next1->next;
                next1->entry.name = strdup(next2->entry.name);
                next2 = next2->next;
        }
        return next1;
}

