/*
 *  file.c
 *  liborganya
 *
 *  Created by Vincent Spader on 6/20/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */
#include <stdlib.h>
#include <string.h>
#include "file.h"
#include "swap.h"


#include <kodi/Filesystem.h>

// File reading helpers 
uint8_t _org_read_8(void* fin) {
	uint8_t i = 0;
	
	static_cast<kodi::vfs::CFile*>(fin)->Read(&i, 1);
	return i;
}

uint16_t _org_read_16(void* fin) {
	uint16_t i = 0;
	static_cast<kodi::vfs::CFile*>(fin)->Read(&i, 2);
	return org_ltoh_16(i);
}

uint32_t _org_read_32(void* fin) {
	uint32_t i = 0;
	static_cast<kodi::vfs::CFile*>(fin)->Read(&i, 4);
	return org_ltoh_32(i);
}

// Read the usual org header
void _org_read_header(org_header_t *header, void* fin)
{
	// Read the magic. All orgyana files start with Org-02.
	int8_t buf[6];
	static_cast<kodi::vfs::CFile*>(fin)->Read(buf, 6);
	if(0 != memcmp(buf, "Org-02", 6)) {
		throw NULL;
	}
	
	header->tempo = _org_read_16(fin);
	header->steps_per_bar = _org_read_8(fin);
	header->beats_per_step = _org_read_8(fin);
	header->loop_start = _org_read_32(fin);
	header->loop_end = _org_read_32(fin);
}

// Read properties for the instrument
void _org_read_instrument(org_instrument_t *instrument, void* fin)
{
	instrument->pitch = _org_read_16(fin);
	instrument->instrument = _org_read_8(fin);
	instrument->disable_sustain = _org_read_8(fin);
	instrument->note_count = _org_read_16(fin);
}

// Read properties for each note
void _org_read_notes(org_note_t notes[], void* fin, uint16_t note_count)
{
	for (uint16_t i = 0; i < note_count; i++) {
		notes[i].start = _org_read_32(fin);
	}
	for (uint16_t i = 0; i < note_count; i++) {
		notes[i].key = _org_read_8(fin);
	}
	for (uint16_t i = 0; i < note_count; i++) {
		notes[i].length = _org_read_8(fin);
	}
	for (uint16_t i = 0; i < note_count; i++) {
		notes[i].volume = _org_read_8(fin);
	}
	for (uint16_t i = 0; i < note_count; i++) {
		notes[i].pan = _org_read_8(fin);
	}
}

// Rather straightforward just follows the file format.
org_file_t *_org_file_create(void* fin) {
	org_file_t *org = ( org_file_t * ) calloc(1, sizeof(org_file_t));
	if ( !org ) throw std::bad_alloc();
	try
	{
		_org_read_header(&org->header, fin);

		// Read instrument properties
		for (uint8_t i = 0; i < 16; i++) {
			_org_read_instrument(&org->instruments[i], fin);

			// Allocate space for notes
			if (org->instruments[i].note_count) {
				org->instruments[i].notes = ( org_note_t * ) malloc(sizeof(org_note_t) * org->instruments[i].note_count);
				if ( !org->instruments[i].notes ) throw std::bad_alloc();
			}
			else {
				org->instruments[i].notes = NULL;
			}
		}

		// Read notes for each instrument
		for (uint8_t i = 0; i < 16; i++) {
			_org_read_notes(org->instruments[i].notes, fin, org->instruments[i].note_count);
		}

		return org;
	}
	catch (...)
	{
		_org_file_destroy( org );
		throw;
	}
}

void _org_file_destroy(org_file_t *org) {
	// Free up memory
	for (uint8_t i = 0; i < 16; i++) {
		if (org->instruments[i].notes) free(org->instruments[i].notes);
	}
	
	free(org);
}
