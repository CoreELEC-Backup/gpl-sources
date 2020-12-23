/***************************************************************************

  Sony Playstaion
  ===============
  Preliminary driver by smf
  Additional development by pSXAuthor and R. Belmont

***************************************************************************/

#include "emu.h"
#include "cpu/psx/psx.h"
#include "cpu/m6805/m6805.h"
#include "video/psx.h"
#include "imagedev/snapquik.h"
#include "imagedev/chd_cd.h"
#include "sound/spu.h"
#include "debugger.h"
#include <zlib.h>
#include "machine/psxcd.h"
#include "bus/psx/ctlrport.h"

#define PSXCD_TAG   "psxcd"

class psx1_state : public driver_device
{
public:
	psx1_state(const machine_config &mconfig, device_type type, const char *tag)
		: driver_device(mconfig, type, tag) ,
		m_maincpu(*this, "maincpu"),
		m_ram(*this, "maincpu:ram")
	{
	}

	UINT8 *m_exe_buffer;
	int m_exe_size;
	int m_cd_param_p;
	int m_cd_result_p;
	int m_cd_result_c;
	int m_cd_result_ready;
	int m_cd_reset;
	UINT8 m_cd_stat;
	UINT8 m_cd_io_status;
	UINT8 m_cd_param[8];
	UINT8 m_cd_result[8];
	DECLARE_DIRECT_UPDATE_MEMBER(psx_default);
	DECLARE_DIRECT_UPDATE_MEMBER(psx_setopbase);
	DECLARE_MACHINE_RESET(psx);
	inline void ATTR_PRINTF(3,4) verboselog( int n_level, const char *s_fmt, ... );
	void psxexe_conv32( UINT32 *p_uint32 );
	int load_psxexe( cpu_device *cpu, unsigned char *p_n_file, int n_len );
	void cpe_set_register( cpu_device *cpu, int n_reg, int n_value );
	int load_cpe( cpu_device *cpu, unsigned char *p_n_file, int n_len );
	int load_psf( cpu_device *cpu, unsigned char *p_n_file, int n_len );
	void cd_dma_read( UINT32 *p_n_psxram, UINT32 n_address, INT32 n_size );
	void cd_dma_write( UINT32 *p_n_psxram, UINT32 n_address, INT32 n_size );
	DECLARE_QUICKLOAD_LOAD_MEMBER( psx_exe_load );
	required_device<psxcpu_device> m_maincpu;
	required_device<ram_device> m_ram;
};


#define VERBOSE_LEVEL ( 0 )

inline void ATTR_PRINTF(3,4)  psx1_state::verboselog( int n_level, const char *s_fmt, ... )
{
	if( VERBOSE_LEVEL >= n_level )
	{
		va_list v;
		char buf[ 32768 ];
		va_start( v, s_fmt );
		vsprintf( buf, s_fmt, v );
		va_end( v );
		logerror( "%s: %s", machine().describe_context(), buf );
	}
}


void psx1_state::psxexe_conv32( UINT32 *p_uint32 )
{
	UINT8 *p_uint8;

	p_uint8 = (UINT8 *)p_uint32;

	*( p_uint32 ) = p_uint8[ 0 ] |
		( p_uint8[ 1 ] << 8 ) |
		( p_uint8[ 2 ] << 16 ) |
		( p_uint8[ 3 ] << 24 );
}

