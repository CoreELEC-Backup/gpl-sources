/******************************************************************************
    BBC Model B

    MESS Driver By:

    Gordon Jefferyes
    mess_bbc@romvault.com
    Nigel Barnes
    ngbarnes@hotmail.com

******************************************************************************/

#include <ctype.h>
#include "emu.h"
#include "cpu/m6502/m6502.h"
#include "sound/tms5220.h"
#include "machine/6522via.h"
#include "machine/wd17xx.h"
#include "imagedev/flopdrv.h"
#include "includes/bbc.h"
#include "machine/mc146818.h"
#include "bus/centronics/ctronics.h"
#include "imagedev/cassette.h"


void bbc_state::check_interrupts()
{
	m_maincpu->set_input_line(M6502_IRQ_LINE, m_via_system_irq || m_via_user_irq || m_acia_irq || m_ACCCON_IRR);
}

/*************************
Model A memory handling functions
*************************/

/* for the model A just address the 4 on board ROM sockets */
WRITE8_MEMBER(bbc_state::bbc_page_selecta_w)
{
	m_bank4->set_entry(data & 0x03);
}


WRITE8_MEMBER(bbc_state::bbc_memorya1_w)
{
	m_region_maincpu->base()[offset]=data;
}

/*************************
Model B memory handling functions
*************************/

/* the model B address all 16 of the ROM sockets */
/* I have set bank 1 as a special case to load different DFS roms selectable from MESS's CONF settings var:bbc_DFSTypes */
WRITE8_MEMBER(bbc_state::bbc_page_selectb_w)
{
	m_rombank = data & 0x0f;
	if (m_rombank != 1)
	{
		m_bank4->set_entry(m_rombank);
	}
	else
	{
		m_bank4->set_entry(0x10 + m_DFSType);
	}
}


WRITE8_MEMBER(bbc_state::bbc_memoryb3_w)
{
	if (m_ram->size() == 32*1024)
	{
		m_region_maincpu->base()[offset + 0x4000] = data;
	}
	else
	{
		m_region_maincpu->base()[offset] = data;
	}
}

/* I have setup 3 types of sideways ram:
0: none
1: 128K (bank 8 to 15) Solidisc sidewaysram userport bank latch
2: 64K (banks 4 to 7) for Acorn sideways ram FE30 bank latch
3: 128K (banks 8 to 15) for Acown sideways ram FE30 bank latch
*/
static const unsigned short bbc_SWRAMtype1[16]={0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1};
static const unsigned short bbc_SWRAMtype2[16]={0,0,0,0,1,1,1,1,0,0,0,0,0,0,0,0};
static const unsigned short bbc_SWRAMtype3[16]={0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1};

WRITE8_MEMBER(bbc_state::bbc_memoryb4_w)
{
	if (m_rombank == 1)
	{
		// special DFS case for Acorn DFS E00 Hack that can write to the DFS RAM Bank;
		if (m_DFSType == 3) m_region_dfs->base()[((m_DFSType) << 14) + offset] = data;
	}
	else
	{
		switch (m_SWRAMtype)
		{
			case 1: if (bbc_SWRAMtype1[m_userport]) m_region_opt->base()[(m_userport << 14) + offset] = data;
			case 2: if (bbc_SWRAMtype2[m_rombank])  m_region_opt->base()[(m_rombank << 14) + offset] = data;
			case 3: if (bbc_SWRAMtype3[m_rombank])  m_region_opt->base()[(m_rombank << 14) + offset] = data;
		}
	}
}

/****************************************/
/* BBC B Plus memory handling function */
/****************************************/


/*  this function should return true if
    the instruction is in the VDU driver address ranged
    these are set when:
    PC is in the range c000 to dfff
    or if pagedRAM set and PC is in the range a000 to afff
*/
int bbc_state::vdudriverset()
{
	int PC;
	PC = machine().device("maincpu")->safe_pc(); // this needs to be set to the 6502 program counter
	return (((PC >= 0xc000) && (PC <= 0xdfff)) || ((m_pagedRAM) && ((PC >= 0xa000) && (PC <= 0xafff))));
}


/* the model B Plus addresses all 16 of the ROM sockets plus the extra 12K of ram at 0x8000
   and 20K of shadow ram at 0x3000 */
WRITE8_MEMBER(bbc_state::bbc_page_selectbp_w)
{
	if ((offset&0x04)==0)
	{
		m_pagedRAM = (data >> 7) & 0x01;
		m_rombank =  data & 0x0f;

		if (m_pagedRAM)
		{
			/* if paged ram then set 8000 to afff to read from the ram 8000 to afff */
			m_bank4->set_entry(0x10);
		}
		else
		{
			/* if paged rom then set the rom to be read from 8000 to afff */
			m_bank4->set_entry(m_rombank);
		};

		/* set the rom to be read from b000 to bfff */
		m_bank6->set_entry(m_rombank);
	}
	else
	{
		//the video display should now use this flag to display the shadow ram memory
		m_vdusel=(data>>7)&0x01;
		bbcbp_setvideoshadow(m_vdusel);
		//need to make the video display do a full screen refresh for the new memory area
		m_bank2->set_base(m_region_maincpu->base() + 0x3000);
	}
}


/* write to the normal memory from 0x0000 to 0x2fff
   the writes to this memory are just done the normal
   way */

WRITE8_MEMBER(bbc_state::bbc_memorybp1_w)
{
	m_region_maincpu->base()[offset]=data;
}


/* the next two function handle reads and write to the shadow video ram area
   between 0x3000 and 0x7fff

   when vdusel is set high the video display uses the shadow ram memory
   the processor only reads and write to the shadow ram when vdusel is set
   and when the instruction being executed is stored in a set range of memory
   addresses known as the VDU driver instructions.
*/


DIRECT_UPDATE_MEMBER(bbc_state::bbcbp_direct_handler)
{
	UINT8 *RAM = m_region_maincpu->base();
	if (m_vdusel == 0)
	{
		// not in shadow ram mode so just read normal ram
		m_bank2->set_base(RAM + 0x3000);
	}
	else
	{
		if (vdudriverset())
		{
			// if VDUDriver set then read from shadow ram
			m_bank2->set_base(RAM + 0xb000);
		}
		else
		{
			// else read from normal ram
			m_bank2->set_base(RAM + 0x3000);
		}
	}
	return address;
}


WRITE8_MEMBER(bbc_state::bbc_memorybp2_w)
{
	UINT8 *RAM = m_region_maincpu->base();
	if (m_vdusel==0)
	{
		// not in shadow ram mode so just write to normal ram
		RAM[offset + 0x3000] = data;
	}
	else
	{
		if (vdudriverset())
		{
			// if VDUDriver set then write to shadow ram
			RAM[offset + 0xb000] = data;
		}
		else
		{
			// else write to normal ram
			RAM[offset + 0x3000] = data;
		}
	}
}


/* if the pagedRAM is set write to RAM between 0x8000 to 0xafff
otherwise this area contains ROM so no write is required */
WRITE8_MEMBER(bbc_state::bbc_memorybp4_w)
{
	if (m_pagedRAM)
	{
		m_region_maincpu->base()[offset+0x8000]=data;
	}
}


/* the BBC B plus 128K had extra ram mapped in replacing the
rom bank 0,1,c and d.
The function memorybp3_128_w handles memory writes from 0x8000 to 0xafff
which could either be sideways ROM, paged RAM, or sideways RAM.
The function memorybp4_128_w handles memory writes from 0xb000 to 0xbfff
which could either be sideways ROM or sideways RAM */


static const unsigned short bbc_b_plus_sideways_ram_banks[16]={ 1,1,0,0,0,0,0,0,0,0,0,0,1,1,0,0 };


WRITE8_MEMBER(bbc_state::bbc_memorybp4_128_w)
{
	if (m_pagedRAM)
	{
		m_region_maincpu->base()[offset+0x8000]=data;
	}
	else
	{
		if (bbc_b_plus_sideways_ram_banks[m_rombank])
		{
			m_region_opt->base()[offset+(m_rombank<<14)]=data;
		}
	}
}

WRITE8_MEMBER(bbc_state::bbc_memorybp6_128_w)
{
	if (bbc_b_plus_sideways_ram_banks[m_rombank])
	{
		m_region_opt->base()[offset+(m_rombank<<14)+0x3000]=data;
	}
}



/****************************************/
/* BBC Master functions                 */
/****************************************/

