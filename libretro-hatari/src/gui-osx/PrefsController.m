/*
  Hatari - PrefsController.m

  This file is distributed under the GNU General Public License, version 2
  or at your option any later version. Read the file gpl.txt for details.

  Preferences window controller implementation file

  Feb-Mar 2006, Sébastien Molines - Created
  Jan 2006, Sébastien Molines - Updated for recent emulator updates
  Miguel SARO, J. VERNET
*/
// bOKDialog = Dialog_MainDlg(&bForceReset, &bLoadedSnapshot);   // prise des préférences
// TODO: Set the default paths to MacOS-friendly values
// TODO: Move hardcoded string to localizable resources (e.g. string "Reset the emulator?")


#import "PrefsController.h"
#import "Shared.h"

#include "main.h"
#include "configuration.h"
#include "change.h"
#include "dialog.h"
#include "file.h"
#include "floppy.h"
#include "screen.h"
#include "sdlgui.h"
#include "paths.h"
#include "keymap.h"

// Macros to transfer data between Cocoa controls and Hatari data structures

// de l'affichage vers la structure  (saveAllControls)
#define EXPORT_TEXTFIELD(mutablStrng, target) [mutablStrng getCString:target maxLength:sizeof((target))-1 encoding:NSASCIIStringEncoding]
#define EXPORT_NTEXTFIELD(nstextfield, target) target = [nstextfield intValue]
#define EXPORT_SWITCH(nsbutton, target) target = ([(nsbutton) state] == NSOnState)
#define EXPORT_RADIO(nsmatrix, target) target = [[(nsmatrix) selectedCell] tag]
#define EXPORT_DROPDOWN(nspopupbutton, target) target = [[(nspopupbutton) selectedItem] tag]
#define EXPORT_SLIDER(nsslider, target) target = [(nsslider) intValue]

// la structure vers l'affichage (setAllControls)
#define IMPORT_TEXTFIELD(nstextfield, mutablStrng, source) [mutablStrng setString:[NSString stringWithCString:(source) encoding:NSASCIIStringEncoding]] ; [nstextfield setStringValue:[NSApp pathUser:mutablStrng]]
#define IMPORT_NTEXTFIELD(nstextfield, source) [(nstextfield) setIntValue:(source)]
#define IMPORT_SWITCH(nsbutton, source) [(nsbutton) setState:((source))? NSOnState : NSOffState]
#define IMPORT_RADIO(nsmatrix, source) [(nsmatrix) selectCellWithTag:(source)]
#define IMPORT_DROPDOWN(nspopupbutton, source) [(nspopupbutton) selectItemAtIndex:[(nspopupbutton) indexOfItemWithTag:(source)]]
#define IMPORT_SLIDER(nsslider,source) [(nsslider) setIntValue:source]

#define INITIAL_DIR(dossier) [dossier length] < 2 ? @"~" : dossier

// Back up of the current configuration parameters
//
CNF_PARAMS CurrentParams;


// Keys to be listed in the Joysticks dropdowns
SDLKey Preferences_KeysForJoysticks[] =
{
	SDLK_BACKSPACE,
	SDLK_TAB,
	SDLK_CLEAR,
	SDLK_RETURN,
	SDLK_PAUSE,
	SDLK_ESCAPE,
	SDLK_SPACE,
	SDLK_EXCLAIM,
	SDLK_QUOTEDBL,
	SDLK_HASH,
	SDLK_DOLLAR,
	SDLK_AMPERSAND,
	SDLK_QUOTE,
	SDLK_LEFTPAREN,
	SDLK_RIGHTPAREN,
	SDLK_ASTERISK,
	SDLK_PLUS,
	SDLK_COMMA,
	SDLK_MINUS,
	SDLK_PERIOD,
	SDLK_SLASH,
	SDLK_0,
	SDLK_1,
	SDLK_2,
	SDLK_3,
	SDLK_4,
	SDLK_5,
	SDLK_6,
	SDLK_7,
	SDLK_8,
	SDLK_9,
	SDLK_COLON,
	SDLK_SEMICOLON,
	SDLK_LESS,
	SDLK_EQUALS,
	SDLK_GREATER,
	SDLK_QUESTION,
	SDLK_AT,
	SDLK_LEFTBRACKET,
	SDLK_BACKSLASH,
	SDLK_RIGHTBRACKET,
	SDLK_CARET,
	SDLK_UNDERSCORE,
	SDLK_BACKQUOTE,
	SDLK_a,
	SDLK_b,
	SDLK_c,
	SDLK_d,
	SDLK_e,
	SDLK_f,
	SDLK_g,
	SDLK_h,
	SDLK_i,
	SDLK_j,
	SDLK_k,
	SDLK_l,
	SDLK_m,
	SDLK_n,
	SDLK_o,
	SDLK_p,
	SDLK_q,
	SDLK_r,
	SDLK_s,
	SDLK_t,
	SDLK_u,
	SDLK_v,
	SDLK_w,
	SDLK_x,
	SDLK_y,
	SDLK_z,
	SDLK_DELETE,
	SDLK_KP0,
	SDLK_KP1,
	SDLK_KP2,
	SDLK_KP3,
	SDLK_KP4,
	SDLK_KP5,
	SDLK_KP6,
	SDLK_KP7,
	SDLK_KP8,
	SDLK_KP9,
	SDLK_KP_PERIOD,
	SDLK_KP_DIVIDE,
	SDLK_KP_MULTIPLY,
	SDLK_KP_MINUS,
	SDLK_KP_PLUS,
	SDLK_KP_ENTER,
	SDLK_KP_EQUALS,
	SDLK_UP,
	SDLK_DOWN,
	SDLK_RIGHT,
	SDLK_LEFT,
	SDLK_INSERT,
	SDLK_HOME,
	SDLK_END,
	SDLK_PAGEUP,
	SDLK_PAGEDOWN,
	SDLK_F1,
	SDLK_F2,
	SDLK_F3,
	SDLK_F4,
	SDLK_F5,
	SDLK_F6,
	SDLK_F7,
	SDLK_F8,
	SDLK_F9,
	SDLK_F10,
	SDLK_F11,
	SDLK_F12,
	SDLK_F13,
	SDLK_F14,
	SDLK_F15,
	SDLK_NUMLOCK,
	SDLK_CAPSLOCK,
	SDLK_SCROLLOCK,
	SDLK_RSHIFT,
	SDLK_LSHIFT,
	SDLK_RCTRL,
	SDLK_LCTRL,
	SDLK_RALT,
	SDLK_LALT,
	SDLK_RMETA,
	SDLK_LMETA,
#if !WITH_SDL2
	SDLK_LSUPER,
	SDLK_RSUPER,
	SDLK_COMPOSE,
	SDLK_BREAK,
	SDLK_EURO,
#endif
	SDLK_MODE,
	SDLK_HELP,
	SDLK_PRINT,
	SDLK_SYSREQ,
	SDLK_MENU,
	SDLK_POWER,
	SDLK_UNDO
};