int psx1_state::load_psxexe( cpu_device *cpu, unsigned char *p_n_file, int n_len )
{
	struct PSXEXE_HEADER
	{
		UINT8 id[ 8 ];
		UINT32 text;    /* SCE only */
		UINT32 data;    /* SCE only */
		UINT32 pc0;
		UINT32 gp0;     /* SCE only */
		UINT32 t_addr;
		UINT32 t_size;
		UINT32 d_addr;  /* SCE only */
		UINT32 d_size;  /* SCE only */
		UINT32 b_addr;  /* SCE only */
		UINT32 b_size;  /* SCE only */
		UINT32 s_addr;
		UINT32 s_size;
		UINT32 SavedSP;
		UINT32 SavedFP;
		UINT32 SavedGP;
		UINT32 SavedRA;
		UINT32 SavedS0;
		UINT8 dummy[ 0x800 - 76 ];
	};

	struct PSXEXE_HEADER *psxexe_header = (struct PSXEXE_HEADER *)p_n_file;

	if( n_len >= sizeof( struct PSXEXE_HEADER ) &&
		memcmp( psxexe_header->id, "PS-X EXE", 8 ) == 0 )
	{
		psxexe_conv32( &psxexe_header->text );
		psxexe_conv32( &psxexe_header->data );
		psxexe_conv32( &psxexe_header->pc0 );
		psxexe_conv32( &psxexe_header->gp0 );
		psxexe_conv32( &psxexe_header->t_addr );
		psxexe_conv32( &psxexe_header->t_size );
		psxexe_conv32( &psxexe_header->d_addr );
		psxexe_conv32( &psxexe_header->d_size );
		psxexe_conv32( &psxexe_header->b_addr );
		psxexe_conv32( &psxexe_header->b_size );
		psxexe_conv32( &psxexe_header->s_addr );
		psxexe_conv32( &psxexe_header->s_size );
		psxexe_conv32( &psxexe_header->SavedSP );
		psxexe_conv32( &psxexe_header->SavedFP );
		psxexe_conv32( &psxexe_header->SavedGP );
		psxexe_conv32( &psxexe_header->SavedRA );
		psxexe_conv32( &psxexe_header->SavedS0 );

		/* todo: check size.. */

		logerror( "psx_exe_load: pc    %08x\n", psxexe_header->pc0 );
		logerror( "psx_exe_load: org   %08x\n", psxexe_header->t_addr );
		logerror( "psx_exe_load: len   %08x\n", psxexe_header->t_size );
		logerror( "psx_exe_load: sp    %08x\n", psxexe_header->s_addr );
		logerror( "psx_exe_load: len   %08x\n", psxexe_header->s_size );

		UINT8 *p_ram = m_ram->pointer();
		UINT32 n_ram = m_ram->size();

		UINT8 *p_psxexe = p_n_file + sizeof( struct PSXEXE_HEADER );

		UINT32 n_address = psxexe_header->t_addr;
		UINT32 n_size = psxexe_header->t_size;
		while( n_size != 0 )
		{
			p_ram[ BYTE4_XOR_LE( n_address ) % n_ram ] = *( p_psxexe );
			n_address++;
			p_psxexe++;
			n_size--;
		}

		cpu->set_state_int( PSXCPU_PC, psxexe_header->pc0 );
		cpu->set_state_int( PSXCPU_R28, psxexe_header->gp0 );
		UINT32 n_stack = psxexe_header->s_addr + psxexe_header->s_size;
		if( n_stack != 0 )
		{
			cpu->set_state_int( PSXCPU_R29, n_stack );
			cpu->set_state_int( PSXCPU_R30, n_stack );
		}

		return 1;
	}
	return 0;
}

void psx1_state::cpe_set_register( cpu_device *cpu, int n_reg, int n_value )
{
	if( n_reg < 0x80 && ( n_reg % 4 ) == 0 )
	{
		logerror( "psx_exe_load: r%-2d   %08x\n", n_reg / 4, n_value );
		cpu->set_state_int( PSXCPU_R0 + ( n_reg / 4 ), n_value );
	}
	else if( n_reg == 0x80 )
	{
		logerror( "psx_exe_load: lo    %08x\n", n_value );
		cpu->set_state_int( PSXCPU_LO, n_value );
	}
	else if( n_reg == 0x84 )
	{
		logerror( "psx_exe_load: hi    %08x\n", n_value );
		cpu->set_state_int( PSXCPU_HI, n_value );
	}
	else if( n_reg == 0x88 )
	{
		logerror( "psx_exe_load: sr    %08x\n", n_value );
		cpu->set_state_int( PSXCPU_CP0R12, n_value );
	}
	else if( n_reg == 0x8c )
	{
		logerror( "psx_exe_load: cause %08x\n", n_value );
		cpu->set_state_int( PSXCPU_CP0R13, n_value );
	}
	else if( n_reg == 0x90 )
	{
		logerror( "psx_exe_load: pc    %08x\n", n_value );
		cpu->set_state_int( PSXCPU_PC, n_value );
	}
	else if( n_reg == 0x94 )
	{
		logerror( "psx_exe_load: prid  %08x\n", n_value );
		cpu->set_state_int( PSXCPU_CP0R15, n_value );
	}
	else
	{
		logerror( "psx_exe_load: invalid register %04x/%08x\n", n_reg, n_value );
	}
}