/*
ROMSEL - &FE30 write Only
B7 RAM 1=Page in ANDY &8000-&8FFF
       0=Page in ROM  &8000-&8FFF
B6 Not Used
B5 Not Used
B4 Not Used
B3-B0  Rom/Ram Bank Select

ACCCON

b7 IRR  1=Causes an IRQ to the processor
b6 TST  1=Selects &FC00-&FEFF read from OS-ROM
b5 IFJ  1=Internal 1MHz bus
        0=External 1MHz bus
b4 ITU  1=Internal Tube
        0=External Tube
b3 Y    1=Read/Write HAZEL &C000-&DFFF RAM
        0=Read/Write ROM &C000-&DFFF OS-ROM
b2 X    1=Read/Write LYNNE
        0=Read/WRITE main memory &3000-&8000
b1 E    1=Causes shadow if VDU code
        0=Main all the time
b0 D    1=Display LYNNE as screen
        0=Display main RAM screen

ACCCON is a read/write register

HAZEL is the 8K of RAM used by the MOS, filing system, and other Roms at &C000-&DFFF

ANDY is the name of the 4K of RAM used by the MOS at &8000-&8FFF

b7:This causes an IRQ to occur. If you set this bit, you must write a routine on IRQ2V to clear it.

b6:If set, this switches in the ROM at &FD00-&FEFF for reading, writes to these addresses are still directed to the I/O
The MOS will not work properly with this but set. the Masters OS uses this feature to place some of the reset code in this area,
which is run at powerup.

b3:If set, access to &C000-&DFFF are directed to HAZEL, an 8K bank of RAM. IF clear, then operating system ROM is read.

b2:If set, read/write access to &3000-&7FFF are directed to the LYNNE, the shadow screen RAM. If clear access is made to the main RAM.

b1:If set, when the program counter is between &C000 and &DFFF, read/write access is directed to the LYNNE shadow RAM.
if the program counter is anywhere else main ram is accessed.

*/


READ8_MEMBER(bbc_state::bbcm_ACCCON_read)
{
	logerror("ACCCON read %d\n",offset);
	return m_ACCCON;
}

WRITE8_MEMBER(bbc_state::bbcm_ACCCON_write)
{
	int tempIRR;
	m_ACCCON=data;

	logerror("ACCCON write  %d %d \n",offset,data);

	tempIRR=m_ACCCON_IRR;

	m_ACCCON_IRR=(data>>7)&1;
	m_ACCCON_TST=(data>>6)&1;
	m_ACCCON_IFJ=(data>>5)&1;
	m_ACCCON_ITU=(data>>4)&1;
	m_ACCCON_Y  =(data>>3)&1;
	m_ACCCON_X  =(data>>2)&1;
	m_ACCCON_E  =(data>>1)&1;
	m_ACCCON_D  =(data>>0)&1;

	if (tempIRR!=m_ACCCON_IRR)
	{
		check_interrupts();
	}

	if (m_ACCCON_Y)
	{
		m_bank7->set_base(m_region_maincpu->base() + 0x9000);
	}
	else
	{
		m_bank7->set_base(m_region_os->base());
	}

	bbcbp_setvideoshadow(m_ACCCON_D);


	if (m_ACCCON_X)
	{
		m_bank2->set_base(m_region_maincpu->base() + 0xb000);
	}
	else
	{
		m_bank2->set_base(m_region_maincpu->base() + 0x3000);
	}

	/* ACCCON_TST controls paging of rom reads in the 0xFC00-0xFEFF reigon */
	/* if 0 the I/O is paged for both reads and writes */
	/* if 1 the ROM is paged in for reads but writes still go to I/O */
	if (m_ACCCON_TST)
	{
		m_bank8->set_base(m_region_os->base() + 0x3c00);
		space.install_read_bank(0xfc00, 0xfeff, "bank8");
	}
	else
	{
		space.install_read_handler(0xfc00, 0xfeff, read8_delegate(FUNC(bbc_state::bbcm_r),this));
	}
}


int bbc_state::bbcm_vdudriverset()
{
	int PC;
	PC = machine().device("maincpu")->safe_pc();
	return ((PC >= 0xc000) && (PC <= 0xdfff));
}


WRITE8_MEMBER(bbc_state::page_selectbm_w)
{
	m_pagedRAM = (data & 0x80) >> 7;
	m_rombank = data & 0x0f;

	if (m_pagedRAM)
	{
		m_bank4->set_entry(0x10);
		m_bank5->set_entry(m_rombank);
	}
	else
	{
		m_bank4->set_entry(m_rombank);
		m_bank5->set_entry(m_rombank);
	}
}



WRITE8_MEMBER(bbc_state::bbc_memorybm1_w)
{
	m_region_maincpu->base()[offset] = data;
}


DIRECT_UPDATE_MEMBER(bbc_state::bbcm_direct_handler)
{
	if (m_ACCCON_X)
	{
		m_bank2->set_base(m_region_maincpu->base() + 0xb000);
	}
	else
	{
		if (m_ACCCON_E && bbcm_vdudriverset())
		{
			m_bank2->set_base(m_region_maincpu->base() + 0xb000);
		}
		else
		{
			m_bank2->set_base(m_region_maincpu->base() + 0x3000);
		}
	}

	return address;
}



WRITE8_MEMBER(bbc_state::bbc_memorybm2_w)
{
	UINT8 *RAM = m_region_maincpu->base();
	if (m_ACCCON_X)
	{
		RAM[offset + 0xb000] = data;
	}
	else
	{
		if (m_ACCCON_E && bbcm_vdudriverset())
		{
			RAM[offset + 0xb000] = data;
		}
		else
		{
			RAM[offset + 0x3000] = data;
		}
	}
}

static const unsigned short bbc_master_sideways_ram_banks[16]=
{
	0,0,0,0,1,1,1,1,0,0,0,0,0,0,0,0
};


WRITE8_MEMBER(bbc_state::bbc_memorybm4_w)
{
	if (m_pagedRAM)
	{
		m_region_maincpu->base()[offset+0x8000] = data;
	}
	else
	{
		if (bbc_master_sideways_ram_banks[m_rombank])
		{
			m_region_opt->base()[offset+(m_rombank<<14)] = data;
		}
	}
}


WRITE8_MEMBER(bbc_state::bbc_memorybm5_w)
{
	if (bbc_master_sideways_ram_banks[m_rombank])
	{
		m_region_opt->base()[offset+(m_rombank<<14)+0x1000] = data;
	}
}


WRITE8_MEMBER(bbc_state::bbc_memorybm7_w)
{
	if (m_ACCCON_Y)
	{
		m_region_maincpu->base()[offset+0x9000] = data;
	}
}



/******************************************************************************
&FC00-&FCFF FRED
&FC00-&FC03 Byte-Wide Expansion RAM
&FC08-&FC0F Ample M2000 MIDI Interface (see also FCF0)
&FC10-&FC13 Teletext
&FC14-&FC1F Prestel
&FC20-&FC27 IEEE 488 Interface
&FC28-&FC2F Acorn Expansion, currently unused
&FC30-&FC3F Cambridge Ring Interface
&FC40-&FC47 Winchester Disc Interface
&FC48-&FC7F Acorn Expansion, currently unused
&FC80-&FC8F Test Hardware
&FC90-&FCBF Acorn Expansion, currently unused
&FCC0-&FCFE User Applications
&FCF0-&FCF7 JGH/ETI MIDI Control (see also FC08)
&FCFC-&FCFF Page-Wide Expansion RAM

&FD00-&FDFF JIM
&FD00-&FDFF Page-wide expansion RAM window
&FD40-&FD4F Torch SASI/SCSI Hard Drive Access
&FDFE-&FDFF Reset Test vector

&FE00-&FEFF SHEILA          Read                    Write
&FE00-&FE07 6845 CRTC       Video controller        Video Controller         8 ( 2 bytes x  4 )
&FE08-&FE0F 6850 ACIA       Serial controller       Serial Controller        8 ( 2 bytes x  4 )
&FE10-&FE17 Serial ULA      -                       Serial system chip       8 ( 1 byte  x  8 )
&FE18-&FE1F uPD7002         A to D converter        A to D converter         8 ( 4 bytes x  2 )
&FE20-&FE23 Video ULA       -                       Video system chip        4 ( 2 bytes x  2 )
&FE24-&FE27 FDC Latch       1770 Control latch      1770 Control latch       4 ( 1 byte  x  4 )
&FE28-&FE2F 1770 registers  1770 Disc Controller    1170 Disc Controller     8 ( 4 bytes x  2 )
&FE30-&FE33 ROMSEL          -                       ROM Select               4 ( 1 byte  x  4 )
&FE34-&3FE7 ACCCON          ACCCON select reg.      ACCCON select reg        4 ( 1 byte  x  4 )
&FE38-&FE3F NC              -                       -
&FE40-&FE5F 6522 VIA        SYSTEM VIA              SYSTEM VIA              32 (16 bytes x  2 ) 1MHz
&FE60-&FE7F 6522 VIA        USER VIA                USER VIA                32 (16 bytes x  2 ) 1MHz
&FE80-&FE9F 8271 registers  8271 Disk Controller    8271 Disk Controller
&FEA0-&FEBF 68B54 ADLC      ECONET controller       ECONET controller       32 ( 4 bytes x  8 ) 2MHz
&FEC0-&FEDF 6854 ADLC       ECONET controller       ECONET controller       32 ( 4 bytes x  8 ) 2MHz
&FEE0-&FEFF Tube ULA        Tube system interface   Tube system interface   32 (32 bytes x  1 ) 2MHz
******************************************************************************/

