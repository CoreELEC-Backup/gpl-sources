#include "vrc4373.h"

#define LOG_NILE            (1)
#define LOG_NILE_MASTER     (0)
#define LOG_NILE_TARGET     (1)

const device_type VRC4373      = &device_creator<vrc4373_device>;

DEVICE_ADDRESS_MAP_START(config_map, 32, vrc4373_device)
	AM_RANGE(0x40, 0x43) AM_READWRITE  (pcictrl_r,  pcictrl_w)
	AM_INHERIT_FROM(pci_host_device::config_map)
ADDRESS_MAP_END

// cpu i/f map
DEVICE_ADDRESS_MAP_START(cpu_map, 32, vrc4373_device)
	AM_RANGE(0x00000000, 0x0000007b) AM_READWRITE(    vrc4373_device::cpu_if_r,          vrc4373_device::cpu_if_w)
ADDRESS_MAP_END

// Target Window 1 map
DEVICE_ADDRESS_MAP_START(target1_map, 32, vrc4373_device)
	AM_RANGE(0x00000000, 0xFFFFFFFF) AM_READWRITE(    vrc4373_device::target1_r,          vrc4373_device::target1_w)
ADDRESS_MAP_END

// Target Window 2 map
DEVICE_ADDRESS_MAP_START(target2_map, 32, vrc4373_device)
	AM_RANGE(0x00000000, 0xFFFFFFFF) AM_READWRITE(    vrc4373_device::target2_r,          vrc4373_device::target2_w)
ADDRESS_MAP_END

vrc4373_device::vrc4373_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock)
	: pci_host_device(mconfig, VRC4373, "NEC VRC4373 System Controller", tag, owner, clock, "vrc4373", __FILE__),
		m_mem_config("memory_space", ENDIANNESS_LITTLE, 32, 32),
		m_io_config("io_space", ENDIANNESS_LITTLE, 32, 32)
{
}

const address_space_config *vrc4373_device::memory_space_config(address_spacenum spacenum) const
{
	return (spacenum == AS_PROGRAM) ? pci_bridge_device::memory_space_config(spacenum) : (spacenum == AS_DATA) ? &m_mem_config : (spacenum == AS_IO) ? &m_io_config : NULL;
}

void vrc4373_device::device_start()
{
	pci_host_device::device_start();
	m_cpu = machine().device<cpu_device>(cpu_tag);
	m_cpu_space = &m_cpu->space(AS_PROGRAM);
	memory_space = &space(AS_DATA);
	io_space = &space(AS_IO);

	memset(m_cpu_regs, 0, sizeof(m_cpu_regs));

	memory_window_start = 0;
	memory_window_end   = 0xffffffff;
	memory_offset       = 0;
	io_window_start = 0;
	io_window_end   = 0xffffffff;
	io_offset       = 0x00000000;
	status = 0x0280;
	m_ram_size = 1<<22;
	m_ram_base = 0;
	m_simm_size = 1<<21;
	m_simm_base = 0;
	regenerate_config_mapping();
}

void vrc4373_device::device_reset()
{
	pci_device::device_reset();
	memset(m_cpu_regs, 0, sizeof(m_cpu_regs));
	remap_cb();
}