size_t Preferences_cKeysForJoysticks = sizeof(Preferences_KeysForJoysticks) / sizeof(Preferences_KeysForJoysticks[0]);

#define DLGSOUND_11KHZ      0
#define DLGSOUND_12KHZ      1
#define DLGSOUND_16KHZ      2
#define DLGSOUND_22KHZ      3
#define DLGSOUND_25KHZ      4
#define DLGSOUND_32KHZ      5
#define DLGSOUND_44KHZ      6
#define DLGSOUND_48KHZ      7
#define DLGSOUND_50KHZ      8

static const int nSoundFreqs[] =
{
	11025,
	12517,
	16000,
	22050,
	25033,
	32000,
	44100,
	48000,
	50066
};


@implementation PrefsController

char szPath[FILENAME_MAX];


- (IBAction)finished:(id)sender
{
	Main_RequestQuit(0) ;
}

/*-----------------------------------------------------------------------*/
/*                                                                       */
/*  Helper method for Choose buttons                                     */
/*  Returns: TRUE is the user selected a path, FALSE if he/she aborted   */
/*-----------------------------------------------------------------------*/
- (BOOL)choosePathForControl:(NSTextField*)textField chooseDirectories:(BOOL)chooseDirectories defaultInitialDir:(NSString*)defaultInitialDir 
																					mutString:(NSMutableString *)mutString what:(NSArray *)what
{
	NSString *directoryToOpen ;
	NSString *fileToPreselect ;
	NSString *newPath ;

	if ((mutString != nil) && ([mutString length] > 2))
	 {	directoryToOpen = [mutString stringByDeletingLastPathComponent];			// There is existing path: we use it.
		fileToPreselect = [mutString lastPathComponent]; }
	else
	 {	directoryToOpen = [defaultInitialDir stringByExpandingTildeInPath];			// no path: use user's directory
		fileToPreselect = nil; } ;

	newPath = [NSApp hopenfile:chooseDirectories defoDir:directoryToOpen defoFile:fileToPreselect types:what];
	if ([newPath length] != 0)														// user canceled if empty
	{
		[mutString setString:[NSString stringWithString:newPath]] ;					// save this path
		[textField setStringValue:[NSApp pathUser:newPath]];						// show localized path
		return YES;
	} ;

	return NO;																		// Selection aborted
}


//-----------------------------------------------------------------------*/
//
//  Helper method to insert a floppy image
//  TODO: Add code to restrict to known file types
//
/*-----------------------------------------------------------------------*/

- (void)insertFloppyImageIntoDrive:(int)drive forTextField:(NSTextField*)floppyTextField  realPath:(NSMutableString *)realPath
{
	if ([self choosePathForControl:floppyTextField  chooseDirectories:NO defaultInitialDir:imgeDir
											mutString:realPath  what:[NSArray arrayWithObjects:allF,nil]])
		
		Floppy_SetDiskFileName(drive, [realPath cStringUsingEncoding:NSASCIIStringEncoding], NULL);
		// Insert the floppy image at this path  ????
}


//-----------------------------------------------------------------------------
- (NSString *)initial:(NSString *)route
{
BOOL flag1, flag2;

	if ((route==nil) || ([route length]==0))  return @"~" ;
	flag1 = [[NSFileManager defaultManager] fileExistsAtPath:route isDirectory:&flag2] ;
	if (flag1 && !flag2)
		return route ;
	return [route stringByDeletingLastPathComponent] ;
}

//
//  Methods for all the "Choose" buttons
//
- (IBAction)chooseCartridgeImage:(id)sender;
{
	[self choosePathForControl: cartridgeImage chooseDirectories:NO defaultInitialDir:[self initial:cartridge]			// cartridge
												mutString:cartridge  what:[NSArray arrayWithObjects:allC,nil]];
}


- (IBAction)chooseDefaultImagesLocation:(id)sender
{
	[self choosePathForControl: defaultImagesLocation chooseDirectories:YES defaultInitialDir:[self initial:imgeDir]	// images location
												mutString:imgeDir  what:nil];
}

- (IBAction)chooseFloppyImageA:(id)sender
{
	[self insertFloppyImageIntoDrive:0 forTextField:floppyImageA realPath:floppyA];										// floppy A
}

- (IBAction)chooseFloppyImageB:(id)sender
{
	[self insertFloppyImageIntoDrive:1 forTextField:floppyImageB realPath:floppyB];										// floppy B
}

- (IBAction)chooseGemdosImage:(id)sender																				// directory for Gemdos
{
	[self choosePathForControl: gemdosImage chooseDirectories:YES defaultInitialDir:INITIAL_DIR(gemdos)					// gemdos
												mutString:gemdos  what:nil] ;
	if ([gemdos length] >2 ) [gemdosImage setStringValue:[NSApp pathUser:gemdos]] ;
}