int psx1_state::load_cpe( cpu_device *cpu, unsigned char *p_n_file, int n_len )
{
	if( n_len >= 4 &&
		memcmp( p_n_file, "CPE\001", 4 ) == 0 )
	{
		int n_offset = 4;

		for( ;; )
		{
			if( n_offset >= n_len || p_n_file[ n_offset ] > 8 )
			{
				break;
			}

			switch( p_n_file[ n_offset++ ] )
			{
			case 0:
				/* end of file */
				return 1;
			case 1:
				/* read bytes */
				{
					int n_address = ( (int)p_n_file[ n_offset + 0 ] << 0 ) |
						( (int)p_n_file[ n_offset + 1 ] << 8 ) |
						( (int)p_n_file[ n_offset + 2 ] << 16 ) |
						( (int)p_n_file[ n_offset + 3 ] << 24 );
					int n_size = ( (int)p_n_file[ n_offset + 4 ] << 0 ) |
						( (int)p_n_file[ n_offset + 5 ] << 8 ) |
						( (int)p_n_file[ n_offset + 6 ] << 16 ) |
						( (int)p_n_file[ n_offset + 7 ] << 24 );

					UINT8 *p_ram = m_ram->pointer();
					UINT32 n_ram = m_ram->size();

					n_offset += 8;

					logerror( "psx_exe_load: org   %08x\n", n_address );
					logerror( "psx_exe_load: len   %08x\n", n_size );

					while( n_size > 0 )
					{
						p_ram[ BYTE4_XOR_LE( n_address ) % n_ram ] = p_n_file[ n_offset++ ];
						n_address++;
						n_size--;
					}
					break;
				}
			case 2:
				/* run address: not tested */
				{
					int n_value = ( (int)p_n_file[ n_offset + 2 ] << 0 ) |
						( (int)p_n_file[ n_offset + 3 ] << 8 ) |
						( (int)p_n_file[ n_offset + 4 ] << 16 ) |
						( (int)p_n_file[ n_offset + 5 ] << 24 );

					n_offset += 4;

					cpe_set_register( cpu, 0x90, n_value );
					break;
				}
			case 3:
				/* set reg to longword */
				{
					int n_reg = ( (int)p_n_file[ n_offset + 0 ] << 0 ) |
						( (int)p_n_file[ n_offset + 1 ] << 8 );
					int n_value = ( (int)p_n_file[ n_offset + 2 ] << 0 ) |
						( (int)p_n_file[ n_offset + 3 ] << 8 ) |
						( (int)p_n_file[ n_offset + 4 ] << 16 ) |
						( (int)p_n_file[ n_offset + 5 ] << 24 );

					n_offset += 6;

					cpe_set_register( cpu, n_reg, n_value );
					break;
				}
			case 4:
				/* set reg to word: not tested */
				{
					int n_reg = ( (int)p_n_file[ n_offset + 0 ] << 0 ) |
						( (int)p_n_file[ n_offset + 1 ] << 8 );
					int n_value = ( (int)p_n_file[ n_offset + 2 ] << 0 ) |
						( (int)p_n_file[ n_offset + 3 ] << 8 );

					n_offset += 4;

					cpe_set_register( cpu, n_reg, n_value );
					break;
				}
			case 5:
				/* set reg to byte: not tested */
				{
					int n_reg = ( (int)p_n_file[ n_offset + 0 ] << 0 ) |
						( (int)p_n_file[ n_offset + 1 ] << 8 );
					int n_value = ( (int)p_n_file[ n_offset + 2 ] << 0 );

					n_offset += 3;

					cpe_set_register( cpu, n_reg, n_value );
					break;
				}
			case 6:
				/* set reg to 3-byte: not tested */
				{
					int n_reg = ( (int)p_n_file[ n_offset + 0 ] << 0 ) |
						( (int)p_n_file[ n_offset + 1 ] << 8 );
					int n_value = ( (int)p_n_file[ n_offset + 2 ] << 0 ) |
						( (int)p_n_file[ n_offset + 3 ] << 8 ) |
						( (int)p_n_file[ n_offset + 4 ] << 16 );

					n_offset += 5;

					cpe_set_register( cpu, n_reg, n_value );
					break;
				}
			case 7:
				/* workspace: not tested */
				n_offset += 4;
				break;
			case 8:
				/* unit */
				{
					int n_unit = p_n_file[ n_offset + 0 ];

					n_offset++;

					logerror( "psx_exe_load: unit  %08x\n", n_unit );
				}
				break;
			}
		}
	}
	return 0;
}

int psx1_state::load_psf( cpu_device *cpu, unsigned char *p_n_file, int n_len )
{
	int n_return;
	unsigned long n_crc;
	unsigned long n_compressed;
	unsigned char *p_n_compressed;
	unsigned long n_uncompressed;
	dynamic_buffer p_n_uncompressed;

	struct PSF_HEADER
	{
		unsigned char id[ 4 ];
		UINT32 reserved_size;
		UINT32 exe_size;
		UINT32 exe_crc;
	};

	struct PSF_HEADER *psf_header = (struct PSF_HEADER *)p_n_file;

	n_return = 0;

	if( n_len >= sizeof( struct PSF_HEADER ) &&
		memcmp( p_n_file, "PSF", 3 ) == 0 )
	{
		psxexe_conv32( &psf_header->reserved_size );
		psxexe_conv32( &psf_header->exe_size );
		psxexe_conv32( &psf_header->exe_crc );

		logerror( "psx_exe_load: reserved_size %08x\n", psf_header->reserved_size );
		logerror( "psx_exe_load: exe_size      %08x\n", psf_header->exe_size );
		logerror( "psx_exe_load: exe_crc       %08x\n", psf_header->exe_crc );

		n_compressed = psf_header->exe_size;
		p_n_compressed = p_n_file + sizeof( struct PSF_HEADER ) + psf_header->reserved_size;

		n_crc = crc32( crc32( 0L, Z_NULL, 0 ), p_n_compressed, n_compressed );
		if( n_crc != psf_header->exe_crc )
		{
			logerror( "psx_exe_load: psf invalid crc\n" );
			return 0;
		}

		n_uncompressed = 0x200000;
		p_n_uncompressed.resize( n_uncompressed );

		if( uncompress( p_n_uncompressed, &n_uncompressed, p_n_compressed, n_compressed ) != Z_OK )
		{
			logerror( "psx_exe_load: psf uncompress failed\n" );
		}
		else if( !load_psxexe( cpu, p_n_uncompressed, n_uncompressed ) )
		{
			logerror( "psx_exe_load: psf load failed\n" );
		}
		else
		{
			n_return = 1;
		}
	}
	return n_return;
}