void vrc4373_device::map_extra(UINT64 memory_window_start, UINT64 memory_window_end, UINT64 memory_offset, address_space *memory_space,
									UINT64 io_window_start, UINT64 io_window_end, UINT64 io_offset, address_space *io_space)
{
	m_cpu_space->unmap_readwrite(0x00000000, 0xffffffff);

	m_cpu_space->install_rom   (0x1fc00000, 0x1fcfffff, m_region->base());
	m_cpu_space->install_device(0x0f000000, 0x0f0000ff, *static_cast<vrc4373_device *>(this), &vrc4373_device::cpu_map);
	// PCI Configuration also mapped at 0x0f000100
	m_cpu_space->install_device(0x0f000100, 0x0f0001ff, *static_cast<vrc4373_device *>(this), &vrc4373_device::config_map);

	UINT32 winStart, winEnd, winSize;

	if (m_cpu_regs[NREG_BMCR]&0x8) {
		m_cpu_space->install_ram      (m_ram_base, m_ram_base+m_ram_size-1, &m_ram[0]);
		if (LOG_NILE)
			logerror("%s: map_extra ram_size=%08X ram_base=%08X\n", tag(),m_ram_size,m_ram_base);
	}
	if (m_cpu_regs[NREG_SIMM1]&0x8) {
		m_cpu_space->install_ram      (m_simm_base, m_simm_base+m_simm_size-1, &m_simm[0]);
		if (LOG_NILE)
			logerror("%s: map_extra simm_size=%08X simm_base=%08X\n", tag(),m_simm_size,m_simm_base);
	}
	// PCI Master Window 1
	if (m_cpu_regs[NREG_PCIMW1]&0x1000) {
		winStart = m_cpu_regs[NREG_PCIMW1]&0xff000000;
		winEnd = winStart | (~(0x80000000 | (((m_cpu_regs[NREG_PCIMW1]>>13)&0x7f)<<24)));
		winSize = winEnd - winStart + 1;
		m_cpu_space->install_read_handler(winStart, winEnd, 0, 0, read32_delegate(FUNC(vrc4373_device::master1_r), this));
		m_cpu_space->install_write_handler(winStart, winEnd, 0, 0, write32_delegate(FUNC(vrc4373_device::master1_w), this));
		if (LOG_NILE)
			logerror("%s: map_extra Master Window 1 start=%08X end=%08X size=%08X laddr=%08X\n", tag(), winStart, winEnd, winSize,  m_pci1_laddr);
	}
	// PCI Master Window 2
	if (m_cpu_regs[NREG_PCIMW2]&0x1000) {
		winStart = m_cpu_regs[NREG_PCIMW2]&0xff000000;
		winEnd = winStart | (~(0x80000000 | (((m_cpu_regs[NREG_PCIMW2]>>13)&0x7f)<<24)));
		winSize = winEnd - winStart + 1;
		m_cpu_space->install_read_handler(winStart, winEnd, 0, 0, read32_delegate(FUNC(vrc4373_device::master2_r), this));
		m_cpu_space->install_write_handler(winStart, winEnd, 0, 0, write32_delegate(FUNC(vrc4373_device::master2_w), this));
		if (LOG_NILE)
			logerror("%s: map_extra Master Window 2 start=%08X end=%08X size=%08X laddr=%08X\n", tag(), winStart, winEnd, winSize,  m_pci2_laddr);
	}
	// PCI IO Window 
	if (m_cpu_regs[NREG_PCIMIOW]&0x1000) {
		winStart = m_cpu_regs[NREG_PCIMIOW]&0xff000000;
		winEnd = winStart | (~(0x80000000 | (((m_cpu_regs[NREG_PCIMIOW]>>13)&0x7f)<<24)));
		winSize = winEnd - winStart + 1;
		m_cpu_space->install_read_handler(winStart, winEnd, 0, 0, read32_delegate(FUNC(vrc4373_device::master_io_r), this));
		m_cpu_space->install_write_handler(winStart, winEnd, 0, 0, write32_delegate(FUNC(vrc4373_device::master_io_w), this));
		if (LOG_NILE)
			logerror("%s: map_extra IO Window start=%08X end=%08X size=%08X laddr=%08X\n", tag(), winStart, winEnd, winSize,  m_pci_io_laddr);
	}
	// PCI Target Window 1
	if (m_cpu_regs[NREG_PCITW1]&0x1000) {		
		winStart = m_cpu_regs[NREG_PCITW1]&0xffe00000;
		winEnd = winStart | (~(0xf0000000 | (((m_cpu_regs[NREG_PCITW1]>>13)&0x7f)<<21)));
		winSize = winEnd - winStart + 1;
		memory_space->install_read_handler(winStart, winEnd, 0, 0, read32_delegate(FUNC(vrc4373_device::target1_r), this));
		memory_space->install_write_handler(winStart, winEnd, 0, 0, write32_delegate(FUNC(vrc4373_device::target1_w), this));
		if (LOG_NILE)
			logerror("%s: map_extra Target Window 1 start=%08X end=%08X size=%08X laddr=%08X\n", tag(), winStart, winEnd, winSize,  m_target1_laddr);
	}
	// PCI Target Window 2
	if (m_cpu_regs[NREG_PCITW2]&0x1000) {
		winStart = m_cpu_regs[NREG_PCITW2]&0xffe00000;
		winEnd = winStart | (~(0xf0000000 | (((m_cpu_regs[NREG_PCITW2]>>13)&0x7f)<<21)));
		winSize = winEnd - winStart + 1;
		memory_space->install_read_handler(winStart, winEnd, 0, 0, read32_delegate(FUNC(vrc4373_device::target2_r), this));
		memory_space->install_write_handler(winStart, winEnd, 0, 0, write32_delegate(FUNC(vrc4373_device::target2_w), this));
		if (LOG_NILE)
			logerror("%s: map_extra Target Window 2 start=%08X end=%08X size=%08X laddr=%08X\n", tag(), winStart, winEnd, winSize,  m_target2_laddr);
	}
}