- (IBAction)chooseHdImage:(id)sender
{
	[self choosePathForControl: hdImage chooseDirectories:NO defaultInitialDir:[self initial:hrdDisk]					// HD image ?
												mutString:hrdDisk  what:[NSArray arrayWithObjects:@"img",@"hdv",nil]] ;
}

- (IBAction)chooseIdeMasterHdImage:(id)sender
{
	[self choosePathForControl: ideMasterHdImage chooseDirectories:NO defaultInitialDir:[self initial:masterIDE]		// IDE master
												mutString:masterIDE  what:[NSArray arrayWithObject:@"hdv"]];
}

- (IBAction)chooseIdeSlaveHdImage:(id)sender
{
	[self choosePathForControl: ideSlaveHdImage chooseDirectories:NO defaultInitialDir:[self initial:slaveIDE]			// IDE slave
												mutString:slaveIDE  what:[NSArray arrayWithObject:@"hdv"]];
}

- (IBAction)chooseKeyboardMappingFile:(id)sender
{
	[self choosePathForControl: keyboardMappingFile chooseDirectories:NO defaultInitialDir:[self initial:keyboard]		// keyboard mapping
												mutString:keyboard  what:[NSArray arrayWithObjects:@"txt",@"map",nil]];
}

- (IBAction)chooseMidiOutputFile:(id)sender
{
	[self choosePathForControl: writeMidiToFile chooseDirectories:NO defaultInitialDir:[self initial:midiOut]			// midi output 
												mutString:midiOut  what:[NSArray arrayWithObject:@"mid"]];
}

- (IBAction)choosePrintToFile:(id)sender
{
	[self choosePathForControl: printToFile chooseDirectories:NO defaultInitialDir:[self initial:printit]				// print to file
												mutString:printit  what:[NSArray arrayWithObject:@"prn"]];
}

- (IBAction)chooseRS232InputFile:(id)sender
{
	[self choosePathForControl: readRS232FromFile chooseDirectories:NO defaultInitialDir:[self initial:rs232In]			// RS232 input
												mutString:rs232In  what:nil];
}

- (IBAction)chooseRS232OutputFile:(id)sender
{
	[self choosePathForControl: writeRS232ToFile chooseDirectories:NO defaultInitialDir:[self initial:rs232Out]			// RS232 output
												mutString:rs232Out  what:nil];
}

- (IBAction)chooseTosImage:(id)sender;
{
	[self choosePathForControl: tosImage chooseDirectories:NO defaultInitialDir:[self initial:TOS]						// TOS image
												mutString:TOS  what:[NSArray arrayWithObjects:allT,nil]];
}


/*-----------------------------------------------------------------------*/
/*                                                                       */
/*  Methods for the "Eject" buttons                                      */
/*-----------------------------------------------------------------------*/
- (IBAction)ejectFloppyA:(id)sender
{
	Floppy_SetDiskFileNameNone(0);
	
	// Refresh  control & mutablestring
	[floppyImageA setStringValue:@""];
	[floppyA setString:@""] ;
}

- (IBAction)ejectFloppyB:(id)sender
{
	Floppy_SetDiskFileNameNone(1);

	// Refresh  control & mutablestring
	[floppyImageB setStringValue:@""];
	[floppyB setString:@""] ;
}

- (IBAction)ejectGemdosImage:(id)sender
{
	// Clear the control. Later. saveAllControls will set the ConfigureParams accordingly to signal this is ejected
	[gemdosImage setStringValue:@""];
	[gemdos setString:@""] ;
}

- (IBAction)ejectHdImage:(id)sender
{
	// Clear the control. Later. saveAllControls will set the ConfigureParams accordingly to signal this is ejected
	[hdImage setStringValue:@""];
	[hrdDisk setString:@""] ;
}

- (IBAction)ejectIdeMasterHdImage:(id)sender
{
	// Clear the control. Later. saveAllControls will set the ConfigureParams accordingly to signal this is ejected
	[ideMasterHdImage setStringValue:@""];
	[masterIDE setString:@""] ;
}

- (IBAction)ejectIdeSlaveHdImage:(id)sender
{
	// Clear the control. Later. saveAllControls will set the ConfigureParams accordingly to signal this is ejected
	[ideSlaveHdImage setStringValue:@""];
	[slaveIDE setString:@""] ;
}

/*-----------------------------------------------------------------------*/
/**
 * Methods for the "Load Config" button
 */

- (IBAction)loadConfigFrom:(id)sender
{
	NSArray		*lesURLs  ;
	NSString	*ru ;
	BOOL		btOk ;

	ru = [NSString stringWithCString:(Paths_GetHatariHome()) encoding:NSASCIIStringEncoding] ;
	[opnPanel setAllowedFileTypes:[NSArray arrayWithObject:@"cfg"]] ;
	[opnPanel setCanChooseDirectories: NO];
	[opnPanel setCanChooseFiles: YES];
	[opnPanel setAccessoryView:partage] ;

	if ([opnPanel respondsToSelector:@selector(setDirectoryURL:)])
	 {	[opnPanel setDirectoryURL:[NSURL fileURLWithPath:ru isDirectory:YES]] ;
		[opnPanel setNameFieldStringValue:@"hatari"] ;
		btOk = [opnPanel runModal] == NSOKButton ;										// Ok ?
	 }
	else
		btOk = [opnPanel runModalForDirectory:ru file:@"hatari"] == NSOKButton	;

	if (!btOk)  return ;																// Cancel

	lesURLs = [opnPanel URLs] ;
	if ((lesURLs == nil) || ([lesURLs count] == 0))
		return ;

	[configNm setString:[[lesURLs objectAtIndex:0] path]] ;

		// Make a non-const C string out of it
	[configNm getCString:sConfigFileName maxLength:FILENAME_MAX encoding:NSASCIIStringEncoding];

		// Load the config into ConfigureParams
	Configuration_Load(sConfigFileName);

		// Refresh all the controls to match ConfigureParams
	[self setAllControls];
}