READ8_MEMBER(bbc_state::bbcm_r)
{
	long myo;

	/* Now handled in bbcm_ACCCON_write PHS - 2008-10-11 */
//  if ( m_ACCCON_TST )
//  {
//      return m_region_os->base()[offset + 0x3c00];
//  };

	if (offset<=0x0ff) /* FRED */
	{
		return 0xff;
	};

	if ((offset>=0x100) && (offset<=0x1ff)) /* JIM */
	{
		return 0xff;
	};

	if ((offset>=0x200) && (offset<=0x2ff)) /* SHEILA */
	{
		myo = offset-0x200;
		if ((myo>=0x00) && (myo<=0x07)) return bbc_6845_r(space, myo-0x00);     /* Video Controller */
		if ((myo>=0x08) && (myo<=0x0f))
		{
			if ((myo - 0x08) & 1)
				return m_acia->status_r(space,0);
			else
				return m_acia->data_r(space,0);
		}
		if ((myo>=0x10) && (myo<=0x17)) return 0xfe;                                /* Serial System Chip */
		if ((myo>=0x18) && (myo<=0x1f)) return m_upd7002->read(space, myo-0x18);    /* A to D converter */
		if ((myo>=0x20) && (myo<=0x23)) return 0xfe;                                /* VideoULA */
		if ((myo>=0x24) && (myo<=0x27)) return bbcm_wd1770l_read(space, myo-0x24);  /* 1770 */
		if ((myo>=0x28) && (myo<=0x2f)) return bbcm_wd1770_read(space, myo-0x28);   /* disc control latch */
		if ((myo>=0x30) && (myo<=0x33)) return 0xfe;                                /* page select */
		if ((myo>=0x34) && (myo<=0x37)) return bbcm_ACCCON_read(space, myo-0x34);   /* ACCCON */
		if ((myo>=0x38) && (myo<=0x3f)) return 0xfe;                                /* NC ?? */
		if ((myo>=0x40) && (myo<=0x5f)) return m_via6522_0->read(space, myo-0x40);
		if ((myo>=0x60) && (myo<=0x7f)) return m_via6522_1->read(space, myo-0x60);
		if ((myo>=0x80) && (myo<=0x9f)) return 0xfe;
		if ((myo>=0xa0) && (myo<=0xbf)) return m_adlc->read(space, myo & 0x03);
		if ((myo>=0xc0) && (myo<=0xdf)) return 0xfe;
		if ((myo>=0xe0) && (myo<=0xff)) return 0xfe;
	}

	return 0xfe;
}

WRITE8_MEMBER(bbc_state::bbcm_w)
{
	long myo;

	if ((offset>=0x200) && (offset<=0x2ff)) /* SHEILA */
	{
		myo=offset-0x200;
		if ((myo>=0x00) && (myo<=0x07)) bbc_6845_w(space, myo-0x00, data);           /* Video Controller */
		if ((myo>=0x08) && (myo<=0x0f))
		{
			if ((myo - 0x08) & 1)
				m_acia->control_w(space, 0, data);
			else
				m_acia->data_w(space, 0, data);
		}
		if ((myo>=0x10) && (myo<=0x17)) bbc_SerialULA_w(space, myo-0x10, data);     /* Serial System Chip */
		if ((myo>=0x18) && (myo<=0x1f)) m_upd7002->write(space, myo-0x18, data);    /* A to D converter */
		if ((myo>=0x20) && (myo<=0x23)) bbc_videoULA_w(space, myo-0x20, data);      /* VideoULA */
		if ((myo>=0x24) && (myo<=0x27)) bbcm_wd1770l_write(space, myo-0x24, data);  /* 1770 */
		if ((myo>=0x28) && (myo<=0x2f)) bbcm_wd1770_write(space, myo-0x28, data);   /* disc control latch */
		if ((myo>=0x30) && (myo<=0x33)) page_selectbm_w(space, myo-0x30, data);     /* page select */
		if ((myo>=0x34) && (myo<=0x37)) bbcm_ACCCON_write(space, myo-0x34, data);   /* ACCCON */
		//if ((myo>=0x38) && (myo<=0x3f))                                           /* NC ?? */
		if ((myo>=0x40) && (myo<=0x5f)) m_via6522_0->write(space, myo-0x40, data);
		if ((myo>=0x60) && (myo<=0x7f)) m_via6522_1->write(space, myo-0x60, data);
		//if ((myo>=0x80) && (myo<=0x9f))
		if ((myo>=0xa0) && (myo<=0xbf)) m_adlc->write(space, myo & 0x03, data);
		//if ((myo>=0xc0) && (myo<=0xdf))
		//if ((myo>=0xe0) && (myo<=0xff))
	}
}


/******************************************************************************

System VIA 6522

PA0-PA7
Port A forms a slow data bus to the keyboard sound and speech processors
PortA   Keyboard
D0  Pin 8
D1  Pin 9
D2  Pin 10
D3  Pin 11
D4  Pin 5
D5  Pin 6
D6  Pin 7
D7  Pin 12

PB0-PB2 outputs
---------------
These 3 outputs form the address to an 8 bit addressable latch.
(IC32 74LS259)

PB3 output
----------
This output holds the data to be written to the selected
addressable latch bit.

PB4 and PB5 inputs
------------------
These are the inputs from the joystick FIRE buttons. They are
normally at logic 1 with no button pressed and change to 0
when a button is pressed.

PB6 and PB7 inputs from the speech processor
--------------------------------------------
PB6 is the speech processor 'ready' output and PB7 is from the
speech processor 'interrupt' output.

CA1 input
---------
This is the vertical sync input from the 6845. CA1 is set up to
interrupt the 6502 every 20ms (50Hz) as a vertical sync from
the video circuity is detected. The operation system changes
the flash colours on the display in this interrupt time so that
they maintain synchronisation with the rest of the picture.
----------------------------------------------------------------
This is required for a lot of time function within the machine
and must be triggered every 20ms. (Should check at some point
how this 20ms signal is made, and see if none standard shaped
screen modes change this time period.)

CB1 input
---------
The CB1 input is the end of conversion (EOC) signal from the
7002 analogue to digital converter. It can be used to interrupt
the 6502 whenever a conversion is complete.

CA2 input
---------
This input comes from the keyboard circuit, and is used to
generate an interrupt whenever a key is pressed. See the
keyboard circuit section for more details.

CB2 input
---------
This is the light pen strobe signal (LPSTB) from the light pen.
If also connects to the 6845 video processor,
CB2 can be programmed to interrupt the processor whenever
a light pen strobe occurs.
----------------------------------------------------------------
CB2 is not needed in the initial emulation
and should be set to logic low, should be mapped through to
a light pen emulator later.

IRQ output
This connects to the IRQ line of the 6502


The addressable latch
This 8 bit addressable latch is operated from port B lines 0-3.
PB0-PB2 are set to the required address of the output bit to be set.
PB3 is set to the value which should be programmed at that bit.
The function of the 8 output bits from this latch are:-

B0 - Write Enable to the sound generator IC
B1 - READ select on the speech processor
B2 - WRITE select on the speech processor
B3 - Keyboard write enable
B4,B5 - these two outputs define the number to be added to the
start of screen address in hardware to control hardware scrolling:-
Mode    Size    Start of screen  Number to add  B5      B4
0,1,2   20K     &3000                12K        1       1
3       16K     &4000                16K        0       0
4,5     10K     &5800 (or &1800)     22K        1       0
6       8K      &6000 (or &2000)     24K        0       1
B6 - Operates the CAPS lock LED  (Pin 17 keyboard connector)
B7 - Operates the SHIFT lock LED (Pin 16 keyboard connector)
******************************************************************************/


