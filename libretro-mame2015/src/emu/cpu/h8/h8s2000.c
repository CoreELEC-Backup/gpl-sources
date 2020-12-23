#include "emu.h"
#include "h8s2000.h"

h8s2000_device::h8s2000_device(const machine_config &mconfig, device_type type, const char *name, const char *tag, device_t *owner, UINT32 clock, const char *shortname, const char *source, address_map_delegate map_delegate) :
	h8h_device(mconfig, type, name, tag, owner, clock, shortname, source, map_delegate)
{
	has_exr = true;
}

offs_t h8s2000_device::disasm_disassemble(char *buffer, offs_t pc, const UINT8 *oprom, const UINT8 *opram, UINT32 options)
{
	return disassemble_generic(buffer, pc, oprom, opram, options, disasm_entries);
}

#include "cpu/h8/h8s2000.inc"