/**
 * Methods for the "Save Config" button  (bottom preference window)
 */
- (IBAction)saveConfigAs:(id)sender
{
NSString	*ru ;
BOOL		btOk ;

	ru = [NSString stringWithCString:(Paths_GetHatariHome()) encoding:NSASCIIStringEncoding] ;
	[savPanel setAllowedFileTypes:[NSArray arrayWithObject:@"cfg"]] ;
	[savPanel setAccessoryView:hartage] ;

	if ([savPanel respondsToSelector:@selector(setDirectoryURL:)])
	 {	[savPanel setDirectoryURL:[NSURL fileURLWithPath:ru isDirectory:YES]] ;			// Since OS X 10.6
		[savPanel setNameFieldStringValue:@"hatari"] ;
		btOk = [savPanel runModal] == NSOKButton ;											// Ok ?
	 }
	else
		btOk = [savPanel runModalForDirectory:ru file:@"hatari"] == NSOKButton ;		// avant 10.6

	if (!btOk)
		return ;                                                                        // Cancel

	[configNm setString:[[savPanel URL] path]];

																	// Make a non-const C string out of it
	[configNm getCString:sConfigFileName maxLength:FILENAME_MAX encoding:NSASCIIStringEncoding];
	[self saveAllControls] ;										// Save the config from ConfigureParams		
	Configuration_Save();											// [self configSave:configNm] ;
}

- (IBAction)aller:(id)sender
{
NSString  *defaultDirectory ;

	defaultDirectory = [NSString stringWithCString:(Paths_GetHatariHome()) encoding:NSASCIIStringEncoding] ;
	if ([opnPanel respondsToSelector:@selector(setDirectoryURL:)])
		[opnPanel setDirectoryURL:[NSURL fileURLWithPath:defaultDirectory isDirectory:YES]] ;
	else
		[opnPanel setDirectory:defaultDirectory] ;
}

- (IBAction)halle:(id)sender
{
NSString  *defaultDirectory ;

	defaultDirectory = [NSString stringWithCString:(Paths_GetHatariHome()) encoding:NSASCIIStringEncoding] ;
	if ([savPanel respondsToSelector:@selector(setDirectoryURL:)])
		[savPanel setDirectoryURL:[NSURL fileURLWithPath:defaultDirectory isDirectory:YES]] ;
	else 
		[savPanel setDirectory:defaultDirectory] ;
}


/*-----------------------------------------------------------------------*/
/*
  Commits and closes         Ok button in preferences window
*/
- (IBAction)commitAndClose:(id)sender
{

	// The user clicked OK
	[self saveAllControls];
	
	[window close] ;


}

// Populate Joystick key dropdown

- (void)initKeysDropDown:(NSPopUpButton*)dropDown
{
	[dropDown removeAllItems];
	unsigned int i;
	for (i = 0; i < Preferences_cKeysForJoysticks; i++)
	{
		SDLKey key = Preferences_KeysForJoysticks[i];
		const char* szKeyName = SDL_GetKeyName(key);
		[dropDown addItemWithTitle:[[NSString stringWithCString:szKeyName encoding:NSASCIIStringEncoding] capitalizedString]];	
		[[dropDown lastItem] setTag:key];
	}
}


/*-----------------------------------------------------------------------*/
/*
  Displays the Preferences dialog   Ouverture de la fenêtre des préférences
*/
- (IBAction)loadPrefs:(id)sender
{
	[configNm setString:[NSString stringWithCString:sConfigFileName encoding:NSASCIIStringEncoding]] ;

	if (!bInitialized)
	{
		// Note: These inits cannot be done in awakeFromNib as by this time SDL is not yet initialized.

		// Fill the keyboard dropdowns
		[self initKeysDropDown:joystickUp];
		[self initKeysDropDown:joystickRight];
		[self initKeysDropDown:joystickDown];
		[self initKeysDropDown:joystickLeft];		
		[self initKeysDropDown:joystickFire];
		
		// Get and store the number of real joysticks
		cRealJoysticks = SDL_NumJoysticks();

		// Fill the real joysticks dropdown, if any are available
		if (cRealJoysticks > 0)
		{
			[realJoystick removeAllItems];
			int i;
			for (i = 0; i < cRealJoysticks; i++)
			{
				const char* szJoystickName = SDL_JoystickName(i);
				[realJoystick addItemWithTitle:[[NSString stringWithCString:szJoystickName encoding:NSASCIIStringEncoding] capitalizedString]];	
				[[realJoystick lastItem] setTag:i];	
			}
		}
		else	// No real joysticks: Disable the controls
		{
			[[joystickMode cellWithTag:1] setEnabled:FALSE];
			[realJoystick setEnabled:FALSE];
		}
		
		bInitialized = true;
	}


	// Backup of configuration settings to CurrentParams (which we will only
	// commit back to the configuration settings if choosing OK)
	CurrentParams = ConfigureParams;
	applyChanges=false;

	[self setAllControls];

	// Display the window
	ModalWrapper *mw=[[ModalWrapper alloc] init];
	
	[mw runModal:window];
	
	[mw release];								// */
	
	// solve bug screen-reset: close and kill preference windows before
	// M. Saro, 2013

    //if(Ok button in preferences Windows)
    {
        // Check if change need reset
        if (Change_DoNeedReset(&CurrentParams, &ConfigureParams))
        {
            applyChanges = [NSApp myAlerte:NSInformationalAlertStyle Txt:nil firstB:localize(@"Don't reset") alternateB:localize(@"Reset")
											otherB:nil informativeTxt:localize(@"Must be reset") ] == NSAlertAlternateReturn ;
			if (applyChanges)
				Change_CopyChangedParamsToConfiguration(&CurrentParams, &ConfigureParams, true) ;
            else
                ConfigureParams = CurrentParams;    //Restore backup params
        }
        else
            Change_CopyChangedParamsToConfiguration(&CurrentParams, &ConfigureParams, false); //Apply config without reset
    }
    // else // not OK button
    // {
    //      ConfigureParams = CurrentParams; //Restore backup params
    // }
    
}

