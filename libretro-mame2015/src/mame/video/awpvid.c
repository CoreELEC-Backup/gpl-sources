/*************************************************************************************

    AWP Hardware video simulation system
    originally written for AGEMAME by J.Wallace, enhanced by D.Haywood

    M.A.M.E Core Copyright Nicola Salmoria and the MAME Team.

    This is a primitive handler for generating reels with multiple symbols visible
    hanging off steppers.c .

    TODO: Add any lamping persistance simulations we need.

**************************************************************************************/

#include "emu.h"
#include "awpvid.h"
#include "machine/steppers.h"


void awp_draw_reel(const char* reeltag, stepper_device &reel)
{
	char rg[16];

	int reelpos =  reel.get_position();
	if (reelpos == output_get_value(reeltag))
	{
		// Not moved, no need to update.
	}
	else
	{
		output_set_value(reeltag,(reelpos));

		// if the reel isn't configured don't do this, otherwise you'll get DIV0
		if (reel.get_max())
		{
			sprintf(rg,"s%s", reeltag); // our new scrolling reels are called 'sreel'
			// normalize the value
			int sreelpos = (reelpos * 0x10000) / reel.get_max();

			output_set_value(rg,sreelpos);
		}
	}
}