DIRECT_UPDATE_MEMBER(psx1_state::psx_default)
{
	return address;
}

DIRECT_UPDATE_MEMBER(psx1_state::psx_setopbase)
{
	if( address == 0x80030000 )
	{
		m_maincpu->space(AS_PROGRAM).set_direct_update_handler(direct_update_delegate(FUNC(psx1_state::psx_default), this));

		if( load_psxexe( m_maincpu, m_exe_buffer, m_exe_size ) ||
			load_cpe( m_maincpu, m_exe_buffer, m_exe_size ) ||
			load_psf( m_maincpu, m_exe_buffer, m_exe_size ) )
		{
/*          DEBUGGER_BREAK; */

			address = m_maincpu->state_int( PSXCPU_PC );
			m_maincpu->set_state_int( PSXCPU_DELAYR, PSXCPU_DELAYR_PC );
			m_maincpu->set_state_int( PSXCPU_DELAYV, address );
		}
		else
		{
			logerror( "psx_exe_load: invalid exe\n" );
		}

		m_exe_size = 0;
		global_free_array( m_exe_buffer );
	}
	return address;
}

QUICKLOAD_LOAD_MEMBER( psx1_state, psx_exe_load )
{
	address_space &space = m_maincpu->space( AS_PROGRAM );

	m_exe_size = 0;
	m_exe_buffer = global_alloc_array( UINT8, quickload_size );
	if( m_exe_buffer == NULL )
	{
		logerror( "psx_exe_load: out of memory\n" );
		return IMAGE_INIT_FAIL;
	}
	if( image.fread( m_exe_buffer, quickload_size ) != quickload_size )
	{
		global_free_array( m_exe_buffer );
		return IMAGE_INIT_FAIL;
	}
	m_exe_size = quickload_size;
	space.set_direct_update_handler(direct_update_delegate(FUNC(psx1_state::psx_setopbase), this));

	return IMAGE_INIT_PASS;
}

/* ----------------------------------------------------------------------- */

void psx1_state::cd_dma_read( UINT32 *p_n_psxram, UINT32 n_address, INT32 n_size )
{
	UINT8 *psxram = (UINT8 *) p_n_psxram;
	psxcd_device *psxcd = machine().device<psxcd_device>(PSXCD_TAG);
	psxcd->start_dma(psxram + n_address, n_size*4);
}

void psx1_state::cd_dma_write( UINT32 *p_n_psxram, UINT32 n_address, INT32 n_size )
{
	printf("cd_dma_write?!: addr %x, size %x\n", n_address, n_size);
}

static ADDRESS_MAP_START( psx_map, AS_PROGRAM, 32, psx1_state )
ADDRESS_MAP_END

static ADDRESS_MAP_START( subcpu_map, AS_PROGRAM, 8, psx1_state )
	AM_RANGE(0x0000, 0xffff) AM_ROM
ADDRESS_MAP_END