INTERRUPT_GEN_MEMBER(bbc_state::bbcb_keyscan)
{
	static const char *const colnames[] = {
		"COL0", "COL1", "COL2", "COL3", "COL4",
		"COL5", "COL6", "COL7", "COL8", "COL9",
		"COL10", "COL11", "COL12"
	};

	/* only do auto scan if keyboard is not enabled */
	if (m_b3_keyboard == 1)
	{
		/* KBD IC1 4 bit addressable counter */
		/* KBD IC3 4 to 10 line decoder */
		/* keyboard not enabled so increment counter */
		m_column = (m_column + 1) % 16;

		if (m_column < 13)
		{
			/* KBD IC4 8 input NAND gate */
			/* set the value of via_system ca2, by checking for any keys
			     being pressed on the selected m_column */
			if ((ioport(colnames[m_column])->read() | 0x01) != 0xff)
			{
				m_via6522_0->write_ca2(1);
			}
			else
			{
				m_via6522_0->write_ca2(0);
			}
		}
		else
		{
			m_via6522_0->write_ca2(0);
		}
	}
}


int bbc_state::bbc_keyboard(address_space &space, int data)
{
	int bit;
	int row;
	int res;
	static const char *const colnames[] = {
		"COL0", "COL1", "COL2", "COL3", "COL4",
		"COL5", "COL6", "COL7", "COL8", "COL9",
		"COL10", "COL11", "COL12"
	};

	m_column = data & 0x0f;
	row = (data>>4) & 0x07;

	bit = 0;

	if (m_column < 13)
	{
		res = ioport(colnames[m_column])->read();
	}
	else
	{
		res = 0xff;
	}

	/* Normal keyboard result */
	if ((res & (1<<row)) == 0)
	{
		bit = 1;
	}

	if ((res | 1) != 0xff)
	{
		m_via6522_0->write_ca2(1);
	}
	else
	{
		m_via6522_0->write_ca2(0);
	}

	return (data & 0x7f) | (bit<<7);
}


void bbc_state::bbcb_IC32_initialise(bbc_state *state)
{
	m_b0_sound=0x01;             // Write Enable to the sound generator IC
	m_b1_speech_read=0x01;       // READ select on the speech processor
	m_b2_speech_write=0x01;      // WRITE select on the speech processor
	m_b3_keyboard=0x01;          // Keyboard write enable
	m_b4_video0=0x01;            // These two outputs define the number to be added to the start of screen address
	m_b5_video1=0x01;            // in hardware to control hardware scrolling
	m_b6_caps_lock_led=0x01;     // Operates the CAPS lock LED
	m_b7_shift_lock_led=0x01;    // Operates the SHIFT lock LED
}


/* This the BBC Masters Real Time Clock and NVRAM IC */
void bbc_state::MC146818_set(address_space &space)
{
	logerror ("146181 WR=%d DS=%d AS=%d CE=%d \n",m_MC146818_WR,m_MC146818_DS,m_MC146818_AS,m_MC146818_CE);
	mc146818_device *rtc = space.machine().device<mc146818_device>("rtc");

	// if chip enabled
	if (m_MC146818_CE)
	{
		// if data select is set then access the data in the 146818
		if (m_MC146818_DS)
		{
			if (m_MC146818_WR)
			{
				m_via_system_porta=rtc->read(space, 1);
				//logerror("read 146818 data %d \n",m_via_system_porta);
			}
			else
			{
				rtc->write(space, 1, m_via_system_porta);
				//logerror("write 146818 data %d \n",m_via_system_porta);
			}
		}

		// if address select is set then set the address in the 146818
		if (m_MC146818_AS)
		{
			rtc->write(space, 0, m_via_system_porta);
			//logerror("write 146818 address %d \n",m_via_system_porta);
		}
	}
}


WRITE8_MEMBER(bbc_state::bbcb_via_system_write_porta)
{
	//logerror("SYSTEM write porta %d\n",data);

	m_via_system_porta = data;
	if (m_b0_sound == 0)
	{
		//logerror("Doing an unsafe write to the sound chip %d \n",data);
		m_sn->write(space, 0, m_via_system_porta);
	}
	if (m_b3_keyboard == 0)
	{
		//logerror("Doing an unsafe write to the keyboard %d \n",data);
		m_via_system_porta = bbc_keyboard(space, m_via_system_porta);
	}
	if (m_Master) MC146818_set(space);
}


WRITE8_MEMBER(bbc_state::bbcb_via_system_write_portb)
{
	int bit, value;
	bit = data & 0x07;
	value = (data >> 3) & 0x01;

	//logerror("SYSTEM write portb %d %d %d\n",data,bit,value);

	if (value)
	{
		switch (bit)
		{
		case 0:
			if (m_b0_sound == 0)
			{
				m_b0_sound = 1;
			}
			break;
		case 1:
			if (m_Master)
			{
				if (m_MC146818_WR == 0)
				{
					/* BBC MASTER has NVRAM Here */
					m_MC146818_WR = 1;
					MC146818_set(space);
				}
			}
			else
			{
				if (m_b1_speech_read == 0)
				{
					/* VSP TMS 5220 */
					m_b1_speech_read = 1;
				}
			}
			break;
		case 2:
			if (m_Master)
			{
				if (m_MC146818_DS == 0)
				{
					/* BBC MASTER has NVRAM Here */
					m_MC146818_DS = 1;
					MC146818_set(space);
				}
			}
			else
			{
				if (m_b2_speech_write == 0)
				{
					/* VSP TMS 5220 */
					m_b2_speech_write = 1;
				}
			}
			break;
		case 3:
			if (m_b3_keyboard == 0)
			{
				m_b3_keyboard = 1;
			}
			break;
		case 4:
			if (m_b4_video0 == 0)
			{
				m_b4_video0 = 1;
			}
			break;
		case 5:
			if (m_b5_video1 == 0)
			{
				m_b5_video1 = 1;
			}
			break;
		case 6:
			if (m_b6_caps_lock_led == 0)
			{
				m_b6_caps_lock_led = 1;
				/* call caps lock led update */
				output_set_value("capslock_led", m_b6_caps_lock_led);
			}
			break;
		case 7:
			if (m_b7_shift_lock_led == 0)
			{
				m_b7_shift_lock_led = 1;
				/* call shift lock led update */
				output_set_value("shiftlock_led", m_b7_shift_lock_led);
			}
			break;
		}
	}
	else
	{
		switch (bit)
		{
		case 0:
			if (m_b0_sound == 1)
			{
				m_b0_sound = 0;
				m_sn->write(space, 0, m_via_system_porta);
			}
			break;
		case 1:
			if (m_Master)
			{
				if (m_MC146818_WR == 1)
				{
					/* BBC MASTER has NV RAM Here */
					m_MC146818_WR = 0;
					MC146818_set(space);
				}
			}
			else
			{
				if (m_b1_speech_read == 1)
				{
					/* VSP TMS 5220 */
					m_b1_speech_read = 0;
				}
			}
			break;
		case 2:
			if (m_Master)
			{
				if (m_MC146818_DS == 1)
				{
					/* BBC MASTER has NV RAM Here */
					m_MC146818_DS = 0;
					MC146818_set(space);
				}
			}
			else
			{
				if (m_b2_speech_write == 1)
				{
					/* VSP TMS 5220 */
					m_b2_speech_write = 0;
				}
			}
			break;
		case 3:
			if (m_b3_keyboard == 1)
			{
				m_b3_keyboard = 0;
				/* *** call keyboard enabled *** */
				m_via_system_porta=bbc_keyboard(space, m_via_system_porta);
			}
			break;
		case 4:
			if (m_b4_video0 == 1)
			{
				m_b4_video0 = 0;
			}
			break;
		case 5:
			if (m_b5_video1 == 1)
			{
				m_b5_video1 = 0;
			}
			break;
		case 6:
			if (m_b6_caps_lock_led == 1)
			{
				m_b6_caps_lock_led = 0;
				/* call caps lock led update */
				output_set_value("capslock_led", m_b6_caps_lock_led);
			}
			break;
		case 7:
			if (m_b7_shift_lock_led == 1)
			{
				m_b7_shift_lock_led = 0;
				/* call shift lock led update */
				output_set_value("shiftlock_led", m_b7_shift_lock_led);
			}
			break;
		}
	}


	if (m_Master)
	{
		//set the Address Select
		if (m_MC146818_AS != ((data>>7)&1))
		{
			m_MC146818_AS=(data>>7)&1;
			MC146818_set(space);
		}

		//if CE changes
		if (m_MC146818_CE != ((data>>6)&1))
		{
			m_MC146818_CE=(data>>6)&1;
			MC146818_set(space);
		}
	}
}