/*-----------------------------------------------------------------------*/
/*
  Updates the controls following a change in the joystick selection
*/
- (IBAction)changeViewedJoystick:(id)sender
{
	// Save the pre-joystick controls, as we are about to change them
	[self saveJoystickControls];
	
	// Refresh the per-joystick controls
	[self setJoystickControls];
	
	// Update the controls' enabled states
	[self updateEnabledStates:self];
}


/*-----------------------------------------------------------------------*/
/*
  Initializes all controls, transfert des préférences dans la fenêtre
*/
- (void)setAllControls
{

	// Import the floppy paths into their controls.
	IMPORT_TEXTFIELD(floppyImageA, floppyA, ConfigureParams.DiskImage.szDiskFileName[0]);				// le A
	IMPORT_TEXTFIELD(floppyImageB, floppyB, ConfigureParams.DiskImage.szDiskFileName[1]);				// le B

	// Import all the preferences into their controls
	IMPORT_TEXTFIELD(cartridgeImage, cartridge, ConfigureParams.Rom.szCartridgeImageFileName);
	IMPORT_TEXTFIELD(defaultImagesLocation, imgeDir, ConfigureParams.DiskImage.szDiskImageDirectory);
	IMPORT_TEXTFIELD(keyboardMappingFile, keyboard, ConfigureParams.Keyboard.szMappingFileName);
	IMPORT_TEXTFIELD(printToFile, printit, ConfigureParams.Printer.szPrintToFileName);
	IMPORT_TEXTFIELD(tosImage, TOS, ConfigureParams.Rom.szTosImageFileName);
	IMPORT_TEXTFIELD(configFile, configNm, sConfigFileName);
	IMPORT_TEXTFIELD(readRS232FromFile, rs232In, ConfigureParams.RS232.szInFileName);
	IMPORT_TEXTFIELD(writeRS232ToFile, rs232Out, ConfigureParams.RS232.szOutFileName);
	
	IMPORT_SWITCH(autoInsertB, ConfigureParams.DiskImage.bAutoInsertDiskB);
	IMPORT_SWITCH(blitter, ConfigureParams.System.bBlitter);
	IMPORT_SWITCH(bootFromHD, ConfigureParams.HardDisk.bBootFromHardDisk);
	IMPORT_SWITCH(captureOnChange, ConfigureParams.Screen.bCrop);
	IMPORT_RADIO(colorDepth, ConfigureParams.Screen.nVdiColors);
	IMPORT_SWITCH(compatibleCpu, ConfigureParams.System.bCompatibleCpu);
	IMPORT_RADIO(cpuClock, ConfigureParams.System.nCpuFreq);
	IMPORT_RADIO(cpuType, ConfigureParams.System.nCpuLevel);
	IMPORT_SWITCH(enableMidi, ConfigureParams.Midi.bEnableMidi);
	IMPORT_SWITCH(enablePrinter, ConfigureParams.Printer.bEnablePrinting);
	IMPORT_SWITCH(enableRS232, ConfigureParams.RS232.bEnableRS232);
	IMPORT_SWITCH(enableSound, ConfigureParams.Sound.bEnableSound);
	IMPORT_DROPDOWN(frameSkip, ConfigureParams.Screen.nFrameSkips);
	IMPORT_RADIO(keyboardMapping, ConfigureParams.Keyboard.nKeymapType);
	IMPORT_RADIO(machineType, ConfigureParams.System.nMachineType);
	IMPORT_RADIO(monitor, ConfigureParams.Screen.nMonitorType);
	IMPORT_SWITCH(patchTimerD, ConfigureParams.System.bPatchTimerD);
	IMPORT_RADIO(ramSize, ConfigureParams.Memory.nMemorySize);
	IMPORT_SWITCH(realTime, ConfigureParams.System.bRealTimeClock);
	IMPORT_SWITCH(fastFDC, ConfigureParams.DiskImage.FastFloppy);
	IMPORT_SWITCH(useBorders, ConfigureParams.Screen.bAllowOverscan);
	IMPORT_SWITCH(useVDIResolution, ConfigureParams.Screen.bUseExtVdiResolutions);
	IMPORT_RADIO(floppyWriteProtection, ConfigureParams.DiskImage.nWriteProtection);
	IMPORT_RADIO(HDWriteProtection, ConfigureParams.HardDisk.nWriteProtection);
	// IMPORT_SWITCH(zoomSTLowRes, ConfigureParams.Screen.bZoomLowRes);
	IMPORT_SWITCH(showStatusBar, ConfigureParams.Screen.bShowStatusbar);
	IMPORT_DROPDOWN(enableDSP,ConfigureParams.System.nDSPType);

	// 12/04/2010
	IMPORT_SWITCH(falconTTRatio, ConfigureParams.Screen.bAspectCorrect);
	IMPORT_SWITCH(fullScreen, ConfigureParams.Screen.bFullScreen);
	IMPORT_SWITCH(ledDisks, ConfigureParams.Screen.bShowDriveLed);
	IMPORT_SWITCH(keepDesktopResolution, ConfigureParams.Screen.bKeepResolution);
	
	//v1.6.1
	IMPORT_SWITCH(FastBootPatch,ConfigureParams.System.bFastBoot);
	IMPORT_RADIO(YMVoicesMixing,ConfigureParams.Sound.YmVolumeMixing);
	
	//deal with the Max Zoomed Stepper
	IMPORT_NTEXTFIELD(maxZoomedWidth, ConfigureParams.Screen.nMaxWidth);
	IMPORT_NTEXTFIELD(maxZoomedHeight, ConfigureParams.Screen.nMaxHeight);
	
	[widthStepper setDoubleValue:[maxZoomedWidth intValue]];
	[heightStepper setDoubleValue:[maxZoomedHeight intValue]];
	
	
	[(force8bpp) setState:((ConfigureParams.Screen.nForceBpp==8))? NSOnState : NSOffState];

	
	int i;
	
	for (i = 0; i <= DLGSOUND_50KHZ-DLGSOUND_11KHZ; i++)
	{
		if (ConfigureParams.Sound.nPlaybackFreq > nSoundFreqs[i]-500
		    && ConfigureParams.Sound.nPlaybackFreq < nSoundFreqs[i]+500)
		{
			[playbackQuality selectCellWithTag:(i)];
			break;
		}
	}
	
	
	if (ConfigureParams.Screen.nVdiWidth >= 1024)
		[resolution selectCellWithTag:(2)];
	else if (ConfigureParams.Screen.nVdiWidth >= 768)
		[resolution selectCellWithTag:(1)];
	else
		[resolution selectCellWithTag:(0)];

	// If the HD flag is set, load the HD path, otherwise make it blank
	if (ConfigureParams.Acsi[0].bUseDevice)
	{
		IMPORT_TEXTFIELD(hdImage, hrdDisk, ConfigureParams.Acsi[0].sDeviceFile);
	}
	else
	{
		[hdImage setStringValue:@""]; [hrdDisk setString:@""] ;
	}
	
	// If the IDE HD flag is set, load the IDE HD path, otherwise make it blank
	//Master
	if (ConfigureParams.HardDisk.bUseIdeMasterHardDiskImage)
	{
		IMPORT_TEXTFIELD(ideMasterHdImage, masterIDE, ConfigureParams.HardDisk.szIdeMasterHardDiskImage);
	}
	else
	{
		[ideMasterHdImage setStringValue:@""]; [masterIDE setString:@""] ;
	}
	//Slave
	if (ConfigureParams.HardDisk.bUseIdeSlaveHardDiskImage)
	{
		IMPORT_TEXTFIELD(ideSlaveHdImage, slaveIDE, ConfigureParams.HardDisk.szIdeSlaveHardDiskImage);
	}
	else
	{
		[ideSlaveHdImage setStringValue:@""]; [slaveIDE setString:@""] ;
	}
	
	// If the Gemdos flag is set, load the Gemdos path, otherwise make it blank
	if (ConfigureParams.HardDisk.bUseHardDiskDirectories)
	{
		[gemdos setString:[NSString stringWithCString:(ConfigureParams.HardDisk.szHardDiskDirectories[0]) encoding:NSASCIIStringEncoding]] ;
//		[gemdosImage setStringValue:[NSApp pathUser:[gemdos stringByDeletingLastPathComponent]]] ;
		[gemdosImage setStringValue:[NSApp pathUser:gemdos]] ;
	}
	else
	{
		[gemdosImage setStringValue:@""]; [gemdos setString:@""];
	}
	
	// Set the per-joystick controls
	[self setJoystickControls];
	
	// Update the controls' enabled states
	[self updateEnabledStates:self];
}