void vrc4373_device::reset_all_mappings()
{
	pci_device::reset_all_mappings();
}

void vrc4373_device::set_cpu_tag(const char *_cpu_tag)
{
	if (LOG_NILE)
		logerror("%s: set_cpu_tag\n", tag());
	cpu_tag = _cpu_tag;
}
// PCI bus control
READ32_MEMBER (vrc4373_device::pcictrl_r)
{
	UINT32 result = 0;
	if (LOG_NILE)
		logerror("%06X:nile pcictrl_r from offset %02X = %08X & %08X\n", space.device().safe_pc(), offset*4, result, mem_mask);
	return result;
}
WRITE32_MEMBER (vrc4373_device::pcictrl_w)
{
	if (LOG_NILE)
		logerror("%06X:nile pcictrl_w to offset %02X = %08X & %08X\n", space.device().safe_pc(), offset*4, data, mem_mask);
}
// PCI Master Window 1
READ32_MEMBER (vrc4373_device::master1_r)
{
	UINT32 result = this->space(AS_DATA).read_dword(m_pci1_laddr | (offset*4), mem_mask);
	if (LOG_NILE_MASTER)
		logerror("%06X:nile master1 read from offset %02X = %08X & %08X\n", space.device().safe_pc(), offset*4, result, mem_mask);
	return result;
}
WRITE32_MEMBER (vrc4373_device::master1_w)
{
	this->space(AS_DATA).write_dword(m_pci1_laddr | (offset*4), data, mem_mask);
	if (LOG_NILE_MASTER)
		logerror("%06X:nile master1 write to offset %02X = %08X & %08X\n", space.device().safe_pc(), offset*4, data, mem_mask);
}

// PCI Master Window 2
READ32_MEMBER (vrc4373_device::master2_r)
{
	UINT32 result = this->space(AS_DATA).read_dword(m_pci2_laddr | (offset*4), mem_mask);
	if (LOG_NILE_MASTER)
		logerror("%06X:nile master2 read from offset %02X = %08X & %08X\n", space.device().safe_pc(), offset*4, result, mem_mask);
	return result;
}
WRITE32_MEMBER (vrc4373_device::master2_w)
{
	this->space(AS_DATA).write_dword(m_pci2_laddr | (offset*4), data, mem_mask);
	if (LOG_NILE_MASTER)
		logerror("%06X:nile master2 write to offset %02X = %08X & %08X\n", space.device().safe_pc(), offset*4, data, mem_mask);
}

// PCI Master IO Window
READ32_MEMBER (vrc4373_device::master_io_r)
{
	UINT32 result = this->space(AS_IO).read_dword(m_pci_io_laddr | (offset*4), mem_mask);
	if (LOG_NILE_MASTER)
		logerror("%06X:nile master io read from offset %02X = %08X & %08X\n", space.device().safe_pc(), offset*4, result, mem_mask);
	return result;
}
WRITE32_MEMBER (vrc4373_device::master_io_w)
{
	this->space(AS_IO).write_dword(m_pci_io_laddr | (offset*4), data, mem_mask);
	if (LOG_NILE_MASTER)
		logerror("%06X:nile master io write to offset %02X = %08X & %08X\n", space.device().safe_pc(), offset*4, data, mem_mask);
}

// PCI Target Window 1
READ32_MEMBER (vrc4373_device::target1_r)
{
	UINT32 result = m_cpu->space(AS_PROGRAM).read_dword(m_target1_laddr | (offset*4), mem_mask);
	if (LOG_NILE_TARGET)
		logerror("%06X:nile target1 read from offset %02X = %08X & %08X\n", space.device().safe_pc(), offset*4, result, mem_mask);
	return result;
}
WRITE32_MEMBER (vrc4373_device::target1_w)
{
	m_cpu->space(AS_PROGRAM).write_dword(m_target1_laddr | (offset*4), data, mem_mask);
	if (LOG_NILE_TARGET)
		logerror("%06X:nile target1 write to offset %02X = %08X & %08X\n", space.device().safe_pc(), offset*4, data, mem_mask);
}