READ8_MEMBER(bbc_state::bbcb_via_system_read_porta)
{
	//logerror("SYSTEM read porta %d\n",m_via_system_porta);
	return m_via_system_porta;
}

// D4 of portb is joystick fire button 1
// D5 of portb is joystick fire button 2
// D6 VSPINT
// D7 VSPRDY

/* this is the interupt and ready signal from the BBC B Speech processor */
static const int TMSint=1;
static const int TMSrdy=1;

#ifdef UNUSED_FUNCTION
void bbc_state::bbc_TMSint(int status)
{
	TMSint=(!status)&1;
	TMSrdy=(!tms5220_readyq_r())&1;
	via_0_portb_w(0,(0xf | ioport("IN0")->read()|(TMSint<<6)|(TMSrdy<<7)));
}
#endif


READ8_MEMBER(bbc_state::bbcb_via_system_read_portb)
{
	//TMSint=(!tms5220_int_r())&1;
	//TMSrdy=(!tms5220_readyq_r())&1;

	//logerror("SYSTEM read portb %d\n",0xf | input_port(machine, "IN0") | (TMSint<<6)|(TMSrdy<<7));

	return (0xf | ioport("IN0")->read() | (TMSint<<6)|(TMSrdy<<7));
}


WRITE_LINE_MEMBER(bbc_state::bbcb_via_system_irq_w)
{
	m_via_system_irq = state;

	check_interrupts();
}

/**********************************************************************
USER VIA
Port A output is buffered before being connected to the printer connector.
This means that they can only be operated as output lines.
CA1 is pulled high by a 4K7 resistor. CA1 normally acts as an acknowledge
line when a printer is used. CA2 is buffered so that it has become an open
collector output only. It usially acts as the printer strobe line.
***********************************************************************/

/* USER VIA 6522 port B is connected to the BBC user port */
READ8_MEMBER(bbc_state::bbcb_via_user_read_portb)
{
	return 0xff;
}

WRITE8_MEMBER(bbc_state::bbcb_via_user_write_portb)
{
	m_userport = data;
}

WRITE_LINE_MEMBER(bbc_state::bbcb_via_user_irq_w)
{
	m_via_user_irq = state;

	check_interrupts();
}


/**************************************
BBC Joystick Support
**************************************/

UPD7002_GET_ANALOGUE(bbc_state::BBC_get_analogue_input)
{
	switch (channel_number)
	{
		case 0:
			return ((0xff - m_joy0->read()) << 8);
		case 1:
			return ((0xff - m_joy1->read()) << 8);
		case 2:
			return ((0xff - m_joy2->read()) << 8);
		case 3:
			return ((0xff - m_joy3->read()) << 8);
	}

	return 0;
}

UPD7002_EOC(bbc_state::BBC_uPD7002_EOC)
{
	m_via6522_0->write_cb1(data);
}

/***************************************
  BBC 2C199 Serial Interface Cassette
****************************************/


void bbc_state::MC6850_Receive_Clock(int new_clock)
{
	m_rxd_cass = new_clock;
	update_acia_rxd();

	//
	// Somehow the "serial processor" generates 16 clock signals towards
	// the 6850. Exact details are unknown, faking it with the following
	// loop.
	//
	for (int i = 0; i < 16; i++ )
	{
		m_acia->write_rxc(1);
		m_acia->write_rxc(0);
	}
}

TIMER_CALLBACK_MEMBER(bbc_state::bbc_tape_timer_cb)
{
	if ( m_cass_out_enabled )
	{
		// 0 = 18-18 18-17-1
		// 1 = 9-9-9-9 9-9-9-8-1

		switch ( m_cass_out_samples_to_go )
		{
			case 0:
				if ( m_cass_out_phase == 0 )
				{
					// get bit value
					m_cass_out_bit = m_txd;
					if ( m_cass_out_bit )
					{
						m_cass_out_phase = 3;
						m_cass_out_samples_to_go = 9;
					}
					else
					{
						m_cass_out_phase = 1;
						m_cass_out_samples_to_go = 18;
					}
					m_cassette->output( +1.0 );
				}
				else
				{
					// switch phase
					m_cass_out_phase--;
					m_cass_out_samples_to_go = m_cass_out_bit ? 9 : 18;
					m_cassette->output( ( m_cass_out_phase & 1 ) ? +1.0 : -1.0 );
				}
				break;
			case 1:
				if ( m_cass_out_phase == 0 )
				{
					m_cassette->output( 0.0 );
				}
				break;
		}

		m_cass_out_samples_to_go--;
	}
	else
	{
		double dev_val = m_cassette->input();

		// look for edges on the cassette wave
		if (((dev_val>=0.0) && (m_last_dev_val<0.0)) || ((dev_val<0.0) && (m_last_dev_val>=0.0)))
		{
			if (m_wav_len>(9*3))
			{
				//this is too long to receive anything so reset the serial IC. This is a hack, this should be done as a timer in the MC6850 code.
				logerror ("Cassette length %d\n",m_wav_len);
				m_nr_high_tones = 0;
				m_dcd_cass = 0;
				update_acia_dcd();
				m_len0=0;
				m_len1=0;
				m_len2=0;
				m_len3=0;
				m_wav_len=0;
			}

			m_len3=m_len2;
			m_len2=m_len1;
			m_len1=m_len0;
			m_len0=m_wav_len;

			m_wav_len=0;
			logerror ("cassette  %d  %d  %d  %d\n",m_len3,m_len2,m_len1,m_len0);

			if ((m_len0+m_len1)>=(18+18-5))
			{
				/* Clock a 0 onto the serial line */
				logerror("Serial value 0\n");
				m_nr_high_tones = 0;
				m_dcd_cass = 0;
				update_acia_dcd();
				MC6850_Receive_Clock(0);
				m_len0=0;
				m_len1=0;
				m_len2=0;
				m_len3=0;
			}

			if (((m_len0+m_len1+m_len2+m_len3)<=41) && (m_len3!=0))
			{
				/* Clock a 1 onto the serial line */
				logerror("Serial value 1\n");
				m_nr_high_tones++;
				if ( m_nr_high_tones > 100 )
				{
					m_dcd_cass = 1;
					update_acia_dcd();
				}
				MC6850_Receive_Clock(1);
				m_len0=0;
				m_len1=0;
				m_len2=0;
				m_len3=0;
			}
		}

		m_wav_len++;
		m_last_dev_val=dev_val;
	}
}

WRITE_LINE_MEMBER( bbc_state::write_rxd_serial )
{
	m_rxd_serial = state;
	update_acia_rxd();
}

void bbc_state::update_acia_rxd()
{
	m_acia->write_rxd(( m_serproc_data & 0x40 ) ? m_rxd_serial : m_rxd_cass);
}


WRITE_LINE_MEMBER( bbc_state::write_dcd_serial )
{
	m_dcd_serial = state;
	update_acia_dcd();
}

void bbc_state::update_acia_dcd()
{
	m_acia->write_dcd(( m_serproc_data & 0x40 ) ? m_dcd_serial : m_dcd_cass);
}


WRITE_LINE_MEMBER( bbc_state::write_cts_serial )
{
	m_cts_serial = state;
	update_acia_cts();
}

void bbc_state::update_acia_cts()
{
	m_acia->write_cts(( m_serproc_data & 0x40 ) ? m_cts_serial : 0);
}


WRITE_LINE_MEMBER( bbc_state::bbc_rts_w )
{
	if ( m_serproc_data & 0x40 )
	{
		m_rs232->write_rts(state);
		m_cass_out_enabled = 0;
	}
	else
	{
		m_cass_out_enabled = state ? 0 : 1;
	}
}


WRITE_LINE_MEMBER( bbc_state::bbc_txd_w )
{
	if ( m_serproc_data & 0x40 )
	{
		m_rs232->write_txd(state);
	}
	else
	{
		m_txd = state;
	}
}


void bbc_state::BBC_Cassette_motor(unsigned char status)
{
	if (status)
	{
		m_cassette->change_state(CASSETTE_MOTOR_ENABLED, CASSETTE_MASK_MOTOR);
		m_tape_timer->adjust(attotime::zero, 0, attotime::from_hz(44100));
	}
	else
	{
		m_cassette->change_state(CASSETTE_MOTOR_DISABLED, CASSETTE_MASK_MOTOR);
		m_tape_timer->reset();
		m_len0 = 0;
		m_len1 = 0;
		m_len2 = 0;
		m_len3 = 0;
		m_wav_len = 0;
		m_cass_out_phase = 0;
		m_cass_out_samples_to_go = 4;
	}
	output_set_value("motor_led", !status);
}