static MACHINE_CONFIG_START( psj, psx1_state )
	/* basic machine hardware */
	MCFG_CPU_ADD( "maincpu", CXD8530CQ, XTAL_67_7376MHz )
	MCFG_CPU_PROGRAM_MAP( psx_map )

	MCFG_RAM_MODIFY("maincpu:ram")
	MCFG_RAM_DEFAULT_SIZE("2M")

	MCFG_DEVICE_ADD("controllers", PSXCONTROLLERPORTS, 0)
	MCFG_PSX_CONTROLLER_PORTS_RXD_HANDLER(DEVWRITELINE("maincpu:sio0", psxsio0_device, write_rxd))
	MCFG_PSX_CONTROLLER_PORTS_DSR_HANDLER(DEVWRITELINE("maincpu:sio0", psxsio0_device, write_dsr))
	MCFG_PSX_CTRL_PORT_ADD("port1", psx_controllers, "digital_pad")
	MCFG_PSX_CTRL_PORT_ADD("port2", psx_controllers, "digital_pad")

	MCFG_DEVICE_MODIFY("maincpu:sio0")
	MCFG_PSX_SIO_DTR_HANDLER(DEVWRITELINE("^controllers", psxcontrollerports_device, write_dtr))
	MCFG_PSX_SIO_SCK_HANDLER(DEVWRITELINE("^controllers", psxcontrollerports_device, write_sck))
	MCFG_PSX_SIO_TXD_HANDLER(DEVWRITELINE("^controllers", psxcontrollerports_device, write_txd))

	/* video hardware */
	MCFG_PSXGPU_ADD( "maincpu", "gpu", CXD8561Q, 0x100000, XTAL_53_693175MHz )

	/* sound hardware */
	MCFG_SPEAKER_STANDARD_STEREO("lspeaker", "rspeaker")
	MCFG_SPU_ADD( "spu", XTAL_67_7376MHz/2 )
	MCFG_SOUND_ROUTE( 0, "lspeaker", 1.00 )
	MCFG_SOUND_ROUTE( 1, "rspeaker", 1.00 )

	/* quickload */
	MCFG_QUICKLOAD_ADD("quickload", psx1_state, psx_exe_load, "cpe,exe,psf,psx", 0)

	MCFG_SOFTWARE_LIST_ADD("cd_list","psx")

	MCFG_DEVICE_MODIFY( "maincpu" )
	MCFG_PSX_CD_READ_HANDLER( DEVREAD8( PSXCD_TAG, psxcd_device, read ) )
	MCFG_PSX_CD_WRITE_HANDLER( DEVWRITE8( PSXCD_TAG, psxcd_device, write ) )

	MCFG_PSXCD_ADD(PSXCD_TAG, "cdrom")
	MCFG_PSXCD_IRQ_HANDLER(DEVWRITELINE("maincpu:irq", psxirq_device, intin2))
	MCFG_PSX_DMA_CHANNEL_READ( "maincpu", 3, psx_dma_read_delegate( FUNC( psx1_state::cd_dma_read ), (psx1_state *) owner ) )
	MCFG_PSX_DMA_CHANNEL_WRITE( "maincpu", 3, psx_dma_write_delegate( FUNC( psx1_state::cd_dma_write ), (psx1_state *) owner ) )
MACHINE_CONFIG_END

static MACHINE_CONFIG_DERIVED( psu, psj )
	MCFG_CPU_ADD( "subcpu", HD63705, 4166667 ) // MC68HC05G6
	MCFG_CPU_PROGRAM_MAP( subcpu_map )
MACHINE_CONFIG_END

static MACHINE_CONFIG_START( pse, psx1_state )
	/* basic machine hardware */
	MCFG_CPU_ADD( "maincpu", CXD8530AQ, XTAL_67_7376MHz )
	MCFG_CPU_PROGRAM_MAP( psx_map)

	MCFG_RAM_MODIFY("maincpu:ram")
	MCFG_RAM_DEFAULT_SIZE("2M")

	MCFG_DEVICE_ADD("controllers", PSXCONTROLLERPORTS, 0)
	MCFG_PSX_CONTROLLER_PORTS_RXD_HANDLER(DEVWRITELINE("maincpu:sio0", psxsio0_device, write_rxd))
	MCFG_PSX_CONTROLLER_PORTS_DSR_HANDLER(DEVWRITELINE("maincpu:sio0", psxsio0_device, write_dsr))
	MCFG_PSX_CTRL_PORT_ADD("port1", psx_controllers, "digital_pad")
	MCFG_PSX_CTRL_PORT_ADD("port2", psx_controllers, "digital_pad")

	MCFG_DEVICE_MODIFY("maincpu:sio0")
	MCFG_PSX_SIO_DTR_HANDLER(DEVWRITELINE("^controllers", psxcontrollerports_device, write_dtr))
	MCFG_PSX_SIO_SCK_HANDLER(DEVWRITELINE("^controllers", psxcontrollerports_device, write_sck))
	MCFG_PSX_SIO_TXD_HANDLER(DEVWRITELINE("^controllers", psxcontrollerports_device, write_txd))

	/* video hardware */
	/* TODO: visible area and refresh rate */
	MCFG_PSXGPU_ADD( "maincpu", "gpu", CXD8561Q, 0x100000, XTAL_53_693175MHz )

	/* sound hardware */
	MCFG_SPEAKER_STANDARD_STEREO("lspeaker", "rspeaker")
	MCFG_SPU_ADD( "spu", XTAL_67_7376MHz/2 )
	MCFG_SOUND_ROUTE( 0, "lspeaker", 1.00 )
	MCFG_SOUND_ROUTE( 1, "rspeaker", 1.00 )

	/* quickload */
	MCFG_QUICKLOAD_ADD("quickload", psx1_state, psx_exe_load, "cpe,exe,psf,psx", 0)

	MCFG_SOFTWARE_LIST_ADD("cd_list","psx")

	MCFG_DEVICE_MODIFY( "maincpu" )
	MCFG_PSX_CD_READ_HANDLER( DEVREAD8( PSXCD_TAG, psxcd_device, read ) )
	MCFG_PSX_CD_WRITE_HANDLER( DEVWRITE8( PSXCD_TAG, psxcd_device, write ) )

	MCFG_PSXCD_ADD(PSXCD_TAG, "cdrom")
	MCFG_PSXCD_IRQ_HANDLER(DEVWRITELINE("maincpu:irq", psxirq_device, intin2))
	MCFG_PSX_DMA_CHANNEL_READ( "maincpu", 3, psx_dma_read_delegate( FUNC( psx1_state::cd_dma_read ), (psx1_state *) owner ) )
	MCFG_PSX_DMA_CHANNEL_WRITE( "maincpu", 3, psx_dma_write_delegate( FUNC( psx1_state::cd_dma_write ), (psx1_state *) owner ) )
