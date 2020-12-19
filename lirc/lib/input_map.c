/****************************************************************************
** input_map.c *************************************************************
****************************************************************************
*
* input_map.c - button namespace derived from Linux input layer
*
* Copyright (C) 2008 Christoph Bartelmus <lirc@bartelmus.de>
*
*/

/**
 * @file input_map.c
 * @brief Implements input_map.h
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __linux__
#include "lirc/input_map.h"
#else
typedef unsigned short linux_input_code;
#endif

struct {
	char*			name;
	linux_input_code	code;
} input_map[] = {
#include "lirc/input_map.inc"
	{
		NULL, 0
	}
};

int get_input_code(const char* name, linux_input_code* code)
{
	int i;

	for (i = 0; input_map[i].name != NULL; i++) {
		if (strcasecmp(name, input_map[i].name) == 0) {
			*code = input_map[i].code;
			return i;
		}
	}
	return -1;
}

void fprint_namespace(FILE* f)
{
	int i;

	for (i = 0; input_map[i].name != NULL; i++)
		fprintf(stdout, "%s\n", input_map[i].name);
}

int is_in_namespace(const char* name)
{
	linux_input_code dummy;

	return get_input_code(name, &dummy) == -1 ? 0 : 1;
}
