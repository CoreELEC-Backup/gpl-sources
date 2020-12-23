/*********************************************************************

    ui/cheatopt.c

    Internal menu for the cheat interface.

    Copyright Nicola Salmoria and the MAME Team.
    Visit http://mamedev.org for licensing and usage restrictions.

*********************************************************************/

#include "emu.h"
#include "cheat.h"

#include "uiinput.h"
#include "ui/ui.h"
#include "ui/cheatopt.h"

/*-------------------------------------------------
    menu_cheat - handle the cheat menu
-------------------------------------------------*/

void ui_menu_cheat::handle()
{
	/* process the menu */
	const ui_menu_event *menu_event = process(UI_MENU_PROCESS_LR_REPEAT);

	/* handle events */
	if (menu_event != NULL && menu_event->itemref != NULL)
	{
		bool changed = false;

		/* clear cheat comment on any movement or keypress */
		popmessage(NULL);

		/* handle reset all + reset all cheats for reload all option */
		if ((FPTR)menu_event->itemref < 3 && menu_event->iptkey == IPT_UI_SELECT)
		{
			for (cheat_entry *curcheat = machine().cheat().first(); curcheat != NULL; curcheat = curcheat->next())
				if (curcheat->select_default_state())
					changed = true;
		}


		/* handle individual cheats */
		else if ((FPTR)menu_event->itemref > 2)
		{
			cheat_entry *curcheat = reinterpret_cast<cheat_entry *>(menu_event->itemref);
			const char *string;
			switch (menu_event->iptkey)
			{
				/* if selected, activate a oneshot */
				case IPT_UI_SELECT:
					changed = curcheat->activate();
					break;

				/* if cleared, reset to default value */
				case IPT_UI_CLEAR:
					changed = curcheat->select_default_state();
					break;

				/* left decrements */
				case IPT_UI_LEFT:
					changed = curcheat->select_previous_state();
					break;

				/* right increments */
				case IPT_UI_RIGHT:
					changed = curcheat->select_next_state();
					break;

				/* bring up display comment if one exists */
				case IPT_UI_DISPLAY_COMMENT:
				case IPT_UI_UP:
				case IPT_UI_DOWN:
					string = curcheat->comment();
					if (string != NULL && string[0] != 0)
						popmessage("Cheat Comment:\n%s", string);
					break;
			}
		}

		/* handle reload all  */
		if ((FPTR)menu_event->itemref == 2 && menu_event->iptkey == IPT_UI_SELECT)
		{
			/* re-init cheat engine and thus reload cheats/cheats have already been turned off by here */
			machine().cheat().reload();

			/* display the reloaded cheats */
			reset(UI_MENU_RESET_REMEMBER_REF);
			popmessage("All cheats reloaded");
		}

		/* if things changed, update */
		if (changed)
			reset(UI_MENU_RESET_REMEMBER_REF);
	}
}


/*-------------------------------------------------
    menu_cheat_populate - populate the cheat menu
-------------------------------------------------*/

ui_menu_cheat::ui_menu_cheat(running_machine &machine, render_container *container) : ui_menu(machine, container)
{
}

void ui_menu_cheat::populate()
{
	/* iterate over cheats */
	astring text;
	astring subtext;
	for (cheat_entry *curcheat = machine().cheat().first(); curcheat != NULL; curcheat = curcheat->next())
	{
		UINT32 flags;
		curcheat->menu_text(text, subtext, flags);
		item_append(text, subtext, flags, curcheat);
	}

	/* add a separator */
	item_append(MENU_SEPARATOR_ITEM, NULL, 0, NULL);

	/* add a reset all option */
	item_append("Reset All", NULL, 0, (void *)1);

	/* add a reload all cheats option */
	item_append("Reload All", NULL, 0, (void *)2);
}

ui_menu_cheat::~ui_menu_cheat()
{
}