/*------------------------------------------------------------------------*/
/* Updates the enabled states of controls who depend on other controls    */
/**************************************************************************/

- (IBAction)updateEnabledStates:(id)sender
{
	// Joystick key controls are only enabled if "Use keyboard" is selected
	int nJoystickMode;
	EXPORT_RADIO(joystickMode, nJoystickMode);
	BOOL bUsingKeyboard = (nJoystickMode == JOYSTICK_KEYBOARD);
	[joystickUp setEnabled:bUsingKeyboard];
	[joystickRight setEnabled:bUsingKeyboard];
	[joystickDown setEnabled:bUsingKeyboard];
	[joystickLeft setEnabled:bUsingKeyboard];
	[joystickFire setEnabled:bUsingKeyboard];

	// Resolution and colour depth depend on Extended GEM VDI resolution
	BOOL bUsingVDI;
	EXPORT_SWITCH(useVDIResolution, bUsingVDI);
	[resolution setEnabled:bUsingVDI];
	[colorDepth setEnabled:bUsingVDI];
	
	// Playback quality depends on enable sound
	BOOL bSoundEnabled;
	EXPORT_SWITCH(enableSound, bSoundEnabled);
	[playbackQuality setEnabled:bSoundEnabled];
}


/*-----------------------------------------------------------------------*/
/* Updates the joystick controls to match the new joystick selection     */
/*************************************************************************/

- (void)setJoystickControls
{
	// Get and persist the ID of the newly selected joystick
	EXPORT_DROPDOWN(currentJoystick, nCurrentJoystick);

	// Data validation: If the JoyID is out of bounds, correct it and, if set to use real joystick, change to disabled
	if ( (ConfigureParams.Joysticks.Joy[nCurrentJoystick].nJoyId < 0)
	|| (ConfigureParams.Joysticks.Joy[nCurrentJoystick].nJoyId >= cRealJoysticks) )
	{
		ConfigureParams.Joysticks.Joy[nCurrentJoystick].nJoyId = 0;
		if (ConfigureParams.Joysticks.Joy[nCurrentJoystick].nJoystickMode == JOYSTICK_REALSTICK)
		{
			ConfigureParams.Joysticks.Joy[nCurrentJoystick].nJoystickMode = JOYSTICK_DISABLED;
		}	
	}

	// Don't change the realJoystick dropdown if none is available (to keep "(None available)" selected)
	if (cRealJoysticks > 0)
	{
		IMPORT_DROPDOWN(realJoystick, ConfigureParams.Joysticks.Joy[nCurrentJoystick].nJoyId);
	}

	IMPORT_RADIO(joystickMode, ConfigureParams.Joysticks.Joy[nCurrentJoystick].nJoystickMode);
	IMPORT_DROPDOWN(joystickUp, ConfigureParams.Joysticks.Joy[nCurrentJoystick].nKeyCodeUp);
	IMPORT_DROPDOWN(joystickRight, ConfigureParams.Joysticks.Joy[nCurrentJoystick].nKeyCodeRight);
	IMPORT_DROPDOWN(joystickDown, ConfigureParams.Joysticks.Joy[nCurrentJoystick].nKeyCodeDown);
	IMPORT_DROPDOWN(joystickLeft, ConfigureParams.Joysticks.Joy[nCurrentJoystick].nKeyCodeLeft);
	IMPORT_DROPDOWN(joystickFire, ConfigureParams.Joysticks.Joy[nCurrentJoystick].nKeyCodeFire);
	IMPORT_SWITCH(enableAutoFire, ConfigureParams.Joysticks.Joy[nCurrentJoystick].bEnableAutoFire);
}


