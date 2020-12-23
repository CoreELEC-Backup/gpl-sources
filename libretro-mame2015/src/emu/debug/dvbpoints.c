// license:BSD-3-Clause
// copyright-holders:Aaron Giles
/*********************************************************************

    dvbpoints.c

    Breakpoint debugger view.

***************************************************************************/

#include "emu.h"
#include "dvbpoints.h"



// Sorting functors for the qsort function
static int cIndexAscending(const void* a, const void* b)
{
	const device_debug::breakpoint* left = *(device_debug::breakpoint**)a;
	const device_debug::breakpoint* right = *(device_debug::breakpoint**)b;
	return left->index() - right->index();
}

static int cIndexDescending(const void* a, const void* b)
{
	return cIndexAscending(b, a);
}

static int cEnabledAscending(const void* a, const void* b)
{
	const device_debug::breakpoint* left = *(device_debug::breakpoint**)a;
	const device_debug::breakpoint* right = *(device_debug::breakpoint**)b;
	return (left->enabled() ? 1 : 0) - (right->enabled() ? 1 : 0);
}

static int cEnabledDescending(const void* a, const void* b)
{
	return cEnabledAscending(b, a);
}

static int cCpuAscending(const void* a, const void* b)
{
	const device_debug::breakpoint* left = *(device_debug::breakpoint**)a;
	const device_debug::breakpoint* right = *(device_debug::breakpoint**)b;
	return strcmp(left->debugInterface()->device().tag(), right->debugInterface()->device().tag());
}

static int cCpuDescending(const void* a, const void* b)
{
	return cCpuAscending(b, a);
}

static int cAddressAscending(const void* a, const void* b)
{
	const device_debug::breakpoint* left = *(device_debug::breakpoint**)a;
	const device_debug::breakpoint* right = *(device_debug::breakpoint**)b;
	return (left->address() > right->address()) ? 1 : (left->address() < right->address()) ? -1 : 0;
}

static int cAddressDescending(const void* a, const void* b)
{
	return cAddressAscending(b, a);
}

static int cConditionAscending(const void* a, const void* b)
{
	const device_debug::breakpoint* left = *(device_debug::breakpoint**)a;
	const device_debug::breakpoint* right = *(device_debug::breakpoint**)b;
	return strcmp(left->condition(), right->condition());
}

static int cConditionDescending(const void* a, const void* b)
{
	return cConditionAscending(b, a);
}

static int cActionAscending(const void* a, const void* b)
{
	const device_debug::breakpoint* left = *(device_debug::breakpoint**)a;
	const device_debug::breakpoint* right = *(device_debug::breakpoint**)b;
	return strcmp(left->action(), right->action());
}

static int cActionDescending(const void* a, const void* b)
{
	return cActionAscending(b, a);
}


//**************************************************************************
//  DEBUG VIEW BREAK POINTS
//**************************************************************************

static const int tableBreaks[] = { 5, 9, 31, 45, 63, 80 };


//-------------------------------------------------
//  debug_view_breakpoints - constructor
//-------------------------------------------------

debug_view_breakpoints::debug_view_breakpoints(running_machine &machine, debug_view_osd_update_func osdupdate, void *osdprivate)
	: debug_view(machine, DVT_BREAK_POINTS, osdupdate, osdprivate),
		m_sortType(cIndexAscending)
{
	// fail if no available sources
	enumerate_sources();
	if (m_source_list.count() == 0)
		throw std::bad_alloc();
}


//-------------------------------------------------
//  ~debug_view_breakpoints - destructor
//-------------------------------------------------

debug_view_breakpoints::~debug_view_breakpoints()
{
}


//-------------------------------------------------
//  enumerate_sources - enumerate all possible
//  sources for a disassembly view
//-------------------------------------------------

void debug_view_breakpoints::enumerate_sources()
{
	// start with an empty list
	m_source_list.reset();

	// iterate over devices with disassembly interfaces
	disasm_interface_iterator iter(machine().root_device());
	for (device_disasm_interface *dasm = iter.first(); dasm != NULL; dasm = iter.next())
	{
		astring name;
		name.printf("%s '%s'", dasm->device().name(), dasm->device().tag());
		m_source_list.append(*global_alloc(debug_view_source(name.cstr(), &dasm->device())));
	}

	// reset the source to a known good entry
	set_source(*m_source_list.first());
}


//-------------------------------------------------
//  view_click - handle a mouse click within the
//  current view
//-------------------------------------------------

void debug_view_breakpoints::view_click(const int button, const debug_view_xy& pos)
{
	bool clickedTopRow = (m_topleft.y == pos.y);

	if (clickedTopRow)
	{
		if (pos.x < tableBreaks[0])
			m_sortType = (m_sortType == &cIndexAscending) ? &cIndexDescending : &cIndexAscending;
		else if (pos.x < tableBreaks[1])
			m_sortType = (m_sortType == &cEnabledAscending) ? &cEnabledDescending : &cEnabledAscending;
		else if (pos.x < tableBreaks[2])
			m_sortType = (m_sortType == &cCpuAscending) ? &cCpuDescending : &cCpuAscending;
		else if (pos.x < tableBreaks[3])
			m_sortType = (m_sortType == &cAddressAscending) ? &cAddressDescending : &cAddressAscending;
		else if (pos.x < tableBreaks[4])
			m_sortType = (m_sortType == &cConditionAscending) ? &cConditionDescending : &cConditionAscending;
		else if (pos.x < tableBreaks[5])
			m_sortType = (m_sortType == &cActionAscending) ? &cActionDescending : &cActionAscending;
	}
	else
	{
		// Gather a sorted list of all the breakpoints for all the CPUs
		gather_breakpoints();

		int const bpIndex = pos.y - 1;
		if ((bpIndex >= m_buffer.count()) || (bpIndex < 0))
			return;

		// Enable / disable
		m_buffer[bpIndex]->setEnabled(!m_buffer[bpIndex]->enabled());

		machine().debug_view().update_all(DVT_DISASSEMBLY);
	}

	begin_update();
	m_update_pending = true;
	end_update();
}