MACHINE_CONFIG_END

ROM_START( psj )
	ROM_REGION32_LE( 0x080000, "maincpu:rom", 0 )

	ROM_SYSTEM_BIOS( 0, "1.0j", "SCPH-1000/DTL-H1000" ) // 22091994
	ROMX_LOAD( "ps-10j.bin",    0x000000, 0x080000, CRC(3b601fc8) SHA1(343883a7b555646da8cee54aadd2795b6e7dd070), ROM_BIOS(1) )

	ROM_SYSTEM_BIOS( 1, "1.1j", "SCPH-3000/DTL-H1000H (Version 1.1 01/22/95)" ) // 22091994
	ROMX_LOAD( "ps-11j.bin",    0x000000, 0x080000, CRC(3539def6) SHA1(b06f4a861f74270be819aa2a07db8d0563a7cc4e), ROM_BIOS(2) )

	ROM_SYSTEM_BIOS( 2, "2.1j", "SCPH-3500 (Version 2.1 07/17/95 J)" ) // 22091994
	ROMX_LOAD( "ps-21j.bin",    0x000000, 0x080000, CRC(bc190209) SHA1(e38466a4ba8005fba7e9e3c7b9efeba7205bee3f), ROM_BIOS(3) )

	ROM_SYSTEM_BIOS( 3, "2.2j", "SCPH-5000/DTL-H1200/DTL-H3000 (Version 2.2 12/04/95 J)" ) // 04121995
/*  ROMX_LOAD( "ps-22j.bad",    0x000000, 0x080000, BAD_DUMP CRC(8c93a399) SHA1(e340db2696274dda5fdc25e434a914db71e8b02b), ROM_BIOS(4) ) */
	ROMX_LOAD( "ps-22j.bin",    0x000000, 0x080000, CRC(24fc7e17) SHA1(ffa7f9a7fb19d773a0c3985a541c8e5623d2c30d), ROM_BIOS(4) )

	ROM_SYSTEM_BIOS( 4, "2.2d", "DTL-H1100 (Version 2.2 03/06/96 D)" ) // 04121995
	ROMX_LOAD( "ps-22d.bin",    0x000000, 0x080000, CRC(decb22f5) SHA1(73107d468fc7cb1d2c5b18b269715dd889ecef06), ROM_BIOS(5) )

	ROM_SYSTEM_BIOS( 5, "3.0j", "SCPH-5500 (Version 3.0 09/09/96 J)" ) // 04121995
	ROMX_LOAD( "ps-30j.bin",    0x000000, 0x080000, CRC(ff3eeb8c) SHA1(b05def971d8ec59f346f2d9ac21fb742e3eb6917), ROM_BIOS(6) )

	ROM_SYSTEM_BIOS( 6, "4.0j", "SCPH-7000/SCPH-7500/SCPH-9000 (Version 4.0 08/18/97 J)" ) // 29051997
	ROMX_LOAD( "ps-40j.bin",    0x000000, 0x080000, CRC(ec541cd0) SHA1(77b10118d21ac7ffa9b35f9c4fd814da240eb3e9), ROM_BIOS(7) )

	ROM_SYSTEM_BIOS( 7, "4.1a", "SCPH-7000W (Version 4.1 11/14/97 A)" ) // 04121995
	ROMX_LOAD( "ps-41a(w).bin", 0x000000, 0x080000, CRC(b7c43dad) SHA1(1b0dbdb23da9dc0776aac58d0755dc80fea20975), ROM_BIOS(8) )

	ROM_SYSTEM_BIOS( 8, "4.3j", "SCPH-100 (Version 4.3 03/11/00 J)" ) // 04121995
	ROMX_LOAD( "psone-43j.bin", 0x000000, 0x080000, CRC(f2af798b) SHA1(339a48f4fcf63e10b5b867b8c93cfd40945faf6c), ROM_BIOS(9) )