// PCI Target Window 2
READ32_MEMBER (vrc4373_device::target2_r)
{
	UINT32 result = m_cpu->space(AS_PROGRAM).read_dword(m_target2_laddr | (offset*4), mem_mask);
	if (LOG_NILE_TARGET)
		logerror("%06X:nile target2 read from offset %02X = %08X & %08X\n", space.device().safe_pc(), offset*4, result, mem_mask);
	return result;
}
WRITE32_MEMBER (vrc4373_device::target2_w)
{
	m_cpu->space(AS_PROGRAM).write_dword(m_target2_laddr | (offset*4), data, mem_mask);
	if (LOG_NILE_TARGET)
		logerror("%06X:nile target2 write to offset %02X = %08X & %08X\n", space.device().safe_pc(), offset*4, data, mem_mask);
}

// CPU I/F 
READ32_MEMBER (vrc4373_device::cpu_if_r)
{
	UINT32 result = m_cpu_regs[offset];
	switch (offset) {
		case NREG_PCICAR:
			result = config_address_r(space, offset);
			break;
		case NREG_PCICDR:
			result = config_data_r(space, offset);
			break;
		default:
			break;
	}
	if (LOG_NILE)
		logerror("%06X:nile read from offset %02X = %08X & %08X\n", space.device().safe_pc(), offset*4, result, mem_mask);
	return result;
}

WRITE32_MEMBER(vrc4373_device::cpu_if_w)
{
	if (LOG_NILE)
		logerror("%06X:nile write to offset %02X = %08X & %08X\n", space.device().safe_pc(), offset*4, data, mem_mask);

	UINT32 modData;
	COMBINE_DATA(&m_cpu_regs[offset]);
	switch (offset) {
		case NREG_PCIMW1:
				m_pci1_laddr = (data&0xff)<<24;
				remap_cb();
			break;
		case NREG_PCIMW2:
				m_pci2_laddr = (data&0xff)<<24;
				remap_cb();
			break;
		case NREG_PCIMIOW:
				m_pci_io_laddr = (data&0xff)<<24;
				remap_cb();
			break;
		case NREG_PCITW1:
				m_target1_laddr = (data&0x7FF)<<21;
			break;
		case NREG_PCITW2:
				m_target2_laddr = (data&0x7FF)<<21;
			break;
		case NREG_PCICAR:
			// Bits in reserved area are used for device selection of type 0 config transactions
			// Assuming 23:11 get mapped into device number for configuration
			if ((data&0x3) == 0x0) { 
				// Type 0 transaction
				modData = 0;
				// Select the device based on one hot bit
				for (int i=11; i<24; i++) {
					if ((data>>i)&0x1) {
						// One hot encoding, bit 11 will mean device 1
						modData = i-10;
						break;
					}
				}
				// Re-organize into Type 1 transaction for bus 0 (local bus)
				modData = (modData<<11) | (data&0x7ff) | (0x80000000);
			} else {
				// Type 1 transaction, no modification needed
				modData = data;
			}
			pci_host_device::config_address_w(space, offset, modData);
			break;
		case NREG_PCICDR:
			pci_host_device::config_data_w(space, offset, data);
			break;
		case NREG_BMCR:
			if ((data>>3)&0x1) {
				m_ram_size = 1<<22;  // 4MB
				for (int i=14; i<=15; i++) {
					if (!((data>>i)&0x1)) m_ram_size<<=1;
					else break;
				}
				m_ram.resize(m_ram_size/4);
				m_ram_base = (data & 0x0fc00000);
			}
			remap_cb();
			break;
		case NREG_SIMM1:
			if ((data>>3)&0x1) {
				m_simm_size = 1<<21;  // 2MB
				for (int i=13; i<=17; i++) {
					if (!((data>>i)&0x1)) m_simm_size<<=1;
					else break;
				}
				m_simm.resize(m_simm_size/4);
				m_simm_base = (data & 0x0fe00000);
			}
			remap_cb();
			break;
		default:
			break;
	}

}

