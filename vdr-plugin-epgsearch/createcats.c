/*                                                                  -*- c++ -*-
Copyright (C) 2004-2013 Christian Wieninger

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
Or, point your browser to http://www.gnu.org/licenses/old-licenses/gpl-2.0.html

The author can be reached at cwieninger@gmx.de

The project's page is at http://winni.vdr-developer.org/epgsearch
*/

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <getopt.h>

#define MINAPPEARANCE 100 // the minimum appearance of a category
#define MAXVALUES 60 // the maximum of values for a category
#define MAXNAMELENGTH 30 // the maximum length of a category name or value


// some helping stuff copied from VDR sources
#define KILOBYTE(n) ((n) * 1024)
#define MAXPARSEBUFFER KILOBYTE(10)

#ifdef __FreeBSD__
#ifdef isnumber
#undef isnumber
#endif
#endif

bool isnumber(const char *s)
{
    if (!*s)
        return false;
    while (*s) {
        if (!isdigit(*s))
            return false;
        s++;
    }
    return true;
}

// --- cReadLine -------------------------------------------------------------
class cReadLine
{
private:
    char buffer[MAXPARSEBUFFER];
public:
    cReadLine() {
        buffer[0] = 0;
    }
    char *Read(FILE *f);
};

char *cReadLine::Read(FILE *f)
{
    if (fgets(buffer, sizeof(buffer), f) > 0) {
        int l = strlen(buffer) - 1;
        if (l >= 0 && buffer[l] == '\n')
            buffer[l] = 0;
        return buffer;
    }
    return NULL;
}

char *skipspace(const char *s)
{
    while (*s && isspace(*s))
        s++;
    return (char *)s;
}

int comparevalue(const void *arg1, const void *arg2)
{
    char* value1 = *(char**) arg1;
    char* value2 = *(char**) arg2;
    return strcmp(value1, value2);
}

// --- cCat -------------------------------------------------------------
class cCat
{
public:
    int appeared;
    char name[MAXPARSEBUFFER];
    int numvalues;
    char** values;

    cCat(char* n)
        : appeared(0), numvalues(0), values(NULL) {
        strcpy(name, n);
    }
    void addvalue(char* value) {
        if (valueexists(value))
            return;
        char* newvalue = (char*) malloc(sizeof(char) * (strlen(value) + 1));
        strcpy(newvalue, value);
        char **tmp = (char**) realloc(values, sizeof(char*) * (numvalues + 1));
        if (tmp) {
            values = tmp;
            values[numvalues++] = newvalue;
        } else {
            free(newvalue);
        }
    }
    bool valueexists(char* value) {
        for (int i = 0; i < numvalues; i++)
            if (strcmp(values[i], value) == 0)
                return true;
        return false;
    }
    void sort() {
        qsort(values, numvalues, sizeof(char*), comparevalue);
    }
};

int comparecat(const void *arg1, const void *arg2)
{
    cCat* cat1 = *(cCat**) arg1;
    cCat* cat2 = *(cCat**) arg2;
    if (cat1->appeared == cat2->appeared) return 0;
    if (cat1->appeared < cat2->appeared) return 1;
    else return -1;
}

// --- cCats -------------------------------------------------------------
class cCats
{
private:
    int numcats;
    cCat** cats;
public:
    cCats(): numcats(0), cats(NULL) {}

    int num() {
        return numcats;
    }

    cCat* add(char* name) {
        cCat* newCat = new cCat(name);
        cCat **tmp = (cCat**) realloc(cats, sizeof(cCat*) * (numcats + 1));
        if (tmp) {
            cats = tmp;
            cats[numcats++] = newCat;
            return newCat;
        } else {
            delete newCat;
            return NULL;
        }
    }

    cCat* get(int i) {
        if (i >= 0 && i < numcats)
            return cats[i];
        else
            return NULL;
    }

    cCat* exists(char* name) {
        for (int i = 0; i < numcats; i++)
            if (strcmp(cats[i]->name, name) == 0)
                return cats[i];
        return NULL;
    }
    void sort() {
        for (int i = 0; i < numcats; i++)
            cats[i]->sort();
        qsort(cats, numcats, sizeof(cCat*), comparecat);
    }
};