ROM_END

ROM_START( psu )
	ROM_REGION32_LE( 0x080000, "maincpu:rom", 0 )

	ROM_SYSTEM_BIOS( 0, "2.0a", "DTL-H1001 (Version 2.0 05/07/95 A)" ) // 22091994
	ROMX_LOAD( "ps-20a.bin",    0x000000, 0x080000, CRC(55847d8c) SHA1(649895efd79d14790eabb362e94eb0622093dfb9), ROM_BIOS(1) )

	ROM_SYSTEM_BIOS( 1, "2.1a", "DTL-H1101 (Version 2.1 07/17/95 A)" ) // 22091994
	ROMX_LOAD( "ps-21a.bin",    0x000000, 0x080000, CRC(aff00f2f) SHA1(ca7af30b50d9756cbd764640126c454cff658479), ROM_BIOS(2) )

	ROM_SYSTEM_BIOS( 2, "2.2a", "SCPH-1001/DTL-H1201/DTL-H3001 (Version 2.2 12/04/95 A)" ) // 04121995
	ROMX_LOAD( "ps-22a.bin",    0x000000, 0x080000, CRC(37157331) SHA1(10155d8d6e6e832d6ea66db9bc098321fb5e8ebf), ROM_BIOS(3) )

	ROM_SYSTEM_BIOS( 3, "3.0a", "SCPH-5501/SCPH-5503/SCPH-7003 (Version 3.0 11/18/96 A)" ) // 04121995
	ROMX_LOAD( "ps-30a.bin",    0x000000, 0x080000, CRC(8d8cb7e4) SHA1(0555c6fae8906f3f09baf5988f00e55f88e9f30b), ROM_BIOS(4) )

	ROM_SYSTEM_BIOS( 4, "4.1a", "SCPH-7001/SCPH-7501/SCPH-7503/SCPH-9001/SCPH-9003/SCPH-9903 (Version 4.1 12/16/97 A)" ) // 04121995
	ROMX_LOAD( "ps-41a.bin",    0x000000, 0x080000, CRC(502224b6) SHA1(14df4f6c1e367ce097c11deae21566b4fe5647a9), ROM_BIOS(5) )

	ROM_SYSTEM_BIOS( 5, "4.5a", "SCPH-101 (Version 4.5 05/25/00 A)" ) // 04121995
	ROMX_LOAD( "psone-45a.bin", 0x000000, 0x080000, CRC(171bdcec) SHA1(dcffe16bd90a723499ad46c641424981338d8378), ROM_BIOS(6) )

	ROM_REGION( 0x10000, "subcpu", 0 )
	ROM_LOAD( "ram.ic304",    0x000000, 0x000400, CRC(e9fefc8b) SHA1(7ab4a498216fb9a3fca9daf6ad21b4553126e74f) )
	ROM_FILL( 0x0400, 0x0c00, 0xff )
	ROM_LOAD( "scea.ic304",   0x001000, 0x004000, CRC(82729934) SHA1(7d5f52eb9df1243dcdab32cb763a9eb6a22706d7) )
	ROM_FILL( 0x5000, 0xae00, 0xff )
	ROM_LOAD( "test.ic304",   0x00fe00, 0x000200, CRC(3b2f8041) SHA1(d7127cb4a9b5efe9deffab3b72ab4451cb30675b) )
ROM_END

ROM_START( pse )
	ROM_REGION32_LE( 0x080000, "maincpu:rom", 0 )

	ROM_SYSTEM_BIOS( 0, "2.0e", "DTL-H1002/SCPH-1002 (Version 2.0 05/10/95 E)" ) // 22091994
	ROMX_LOAD( "ps-20e.bin",    0x000000, 0x080000, CRC(9bb87c4b) SHA1(20b98f3d80f11cbf5a7bfd0779b0e63760ecc62c), ROM_BIOS(1) )

	ROM_SYSTEM_BIOS( 1, "2.1e", "SCPH-1002/DTL-H1102 (Version 2.1 07/17/95 E)" ) // 22091994
	ROMX_LOAD( "ps-21e.bin",    0x000000, 0x080000, CRC(86c30531) SHA1(76cf6b1b2a7c571a6ad07f2bac0db6cd8f71e2cc), ROM_BIOS(2) )

	ROM_SYSTEM_BIOS( 2, "2.2e", "SCPH-1002/DTL-H1202/DTL-H3002 (Version 2.2 12/04/95 E)" ) // 04121995
	ROMX_LOAD( "ps-22e.bin",    0x000000, 0x080000, CRC(1e26792f) SHA1(b6a11579caef3875504fcf3831b8e3922746df2c), ROM_BIOS(3) )

	ROM_SYSTEM_BIOS( 3, "3.0e", "SCPH-5502/SCPH-5552 (Version 3.0 01/06/97 E)" ) // 04121995