//
// Serial processor control
// x--- ---- - Motor OFF(0)/ON(1)
// -x-- ---- - Cassette(0)/RS243 input(1)
// --xx x--- - Receive baud rate generator control
// ---- -xxx - Transmit baud rate generator control
//             These possible settings apply to both the receive
//             and transmit baud generator control bits:
//             000 - 16MHz / 13 /   1 - 19200 baud
//             001 - 16MHz / 13 /  16 -  1200 baud
//             010 - 16MHz / 13 /   4 -  4800 baud
//             011 - 16MHz / 13 / 128 -   150 baud
//             100 - 16MHz / 13 /   2 -  9600 baud
//             101 - 16MHz / 13 /  64 -   300 baud
//             110 - 16MHz / 13 /   8 -  2400 baud
//             110 - 16MHz / 13 / 256 -    75 baud
//
WRITE8_MEMBER(bbc_state::bbc_SerialULA_w)
{
	static const int serial_clocks[8] =
	{
		1,    // 000
		16,   // 001
		4,    // 010
		128,  // 011
		2,    // 100
		64,   // 101
		8,    // 110
		256   // 111
	};

	m_serproc_data = data;
	update_acia_rxd();
	update_acia_dcd();
	update_acia_cts();
	BBC_Cassette_motor(m_serproc_data & 0x80);

	// Set transmit clock rate
	m_acia_clock->set_clock_scale( (double) 1 / serial_clocks[ data & 0x07 ] );
}

WRITE_LINE_MEMBER(bbc_state::write_acia_clock)
{
	m_acia->write_txc(state);

	if (m_serproc_data & 0x40)
		m_acia->write_rxc(state);
}

/**************************************
   i8271 disc control function
***************************************/


WRITE_LINE_MEMBER(bbc_state::bbc_i8271_interrupt)
{
	/* I'm assuming that the nmi is edge triggered */
	/* a interrupt from the fdc will cause a change in line state, and
	the nmi will be triggered, but when the state changes because the int
	is cleared this will not cause another nmi */
	/* I'll emulate it like this to be sure */

	if (state!=m_previous_i8271_int_state)
	{
		if (state)
		{
			/* I'll pulse it because if I used hold-line I'm not sure
			it would clear - to be checked */
			m_maincpu->set_input_line(INPUT_LINE_NMI,PULSE_LINE);
		}
	}

	m_previous_i8271_int_state = state;
}


READ8_MEMBER(bbc_state::bbc_i8271_read)
{
	int ret;
	logerror("i8271 read %d  ",offset);
	switch (offset)
	{
		case 0:
		case 1:
		case 2:
		case 3:
			/* 8271 registers */
			ret=m_i8271->read(space, offset);
			logerror("  %d\n",ret);
			break;
		case 4:
			ret=m_i8271->data_r(space, offset);
			logerror("  %d\n",ret);
			break;
		default:
			ret=0x0ff;
			break;
	}
	logerror("  void\n");
	return ret;
}

WRITE8_MEMBER(bbc_state::bbc_i8271_write)
{
	logerror("i8271 write  %d  %d\n",offset,data);

	switch (offset)
	{
		case 0:
		case 1:
		case 2:
		case 3:
			/* 8271 registers */
			m_i8271->write(space, offset, data);
			return;
		case 4:
			m_i8271->data_w(space, offset, data);
			return;
		default:
			break;
	}
}



/**************************************
   WD1770 disc control function
***************************************/


/*
   B/ B+ drive control:

        Bit       Meaning
        -----------------
        7,6       Not used.
         5        Reset drive controller chip. (0 = reset controller, 1 = no reset)
         4        Interrupt Enable (0 = enable int, 1 = disable int)
         3        Double density select (0 = double, 1 = single).
         2        Side select (0 = side 0, 1 = side 1).
         1        Drive select 1.
         0        Drive select 0.
*/

/*
density select
single density is as the 8271 disc format
double density is as the 8271 disc format but with 16 sectors per track

At some point we need to check the size of the disc image to work out if it is a single or double
density disc image
*/


/* wd177x_IRQ_SET and latch bit 4 (nmi_enable) are NAND'ED together
   wd177x_DRQ_SET and latch bit 4 (nmi_enable) are NAND'ED together
   the output of the above two NAND gates are then OR'ED together and sent to the 6502 NMI line.
    DRQ and IRQ are active low outputs from wd177x. We use wd177x_DRQ_SET for DRQ = 0,
    and wd177x_DRQ_CLR for DRQ = 1. Similarly wd177x_IRQ_SET for IRQ = 0 and wd177x_IRQ_CLR
    for IRQ = 1.

  The above means that if IRQ or DRQ are set, a interrupt should be generated.
  The nmi_enable decides if interrupts are actually triggered.
  The nmi is edge triggered, and triggers on a +ve edge.
*/

void bbc_state::bbc_update_fdq_int(int state)
{
	int bbc_state;

	/* if drq or irq is set, and interrupt is enabled */
	if ((m_wd177x_irq_state || m_wd177x_drq_state) && (m_1770_IntEnabled))
	{
		/* int trigger */
		bbc_state = 1;
	}
	else
	{
		/* do not trigger int */
		bbc_state = 0;
	}

	/* nmi is edge triggered, and triggers when the state goes from clear->set.
	Here we are checking this transition before triggering the nmi */
	if (bbc_state!=m_previous_wd177x_int_state)
	{
		if (bbc_state)
		{
			/* I'll pulse it because if I used hold-line I'm not sure
			it would clear - to be checked */
			m_maincpu->set_input_line(INPUT_LINE_NMI,PULSE_LINE);
		}
	}

	m_previous_wd177x_int_state = bbc_state;
}

WRITE_LINE_MEMBER(bbc_state::bbc_wd177x_intrq_w)
{
	m_wd177x_irq_state = state;
	bbc_update_fdq_int(state);
}

WRITE_LINE_MEMBER(bbc_state::bbc_wd177x_drq_w)
{
	m_wd177x_drq_state = state;
	bbc_update_fdq_int(state);
}

WRITE8_MEMBER(bbc_state::bbc_wd177x_status_w)
{
	wd1770_device *fdc = machine().device<wd1770_device>("wd177x");
	m_drive_control = data;

	/* set drive */
	if ((data>>0) & 0x01) fdc->set_drive(0);
	if ((data>>1) & 0x01) fdc->set_drive(1);

	/* set side */
	fdc->set_side((data>>2) & 0x01);

	/* set density */
	fdc->dden_w(BIT(data, 3));

	m_1770_IntEnabled=(((data>>4) & 0x01)==0);
}



READ8_MEMBER(bbc_state::bbc_wd1770_read)
{
	int retval=0xff;
	wd1770_device *fdc = machine().device<wd1770_device>("wd177x");
	switch (offset)
	{
	case 4:
		retval=fdc->status_r(space, 0);
		break;
	case 5:
		retval=fdc->track_r(space, 0);
		break;
	case 6:
		retval=fdc->sector_r(space, 0);
		break;
	case 7:
		retval=fdc->data_r(space, 0);
		break;
	default:
		break;
	}
	logerror("wd177x read: $%02X  $%02X\n", offset,retval);

	return retval;
}

WRITE8_MEMBER(bbc_state::bbc_wd1770_write)
{
	wd1770_device *fdc = machine().device<wd1770_device>("wd177x");
	logerror("wd177x write: $%02X  $%02X\n", offset,data);
	switch (offset)
	{
	case 0:
		bbc_wd177x_status_w(space, 0, data);
		break;
	case 4:
		fdc->command_w(space, 0, data);
		break;
	case 5:
		fdc->track_w(space, 0, data);
		break;
	case 6:
		fdc->sector_w(space, 0, data);
		break;
	case 7:
		fdc->data_w(space, 0, data);
		break;
	default:
		break;
	}
}