/*-----------------------------------------------------------------------*/
/*   Saves the setting for the joystick currently being viewed           */
- (void)saveJoystickControls
{
	EXPORT_RADIO(joystickMode, ConfigureParams.Joysticks.Joy[nCurrentJoystick].nJoystickMode);	
	EXPORT_DROPDOWN(realJoystick, ConfigureParams.Joysticks.Joy[nCurrentJoystick].nJoyId);
	EXPORT_DROPDOWN(joystickUp, ConfigureParams.Joysticks.Joy[nCurrentJoystick].nKeyCodeUp);
	EXPORT_DROPDOWN(joystickRight, ConfigureParams.Joysticks.Joy[nCurrentJoystick].nKeyCodeRight);
	EXPORT_DROPDOWN(joystickDown, ConfigureParams.Joysticks.Joy[nCurrentJoystick].nKeyCodeDown);
	EXPORT_DROPDOWN(joystickLeft, ConfigureParams.Joysticks.Joy[nCurrentJoystick].nKeyCodeLeft);
	EXPORT_DROPDOWN(joystickFire, ConfigureParams.Joysticks.Joy[nCurrentJoystick].nKeyCodeFire);
	EXPORT_SWITCH(enableAutoFire, ConfigureParams.Joysticks.Joy[nCurrentJoystick].bEnableAutoFire);
}


/*-----------------------------------------------------------------------*/
/* Saves the settings for all controls                                   */
- (void)saveAllControls
{
	// Export the preference controls into their vars

	EXPORT_TEXTFIELD(cartridge, ConfigureParams.Rom.szCartridgeImageFileName);
	EXPORT_TEXTFIELD(imgeDir, ConfigureParams.DiskImage.szDiskImageDirectory);
	EXPORT_TEXTFIELD(keyboard, ConfigureParams.Keyboard.szMappingFileName);
	EXPORT_TEXTFIELD(printit, ConfigureParams.Printer.szPrintToFileName);
	EXPORT_TEXTFIELD(rs232In, ConfigureParams.RS232.szInFileName);
	EXPORT_TEXTFIELD(TOS, ConfigureParams.Rom.szTosImageFileName);
	EXPORT_TEXTFIELD(midiOut, ConfigureParams.Midi.sMidiOutFileName);
	EXPORT_TEXTFIELD(rs232Out, ConfigureParams.RS232.szOutFileName);

	//EXPORT_TEXTFIELD(cartridgeImage, ConfigureParams.Rom.szCartridgeImageFileName);
	
	EXPORT_SWITCH(autoInsertB, ConfigureParams.DiskImage.bAutoInsertDiskB);
	EXPORT_SWITCH(blitter, ConfigureParams.System.bBlitter);
	EXPORT_SWITCH(bootFromHD, ConfigureParams.HardDisk.bBootFromHardDisk);
	EXPORT_SWITCH(captureOnChange, ConfigureParams.Screen.bCrop);
	EXPORT_RADIO(colorDepth, ConfigureParams.Screen.nVdiColors);
	EXPORT_SWITCH(compatibleCpu, ConfigureParams.System.bCompatibleCpu);
	EXPORT_RADIO(cpuClock, ConfigureParams.System.nCpuFreq);
	EXPORT_RADIO(cpuType, ConfigureParams.System.nCpuLevel);
	EXPORT_SWITCH(enableMidi, ConfigureParams.Midi.bEnableMidi);
	EXPORT_SWITCH(enablePrinter, ConfigureParams.Printer.bEnablePrinting);
	EXPORT_SWITCH(enableRS232, ConfigureParams.RS232.bEnableRS232);
	EXPORT_SWITCH(enableSound, ConfigureParams.Sound.bEnableSound);
	EXPORT_DROPDOWN(frameSkip, ConfigureParams.Screen.nFrameSkips);
	EXPORT_RADIO(keyboardMapping, ConfigureParams.Keyboard.nKeymapType);
	EXPORT_RADIO(machineType, ConfigureParams.System.nMachineType);
	EXPORT_RADIO(monitor, ConfigureParams.Screen.nMonitorType);
	EXPORT_SWITCH(patchTimerD, ConfigureParams.System.bPatchTimerD);
	EXPORT_RADIO(ramSize, ConfigureParams.Memory.nMemorySize);
	EXPORT_SWITCH(realTime, ConfigureParams.System.bRealTimeClock);
	EXPORT_SWITCH(fastFDC, ConfigureParams.DiskImage.FastFloppy);
	EXPORT_SWITCH(useBorders, ConfigureParams.Screen.bAllowOverscan);
	EXPORT_SWITCH(useVDIResolution, ConfigureParams.Screen.bUseExtVdiResolutions);
	EXPORT_RADIO(floppyWriteProtection, ConfigureParams.DiskImage.nWriteProtection);
	EXPORT_RADIO(HDWriteProtection, ConfigureParams.HardDisk.nWriteProtection);
	// EXPORT_SWITCH(zoomSTLowRes, ConfigureParams.Screen.bZoomLowRes);
	EXPORT_SWITCH(showStatusBar,ConfigureParams.Screen.bShowStatusbar);
	EXPORT_DROPDOWN(enableDSP,ConfigureParams.System.nDSPType);
	
	EXPORT_SWITCH(falconTTRatio, ConfigureParams.Screen.bAspectCorrect);
	EXPORT_SWITCH(fullScreen, ConfigureParams.Screen.bFullScreen);
	EXPORT_SWITCH(ledDisks, ConfigureParams.Screen.bShowDriveLed);
	EXPORT_SWITCH(keepDesktopResolution, ConfigureParams.Screen.bKeepResolution);
	
	//v1.6.1
	EXPORT_SWITCH(FastBootPatch,ConfigureParams.System.bFastBoot);
	EXPORT_RADIO(YMVoicesMixing,ConfigureParams.Sound.YmVolumeMixing);
	
	EXPORT_NTEXTFIELD(maxZoomedWidth, ConfigureParams.Screen.nMaxWidth);
	EXPORT_NTEXTFIELD(maxZoomedHeight, ConfigureParams.Screen.nMaxHeight);

	ConfigureParams.Screen.nForceBpp = ([force8bpp state] == NSOnState) ? 8 : 0;

	ConfigureParams.Sound.nPlaybackFreq = nSoundFreqs[[[playbackQuality selectedCell] tag]];
			
	switch ([[resolution selectedCell] tag])
	{
	 case 0:
		ConfigureParams.Screen.nVdiWidth = 640;
		ConfigureParams.Screen.nVdiHeight = 480;
		break;
	 case 1:
		ConfigureParams.Screen.nVdiWidth = 800;
		ConfigureParams.Screen.nVdiHeight = 600;
		break;
	 case 2:
		ConfigureParams.Screen.nVdiWidth = 1024;
		ConfigureParams.Screen.nVdiHeight = 768;
		break;
	}

	// Define the HD flag, and export the HD path if one is selected
	if ([hrdDisk length] > 0)
	{
		EXPORT_TEXTFIELD(hrdDisk, ConfigureParams.Acsi[0].sDeviceFile);
		ConfigureParams.Acsi[0].bUseDevice = true;
	}
	else
	{
		ConfigureParams.Acsi[0].bUseDevice = false;
	}

	// Define the IDE HD flag, and export the IDE HD path if one is selected
	if ([masterIDE length] > 0)
	{
		EXPORT_TEXTFIELD(masterIDE, ConfigureParams.HardDisk.szIdeMasterHardDiskImage);
		ConfigureParams.HardDisk.bUseIdeMasterHardDiskImage = YES;
	}
	else
	{
		ConfigureParams.HardDisk.bUseIdeMasterHardDiskImage = NO;
	}
	
	// IDE Slave
	if ([slaveIDE length] > 0)
	{
		EXPORT_TEXTFIELD(slaveIDE, ConfigureParams.HardDisk.szIdeSlaveHardDiskImage);
		ConfigureParams.HardDisk.bUseIdeSlaveHardDiskImage = YES;
	}
	else
	{
		ConfigureParams.HardDisk.bUseIdeSlaveHardDiskImage = NO;
	}
	
	// Define the Gemdos flag, and export the Gemdos path if one is selected
	if ([gemdos length] > 0)
	{
		EXPORT_TEXTFIELD(gemdos, ConfigureParams.HardDisk.szHardDiskDirectories[0]);
		ConfigureParams.HardDisk.bUseHardDiskDirectories = YES;
	}
	else
	{
		ConfigureParams.HardDisk.bUseHardDiskDirectories = NO;
	}

	// Save the per-joystick controls
	[self saveJoystickControls];
}