/*  ROMX_LOAD( "ps-30e.bad",    0x000000, 0x080000, BAD_DUMP CRC(4d9e7c86) SHA1(f8de9325fc36fcfa4b29124d291c9251094f2e54), ROM_BIOS(4) ) */
	ROMX_LOAD( "ps-30e.bin",    0x000000, 0x080000, CRC(d786f0b9) SHA1(f6bc2d1f5eb6593de7d089c425ac681d6fffd3f0), ROM_BIOS(4) )

	ROM_SYSTEM_BIOS( 4, "4.1e", "SCPH-7002/SCPH-7502/SCPH-9002 (Version 4.1 12/16/97 E)" ) // 04121995
	ROMX_LOAD( "ps-41e.bin",    0x000000, 0x080000, CRC(318178bf) SHA1(8d5de56a79954f29e9006929ba3fed9b6a418c1d), ROM_BIOS(5) )

	ROM_SYSTEM_BIOS( 5, "4.4e", "SCPH-102 (Version 4.4 03/24/00 E)" ) // 04121995
	ROMX_LOAD( "psone-44e.bin", 0x000000, 0x080000, CRC(0bad7ea9) SHA1(beb0ac693c0dc26daf5665b3314db81480fa5c7c), ROM_BIOS(6) )

	ROM_SYSTEM_BIOS( 6, "4.5e", "SCPH-102 (Version 4.5 05/25/00 E)" ) // 04121995
	ROMX_LOAD( "psone-45e.bin", 0x000000, 0x080000, CRC(76b880e5) SHA1(dbc7339e5d85827c095764fc077b41f78fd2ecae), ROM_BIOS(7) )
ROM_END

ROM_START( psa )
	ROM_REGION32_LE( 0x080000, "maincpu:rom", 0 )

	ROM_SYSTEM_BIOS( 0, "3.0a", "SCPH-5501/SCPH-5503/SCPH-7003 (Version 3.0 11/18/96 A)" ) // 04121995
	ROMX_LOAD( "ps-30a.bin",    0x000000, 0x080000, CRC(8d8cb7e4) SHA1(0555c6fae8906f3f09baf5988f00e55f88e9f30b), ROM_BIOS(1) )

	ROM_SYSTEM_BIOS( 1, "4.1a", "SCPH-7001/SCPH-7501/SCPH-7503/SCPH-9001/SCPH-9003/SCPH-9903 (Version 4.1 12/16/97 A)" ) // 04121995
	ROMX_LOAD( "ps-41a.bin",    0x000000, 0x080000, CRC(502224b6) SHA1(14df4f6c1e367ce097c11deae21566b4fe5647a9), ROM_BIOS(2) )
ROM_END

/*
The version number & release date is stored in ascii text at the end of every bios, except for scph1000.
There is also a BCD encoded date at offset 0x100, but this is set to 22091994 for versions prior to 2.2
and 04121995 for all versions from 2.2 ( except Version 4.0J which is 29051997 ).

Consoles not dumped:

DTL-H1001H
SCPH-5001
SCPH-5002 (this is not mentioned in SCPH-102B.pdf so it's likely it doesn't exist)
SCPH-5003
SCPH-103

Holes in version numbers:

Version 2.0 J
Version 4.1 J (SCPH7000W uses 4.1 A)
Version 4.2 J
Version 4.4 J
Version 4.5 J
Version 4.0 A
Version 4.2 A
Version 4.3 A
Version 4.4 A
Version 4.0 E
Version 4.2 E
Version 4.3 E

*/

/*    YEAR  NAME    PARENT  COMPAT  MACHINE     INPUT   INIT    COMPANY                             FULLNAME                            FLAGS */
CONS( 1994, psj,    0,      0,      psj,        0, driver_device,    0,    "Sony Computer Entertainment Inc", "Sony PlayStation (Japan)",         GAME_NOT_WORKING | GAME_IMPERFECT_SOUND | GAME_IMPERFECT_GRAPHICS )
CONS( 1995, pse,    psj,    0,      pse,        0, driver_device,    0,    "Sony Computer Entertainment Inc", "Sony PlayStation (Europe)",        GAME_NOT_WORKING | GAME_IMPERFECT_SOUND | GAME_IMPERFECT_GRAPHICS )
CONS( 1995, psu,    psj,    0,      psu,        0, driver_device,    0,    "Sony Computer Entertainment Inc", "Sony PlayStation (USA)",           GAME_NOT_WORKING | GAME_IMPERFECT_SOUND | GAME_IMPERFECT_GRAPHICS )
CONS( 1995, psa,    psj,    0,      psj,        0, driver_device,    0,    "Sony Computer Entertainment Inc", "Sony PlayStation (Asia-Pacific)",  GAME_NOT_WORKING | GAME_IMPERFECT_SOUND | GAME_IMPERFECT_GRAPHICS )