/*********************************************
OPUS CHALLENGER MEMORY MAP
        Read                        Write

&FCF8   1770 Status register        1770 command register
&FCF9               1770 track register
&FCFA               1770 sector register
&FCFB               1770 data register
&FCFC                               1770 drive control


drive control register bits
0   select side 0= side 0   1= side 1
1   select drive 0
2   select drive 1
3   ?unused?
4   ?Always Set
5   Density Select 0=double, 1=single
6   ?unused?
7   ?unused?

The RAM is accessible through JIM (page &FD). One page is visible in JIM at a time.
The selected page is controlled by the two paging registers:

&FCFE       Paging register MSB
&FCFF       Paging register LSB

256K model has 1024 pages &000 to &3ff
512K model has 2048 pages &000 to &7ff

AM_RANGE(0xfc00, 0xfdff) AM_READWRITE(bbc_opus_read     , bbc_opus_write    )


**********************************************/


WRITE8_MEMBER(bbc_state::bbc_opus_status_w)
{
	wd1770_device *fdc = machine().device<wd1770_device>("wd177x");
	m_drive_control = data;

	/* set drive */
	if ((data>>1) & 0x01) fdc->set_drive(0);
	if ((data>>2) & 0x01) fdc->set_drive(1);

	/* set side */
	fdc->set_side((data>>0) & 0x01);

	/* set density */
	fdc->dden_w(BIT(data, 5));

	m_1770_IntEnabled=(data>>4) & 0x01;
}

READ8_MEMBER(bbc_state::bbc_opus_read)
{
	wd1770_device *fdc = machine().device<wd1770_device>("wd177x");
	logerror("wd177x read: $%02X\n", offset);

	if (m_DFSType==6)
	{
		if (offset<0x100)
		{
			switch (offset)
			{
				case 0xf8:
					return fdc->status_r(space, 0);
				case 0xf9:
					return fdc->track_r(space, 0);
				case 0xfa:
					return fdc->sector_r(space, 0);
				case 0xfb:
					return fdc->data_r(space, 0);
			}
		}
		else
		{
			return memregion("disks")->base()[offset + (m_opusbank << 8)];
		}
	}
	return 0xff;
}

WRITE8_MEMBER(bbc_state::bbc_opus_write)
{
	wd1770_device *fdc = machine().device<wd1770_device>("wd177x");
	logerror("wd177x write: $%02X  $%02X\n", offset,data);

	if (m_DFSType==6)
	{
		if (offset<0x100)
		{
			switch (offset)
			{
				case 0xf8:
					fdc->command_w(space, 0, data);
					break;
				case 0xf9:
					fdc->track_w(space, 0, data);
					break;
				case 0xfa:
					fdc->sector_w(space, 0, data);
					break;
				case 0xfb:
					fdc->data_w(space, 0, data);
					break;
				case 0xfc:
					bbc_opus_status_w(space, 0,data);
					break;
				case 0xfe:
					m_opusbank=(m_opusbank & 0xff) | (data<<8);
					break;
				case 0xff:
					m_opusbank=(m_opusbank & 0xff00) | data;
					break;
			}
		}
		else
		{
			memregion("disks")->base()[offset + (m_opusbank << 8)] = data;
		}
	}
}


/***************************************
BBC MASTER DISC SUPPORT
***************************************/


READ8_MEMBER(bbc_state::bbcm_wd1770_read)
{
	int retval=0xff;
	wd1770_device *fdc = machine().device<wd1770_device>("wd177x");
	switch (offset)
	{
	case 0:
		retval=fdc->status_r(space, 0);
		break;
	case 1:
		retval=fdc->track_r(space, 0);
		break;
	case 2:
		retval=fdc->sector_r(space, 0);
		break;
	case 3:
		retval=fdc->data_r(space, 0);
		break;
	default:
		break;
	}
	return retval;
}


WRITE8_MEMBER(bbc_state::bbcm_wd1770_write)
{
	wd1770_device *fdc = machine().device<wd1770_device>("wd177x");
	//logerror("wd177x write: $%02X  $%02X\n", offset,data);
	switch (offset)
	{
	case 0:
		fdc->command_w(space, 0, data);
		break;
	case 1:
		fdc->track_w(space, 0, data);
		break;
	case 2:
		fdc->sector_w(space, 0, data);
		break;
	case 3:
		fdc->data_w(space, 0, data);
		break;
	default:
		break;
	}
}


READ8_MEMBER(bbc_state::bbcm_wd1770l_read)
{
	return m_drive_control;
}

WRITE8_MEMBER(bbc_state::bbcm_wd1770l_write)
{
	wd1770_device *fdc = machine().device<wd1770_device>("wd177x");
	m_drive_control = data;

	/* set drive */
	if ((data>>0) & 0x01) fdc->set_drive(0);
	if ((data>>1) & 0x01) fdc->set_drive(1);

	/* set side */
	fdc->set_side((data>>4) & 0x01);

	/* set density */
	fdc->dden_w(BIT(data, 5));

//  m_1770_IntEnabled=(((data>>4) & 0x01)==0);
	m_1770_IntEnabled=1;

}


/**************************************
DFS Hardware mapping for different Disc Controller types
***************************************/

READ8_MEMBER(bbc_state::bbc_disc_r)
{
	switch (m_DFSType){
	/* case 0 to 3 are all standard 8271 interfaces */
	case 0: case 1: case 2: case 3:
		return bbc_i8271_read(space, offset);
	/* case 4 is the acorn 1770 interface */
	case 4:
		return bbc_wd1770_read(space, offset);
	/* case 5 is the watford 1770 interface */
	case 5:
		return bbc_wd1770_read(space, offset);
	/* case 6 is the Opus challenger interface */
	case 6:
		/* not connected here, opus drive is connected via the 1MHz Bus */
		break;
	/* case 7 in no disc controller */
	case 7:
		break;
	}
	return 0x0ff;
}

WRITE8_MEMBER(bbc_state::bbc_disc_w)
{
	switch (m_DFSType){
	/* case 0 to 3 are all standard 8271 interfaces */
	case 0: case 1: case 2: case 3:
		bbc_i8271_write(space, offset,data);
		break;
	/* case 4 is the acorn 1770 interface */
	case 4:
		bbc_wd1770_write(space, offset,data);
		break;
	/* case 5 is the watford 1770 interface */
	case 5:
		bbc_wd1770_write(space, offset,data);
		break;
	/* case 6 is the Opus challenger interface */
	case 6:
		/* not connected here, opus drive is connected via the 1MHz Bus */
		break;
	/* case 7 in no disc controller */
	case 7:
		break;
	}
}



/**************************************
   BBC B Rom loading functions
***************************************/

int bbc_state::bbc_load_cart(device_image_interface &image, generic_slot_device *slot)
{
	UINT32 size = slot->common_get_size("rom");

	if (size != 0x2000 && size != 0x4000)
	{
		image.seterror(IMAGE_ERROR_UNSPECIFIED, "Unsupported cartridge size");
		return IMAGE_INIT_FAIL;
	}

	slot->rom_alloc(size, GENERIC_ROM8_WIDTH, ENDIANNESS_LITTLE);
	slot->common_load_rom(slot->get_rom_base(), size, "rom");

	return IMAGE_INIT_PASS;
}

/**************************************
   BBC Master Rom loading functions
***************************************/

int bbc_state::bbcm_load_cart(device_image_interface &image, generic_slot_device *slot)
{
	if (image.software_entry() == NULL)
	{
		UINT32 filesize = image.length();

		if (filesize != 0x8000)
		{
			image.seterror(IMAGE_ERROR_UNSPECIFIED, "Unsupported cartridge size");
			return IMAGE_INIT_FAIL;
		}

		slot->rom_alloc(filesize, GENERIC_ROM8_WIDTH, ENDIANNESS_LITTLE);
		image.fread(slot->get_rom_base(), filesize);
		return IMAGE_INIT_PASS;
	}
	else
	{
		UINT32 size_lo = image.get_software_region_length("lorom");
		UINT32 size_hi = image.get_software_region_length("uprom");

		if (size_lo + size_hi != 0x8000)
		{
			image.seterror(IMAGE_ERROR_UNSPECIFIED, "Unsupported cartridge size");
			return IMAGE_INIT_FAIL;
		}

		slot->rom_alloc(size_lo + size_hi, GENERIC_ROM8_WIDTH, ENDIANNESS_LITTLE);
		memcpy(slot->get_rom_base() + 0,       image.get_software_region("uprom"), size_hi);
		memcpy(slot->get_rom_base() + size_hi, image.get_software_region("lorom"), size_lo);
	}

	return IMAGE_INIT_PASS;
}


/**************************************
   Machine Initialisation functions
***************************************/