// Max Zoomed Adjust

- (IBAction) setWidth:(id)sender;
{
	NSLog(@"Change Max Zoom width: %d", [sender intValue]);
    [maxZoomedWidth setIntValue: [sender intValue]];
}

- (IBAction) setHeight:(id)sender;
{
	NSLog(@"Change Max Zoom height: %d", [sender intValue]);
    [maxZoomedHeight setIntValue: [sender intValue]];
}

+(PrefsController *)prefs
{
	static PrefsController *prefs = nil;
	if (!prefs)
		prefs = [[PrefsController alloc] init];
	
	return prefs;
}																	// */

- (void)awakeFromNib
{
	cartridge = [NSMutableString stringWithCapacity:50] ; [cartridge setString:@""] ; [cartridge retain] ;
	imgeDir = [NSMutableString stringWithCapacity:50] ; [imgeDir setString:@""] ; [imgeDir retain] ;
	floppyA = [NSMutableString stringWithCapacity:50] ; [floppyA setString:@""] ; [floppyA retain] ;
	floppyB = [NSMutableString stringWithCapacity:50] ; [floppyB setString:@""] ; [floppyB retain] ;
	gemdos = [NSMutableString stringWithCapacity:50] ; [gemdos setString:@""] ; [gemdos retain] ;
	hrdDisk = [NSMutableString stringWithCapacity:50] ; [hrdDisk setString:@""] ; [hrdDisk retain] ;
	masterIDE = [NSMutableString stringWithCapacity:50] ; [masterIDE setString:@""] ; [masterIDE retain] ;
	slaveIDE = [NSMutableString stringWithCapacity:50] ; [slaveIDE setString:@""] ; [slaveIDE retain] ;
	keyboard = [NSMutableString stringWithCapacity:50] ; [keyboard setString:@""] ; [keyboard retain] ;
	midiOut = [NSMutableString stringWithCapacity:50] ; [midiOut setString:@""] ; [midiOut retain] ;
	printit = [NSMutableString stringWithCapacity:50] ; [printit setString:@""] ; [printit retain] ;
	rs232In = [NSMutableString stringWithCapacity:50] ; [rs232In setString:@""] ; [rs232In retain] ;
	rs232Out = [NSMutableString stringWithCapacity:50] ; [rs232Out setString:@""] ; [rs232Out retain] ;
	TOS = [NSMutableString stringWithCapacity:50] ; [TOS setString:@""] ; [TOS retain] ;
	configNm = [NSMutableString stringWithCapacity:50] ; [configNm setString:@""] ; [configNm retain] ;
	opnPanel = [NSOpenPanel openPanel]; [opnPanel retain] ;
	savPanel = [NSSavePanel savePanel]; [savPanel retain] ;
}

@end