void debug_view_breakpoints::pad_astring_to_length(astring& str, int len)
{
	int diff = len - str.len();
	if (diff > 0)
	{
		astring buffer;
		buffer.expand(diff);
		for (int i = 0; i < diff; i++)
			buffer.catprintf(" ");
		str.catprintf("%s", buffer.cstr());
	}
}


void debug_view_breakpoints::gather_breakpoints()
{
	m_buffer.resize(0);
	for (const debug_view_source *source = m_source_list.first(); source != NULL; source = source->next())
	{
		// Collect
		device_debug &debugInterface = *source->device()->debug();
		for (device_debug::breakpoint *bp = debugInterface.breakpoint_first(); bp != NULL; bp = bp->next())
			m_buffer.append() = bp;
	}

	// And now for the sort
	if (m_buffer.count() > 0)
		qsort(&m_buffer[0], m_buffer.count(), sizeof(device_debug::breakpoint *), m_sortType);
}


//-------------------------------------------------
//  view_update - update the contents of the
//  breakpoints view
//-------------------------------------------------

void debug_view_breakpoints::view_update()
{
	// Gather a list of all the breakpoints for all the CPUs
	gather_breakpoints();

	// Set the view region so the scroll bars update
	m_total.x = tableBreaks[ARRAY_LENGTH(tableBreaks) - 1];
	m_total.y = m_buffer.count() + 1;
	if (m_total.y < 10)
		m_total.y = 10;

	// Draw
	debug_view_char *dest = m_viewdata;
	astring         linebuf;

	// Header
	if (m_visible.y > 0)
	{
		linebuf.reset();
		linebuf.cat("ID");
		if (m_sortType == &cIndexAscending) linebuf.cat('\\');
		else if (m_sortType == &cIndexDescending) linebuf.cat('/');
		pad_astring_to_length(linebuf, tableBreaks[0]);
		linebuf.cat("En");
		if (m_sortType == &cEnabledAscending) linebuf.cat('\\');
		else if (m_sortType == &cEnabledDescending) linebuf.cat('/');
		pad_astring_to_length(linebuf, tableBreaks[1]);
		linebuf.cat("CPU");
		if (m_sortType == &cCpuAscending) linebuf.cat('\\');
		else if (m_sortType == &cCpuDescending) linebuf.cat('/');
		pad_astring_to_length(linebuf, tableBreaks[2]);
		linebuf.cat("Address");
		if (m_sortType == &cAddressAscending) linebuf.cat('\\');
		else if (m_sortType == &cAddressDescending) linebuf.cat('/');
		pad_astring_to_length(linebuf, tableBreaks[3]);
		linebuf.cat("Condition");
		if (m_sortType == &cConditionAscending) linebuf.cat('\\');
		else if (m_sortType == &cConditionDescending) linebuf.cat('/');
		pad_astring_to_length(linebuf, tableBreaks[4]);
		linebuf.cat("Action");
		if (m_sortType == &cActionAscending) linebuf.cat('\\');
		else if (m_sortType == &cActionDescending) linebuf.cat('/');
		pad_astring_to_length(linebuf, tableBreaks[5]);

		for (UINT32 i = m_topleft.x; i < (m_topleft.x + m_visible.x); i++, dest++)
		{
			dest->byte = (i < linebuf.len()) ? linebuf[i] : ' ';
			dest->attrib = DCA_ANCILLARY;
		}
	}

	for (int row = 1; row < m_visible.y; row++)
	{
		// Breakpoints
		int bpi = row + m_topleft.y - 1;
		if ((bpi < m_buffer.count()) && (bpi >= 0))
		{
			device_debug::breakpoint *const bp = m_buffer[bpi];

			linebuf.reset();
			linebuf.catprintf("%2X", bp->index());
			pad_astring_to_length(linebuf, tableBreaks[0]);
			linebuf.cat(bp->enabled() ? 'X' : 'O');
			pad_astring_to_length(linebuf, tableBreaks[1]);
			linebuf.cat(bp->debugInterface()->device().tag());
			pad_astring_to_length(linebuf, tableBreaks[2]);
			linebuf.cat(core_i64_hex_format(bp->address(), bp->debugInterface()->logaddrchars()));
			pad_astring_to_length(linebuf, tableBreaks[3]);
			if (strcmp(bp->condition(), "1"))
				linebuf.cat(bp->condition());
			pad_astring_to_length(linebuf, tableBreaks[4]);
			linebuf.cat(bp->action());
			pad_astring_to_length(linebuf, tableBreaks[5]);

			for (UINT32 i = m_topleft.x; i < (m_topleft.x + m_visible.x); i++, dest++)
			{
				dest->byte = (i < linebuf.len()) ? linebuf[i] : ' ';
				dest->attrib = DCA_NORMAL;

				// Color disabled breakpoints red
				if ((i >= tableBreaks[0]) && (i < tableBreaks[1]) && !bp->enabled())
					dest->attrib |= DCA_CHANGED;
			}
		}
		else
		{
			// Fill the remaining vertical space
			for (UINT32 i = m_topleft.x; i < (m_topleft.x + m_visible.x); i++, dest++)
			{
				dest->byte = ' ';
				dest->attrib = DCA_NORMAL;
			}
		}
	}
}