DRIVER_INIT_MEMBER(bbc_state,bbc)
{
	m_Master=0;
	m_rxd_cass = 0;
	m_nr_high_tones = 0;
	m_serproc_data = 0;
	m_cass_out_enabled = 0;
	m_tape_timer = machine().scheduler().timer_alloc(timer_expired_delegate(FUNC(bbc_state::bbc_tape_timer_cb),this));

	/* vertical sync pulse from video circuit */
	m_via6522_0->write_ca1(1);

	/* light pen strobe detect (not emulated) */
	m_via6522_0->write_cb2(1);
}

DRIVER_INIT_MEMBER(bbc_state,bbcm)
{
	m_Master=1;
	m_rxd_cass = 0;
	m_nr_high_tones = 0;
	m_serproc_data = 0;
	m_cass_out_enabled = 0;
	m_tape_timer = machine().scheduler().timer_alloc(timer_expired_delegate(FUNC(bbc_state::bbc_tape_timer_cb),this));

	/* vertical sync pulse from video circuit */
	m_via6522_0->write_ca1(1);

	/* light pen strobe detect (not emulated) */
	m_via6522_0->write_cb2(1);
}

// setup pointers for optional EPROMs
void bbc_state::bbc_setup_banks(memory_bank *membank, int banks, UINT32 shift, UINT32 size)
{
	astring region_tag;
	memory_region *tmp_reg;
	UINT8 *eprom[4];
	if (m_exp1 && (tmp_reg = memregion(region_tag.cpy(m_exp1->tag()).cat(GENERIC_ROM_REGION_TAG))))
		eprom[0] = tmp_reg->base() + shift;
	else
		eprom[0] = m_region_opt->base() + 0x0000 + shift;
	if (m_exp2 && (tmp_reg = memregion(region_tag.cpy(m_exp2->tag()).cat(GENERIC_ROM_REGION_TAG))))
		eprom[1] = tmp_reg->base() + shift;
	else
		eprom[1] = m_region_opt->base() + 0x4000 + shift;
	if (m_exp3 && (tmp_reg = memregion(region_tag.cpy(m_exp3->tag()).cat(GENERIC_ROM_REGION_TAG))))
		eprom[2] = tmp_reg->base() + shift;
	else
		eprom[2] = m_region_opt->base() + 0x8000 + shift;
	if (m_exp4 && (tmp_reg = memregion(region_tag.cpy(m_exp4->tag()).cat(GENERIC_ROM_REGION_TAG))))
		eprom[3] = tmp_reg->base() + shift;
	else
		eprom[3] = m_region_opt->base() + 0xc000 + shift;

	membank->configure_entries(0, 1,  eprom[0], size);
	membank->configure_entries(1, 1,  eprom[1], size);
	membank->configure_entries(2, 1,  eprom[2], size);
	membank->configure_entries(3, 1,  eprom[3], size);

	if (banks > 4)
	{
		for (int i = 0; i < banks - 4; i++)
			membank->configure_entries(i + 4, 1,  m_region_opt->base() + 0x10000 + shift + i * 0x4000, size);
	}
}

void bbc_state::bbcm_setup_banks(memory_bank *membank, int banks, UINT32 shift, UINT32 size)
{
	astring region_tag;
	memory_region *tmp_reg;
	UINT8 *eprom[2];
	if (m_exp1 && (tmp_reg = memregion(region_tag.cpy(m_exp1->tag()).cat(GENERIC_ROM_REGION_TAG))))
		eprom[0] = tmp_reg->base() + shift;
	else
		eprom[0] = m_region_opt->base() + 0x0000 + shift;
	if (m_exp2 && (tmp_reg = memregion(region_tag.cpy(m_exp2->tag()).cat(GENERIC_ROM_REGION_TAG))))
		eprom[1] = tmp_reg->base() + shift;
	else
		eprom[1] = m_region_opt->base() + 0x8000 + shift;

	membank->configure_entries(0, 1,  eprom[0], size);
	membank->configure_entries(1, 1,  eprom[0] + 0x4000, size);
	membank->configure_entries(2, 1,  eprom[1], size);
	membank->configure_entries(3, 1,  eprom[1] + 0x4000, size);

	if (banks > 4)
	{
		for (int i = 0; i < banks - 4; i++)
			membank->configure_entries(i + 4, 1,  m_region_opt->base() + 0x10000 + shift + i * 0x4000, size);
	}
}

MACHINE_START_MEMBER(bbc_state, bbca)
{
	bbc_setup_banks(m_bank4, 4, 0, 0x4000);
}

MACHINE_RESET_MEMBER(bbc_state, bbca)
{
	UINT8 *RAM = m_region_maincpu->base();

	m_bank1->set_base(RAM);
	if (m_ram->size() == 32*1024)
	{
		/* 32K Model A */
		m_bank3->set_base(RAM + 0x4000);
		m_memorySize=32;
	}
	else
	{
		/* 16K just repeat the lower 16K*/
		m_bank3->set_base(RAM);
		m_memorySize=16;
	}

	m_bank4->set_entry(0);
	m_bank7->set_base(m_region_os->base());  /* bank 7 points at the OS rom  from c000 to ffff */

	bbcb_IC32_initialise(this);
}

MACHINE_START_MEMBER(bbc_state, bbcb)
{
	m_mc6850_clock = 0;
	m_previous_i8271_int_state=0;
	m_previous_wd177x_int_state=1;
	bbc_setup_banks(m_bank4, 16, 0, 0x4000);
	if (m_region_dfs)
		m_bank4->configure_entries(16, 8, m_region_dfs->base(), 0x4000);    // additional bank for paged ram
}

MACHINE_RESET_MEMBER(bbc_state, bbcb)
{
	UINT8 *RAM = m_region_maincpu->base();
	m_DFSType=    (ioport("BBCCONFIG")->read() >> 0) & 0x07;
	m_SWRAMtype = (ioport("BBCCONFIG")->read() >> 3) & 0x03;
	m_bank1->set_base(RAM);
	m_bank3->set_base(RAM + 0x4000);
	m_memorySize=32;

	m_bank4->set_entry(0);
	m_bank7->set_base(m_region_os->base());  /* bank 7 points at the OS rom  from c000 to ffff */

	bbcb_IC32_initialise(this);

	m_opusbank = 0;
}


MACHINE_START_MEMBER(bbc_state, bbcbp)
{
	m_mc6850_clock = 0;

	m_maincpu->space(AS_PROGRAM).set_direct_update_handler(direct_update_delegate(FUNC(bbc_state::bbcbp_direct_handler), this));

	bbc_setup_banks(m_bank4, 16, 0, 0x3000);
	m_bank4->configure_entries(16, 1, m_region_maincpu->base() + 0x8000, 0x3000);   // additional bank for paged ram
	bbc_setup_banks(m_bank6, 16, 0x3000, 0x1000);
}

MACHINE_RESET_MEMBER(bbc_state, bbcbp)
{
	m_bank1->set_base(m_region_maincpu->base());
	m_bank2->set_base(m_region_maincpu->base() + 0x03000);  /* bank 2 screen/shadow ram     from 3000 to 7fff */
	m_bank4->set_entry(0);
	m_bank6->set_entry(0);
	m_bank7->set_base(m_region_os->base());    /* bank 7 points at the OS rom  from c000 to ffff */

	bbcb_IC32_initialise(this);

	m_previous_wd177x_int_state=1;
}


MACHINE_START_MEMBER(bbc_state, bbcm)
{
	m_mc6850_clock = 0;

	m_maincpu->space(AS_PROGRAM).set_direct_update_handler(direct_update_delegate(FUNC(bbc_state::bbcm_direct_handler), this));

	bbcm_setup_banks(m_bank4, 16, 0, 0x1000);
	m_bank4->configure_entries(16, 1, m_region_maincpu->base() + 0x8000, 0x1000);   // additional bank for paged ram
	bbcm_setup_banks(m_bank5, 16, 0x1000, 0x3000);

	/* Set ROM/IO bank to point to rom */
	m_bank8->set_base(m_region_os->base() + 0x3c00);
	m_maincpu->space(AS_PROGRAM).install_read_bank(0xfc00, 0xfeff, "bank8");
}

MACHINE_RESET_MEMBER(bbc_state, bbcm)
{
	m_bank1->set_base(m_region_maincpu->base());           /* bank 1 regular lower ram     from 0000 to 2fff */
	m_bank2->set_base(m_region_maincpu->base() + 0x3000);  /* bank 2 screen/shadow ram     from 3000 to 7fff */
	m_bank4->set_entry(0);
	m_bank5->set_entry(0);
	m_bank7->set_base(m_region_os->base());   /* bank 6 OS rom of RAM         from c000 to dfff */

	bbcb_IC32_initialise(this);

	m_previous_wd177x_int_state=1;
}