int main(int argc, char *argv[])
{
    FILE* f = NULL;
    cCats catlist;
    unsigned int minappearance = MINAPPEARANCE;
    unsigned int maxvalues = MAXVALUES;
    unsigned int maxlength = MAXNAMELENGTH;

    static const struct option long_options[] = {
        { "minappearance", required_argument, NULL, 'm' },
        { "maxvalues",   required_argument, NULL, 'v' },
        { "maxlength",   required_argument,       NULL, 'l' },
        { "help",     no_argument,       NULL, 'h' },
        { NULL,     no_argument,       NULL, 0 }
    };

    int c;
    while ((c = getopt_long(argc, argv, "m:v:l:h", long_options, NULL)) != -1) {
        switch (c) {
        case 'm':
            if (isnumber(optarg)) {
                minappearance = atoi(optarg);
                break;
            }
            fprintf(stderr, "invalid parameter minappearance: %s\n", optarg);
            return 2;
            break;
        case 'v':
            if (isnumber(optarg)) {
                maxvalues = atoi(optarg);
                break;
            }
            fprintf(stderr, "invalid parameter maxvalues: %s\n", optarg);
            return 2;
            break;
        case 'l':
            if (isnumber(optarg)) {
                maxlength = atoi(optarg);
                break;
            }
            fprintf(stderr, "invalid parameter maxlength: %s\n", optarg);
            return 2;
            break;
        case 'h':
            printf("usage: createcats [OPTIONS] /path_to/epg.data\n\n");
            printf("-m N, --minappearance=N    the minimum number a category has to appear\n");
            printf("                           to be used\n");
            printf("-v N, --maxvalues=N        values of a category are omitted if they exceed\n");
            printf("                           this number\n");
            printf("-l N, --maxlength=N        the maximum length of a text to be accepted\n");
            printf("                           as a category value\n");
            printf("-h, --help                 this help\n\n");
            return 0;
        default:
            break;
        }
    }

    if (argc < 2) {
        fprintf(stderr, "ERROR: please pass your epg.data\nusage: createcats epg.data\n");
        return 1;
    }

    f = fopen(argv[argc - 1], "r");
    if (f == NULL) {
        fprintf(stderr, "ERROR: could not open: %s\n", argv[1]);
        return 1;
    }

    char *s;
    cReadLine ReadLine;
    while ((s = ReadLine.Read(f)) != NULL) {
        if (*s == 'D') {
            s = strchr(s, '|'); // jump to possibly first category
            if (!s)
                continue;
            s++;
            char *pstrSearchToken;
            char *pstrSearch = strdup(s);
            pstrSearchToken = strtok(pstrSearch, "|");

            while (pstrSearchToken) {
                // must have a ':'
                char* szPos = NULL;
                if ((szPos = strchr(pstrSearchToken, ':')) == NULL) {
                    pstrSearchToken = strtok(NULL, "|");
                    continue;
                }

                char catname[MAXPARSEBUFFER] = "";
                char catvalue[MAXPARSEBUFFER] = "";

                strncpy(catname, pstrSearchToken, szPos - pstrSearchToken);
                catname[szPos - pstrSearchToken] = 0;
                strcpy(catvalue, skipspace(szPos + 1));

                cCat* cat = catlist.exists(catname);
                if (!cat && strlen(catname) < maxlength) // accept only names up to 30 chars
                    cat = catlist.add(catname);

                if (cat) {
                    cat->appeared++;
                    if (strlen(catvalue) < maxlength) // accept only values up to 30 chars
                        cat->addvalue(catvalue);
                }

                pstrSearchToken = strtok(NULL, "|");
            }
            free(pstrSearch);
        }
    }
    fclose(f);

    catlist.sort();

    f = fopen("epgsearchcats.conf", "w");
    if (f == NULL) {
        fprintf(stderr, "ERROR: could not open outputfile\n");
        return 1;
    }
    fprintf(f, "# -----------------------------------------------------------------------------\n");
    fprintf(f, "# This is just a template based on your current epg.data. Please edit!\n");
    fprintf(f, "# Perhaps a category or its value list should be removed. Also the\n");
    fprintf(f, "# 'name in menu' should be adjusted to your language.\n");
    fprintf(f, "# The order of items determines the order listed in epgsearch. It does not\n");
    fprintf(f, "# depend on the ID, which is used by epgsearch.\n");
    fprintf(f, "# Format:\n");
    fprintf(f, "# ID|category name|name in menu|values separated by ',' (option)|searchmode(option)\n");
    fprintf(f, "# - 'ID' should be a unique positive integer\n");
    fprintf(f, "#   (changing the id later on will force you to reedit your search timers!)\n");
    fprintf(f, "# - 'category name' is the name in your epg.data\n");
    fprintf(f, "# - 'name in menu' is the name displayed in epgsearch.\n");
    fprintf(f, "# - 'values' is an optional list of possible values\n");
    fprintf(f, "#   if you omit the list, the entry turns to an edit field in epgsearch,\n");
    fprintf(f, "#   else it's a list of items to select from\n");
    fprintf(f, "# - 'searchmode' is an optional parameter specifying the mode of search:\n");
    fprintf(f, "#     text comparison:\n");
    fprintf(f, "#     0 - the whole term must appear as substring\n");
    fprintf(f, "#     1 - all single words (delimiters are ',', ';', '|' or '~')\n");
    fprintf(f, "#         must exist as substrings. This is the default search mode.\n");
    fprintf(f, "#     2 - at least one word (delimiters are ',', ';', '|' or '~')\n");
    fprintf(f, "#         must exist as substring.\n");
    fprintf(f, "#     3 - matches exactly\n");
    fprintf(f, "#     4 - regular expression\n");
    fprintf(f, "#     numerical comparison:\n");
    fprintf(f, "#     10 - less\n");
    fprintf(f, "#     11 - less or equal\n");
    fprintf(f, "#     12 - greater\n");
    fprintf(f, "#     13 - greater or equal\n");
    fprintf(f, "#     14 - equal\n");
    fprintf(f, "#     15 - not equal\n");
    fprintf(f, "# -----------------------------------------------------------------------------\n\n");
    int id = 1;
    for (int i = 0; i < catlist.num(); i++) {
        cCat* cat = catlist.get(i);
        if (cat->appeared > (int)minappearance && cat->numvalues > 1) { // accept only category, that have at least 2 values and appear more than MINAPPEARANCE timers
            fprintf(f, "# '%s' found %d times with %d different values %s\n", cat->name, cat->appeared, cat->numvalues, cat->numvalues >= (int)maxvalues ? "(values omitted, too much)" : "");
            fprintf(f, "%d|%s|%s|", id++, cat->name, cat->name);
            for (int j = 0; cat->numvalues < (int)maxvalues && j < cat->numvalues; j++)
                fprintf(f, "%s%s", cat->values[j], (j == cat->numvalues - 1 ? "" : ","));
            fprintf(f, "|1\n\n");
        }
    }
    fclose(f);

    printf("epgsearchcats.conf created!\n");
    return 0;
}
