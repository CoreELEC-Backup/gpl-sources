// FB Alpha - Emulator for MC68000/Z80 based arcade games
//            Refer to the "license.txt" file for more info

// Main module

// #define USE_SDL					// define if SDL is used
// #define DONT_DISPLAY_SPLASH		// Prevent Splash screen from being displayed
#define APP_DEBUG_LOG			// log debug messages to zzBurnDebug.html

#ifdef USE_SDL
 #include "SDL.h"
#endif

#include "burner.h"

#ifdef _MSC_VER
//  #include <winable.h>
 #ifdef _DEBUG
  #include <crtdbg.h>
 #endif
#endif

#include "version.h"

HINSTANCE hAppInst = NULL;			// Application Instance
HANDLE hMainThread;
long int nMainThreadID;
int nAppThreadPriority = THREAD_PRIORITY_NORMAL;
int nAppShowCmd;

static TCHAR szCmdLine[1024] = _T("");

HACCEL hAccel = NULL;

int nAppVirtualFps = 6000;			// App fps * 100

TCHAR szAppBurnVer[16] = _T("");
TCHAR szAppExeName[EXE_NAME_SIZE + 1];

bool bCmdOptUsed = 0; // 1 fullscreen, 2 windowed (-w)
bool bAlwaysProcessKeyboardInput = false;
bool bAlwaysCreateSupportFolders = true;
bool bAutoLoadGameList = false;

bool bQuietLoading = false;

bool bNoChangeNumLock = 1;
static bool bNumlockStatus;

bool bMonitorAutoCheck = true;

// Used for the load/save dialog in commdlg.h (savestates, input replay, wave logging)
TCHAR szChoice[MAX_PATH] = _T("");
OPENFILENAME ofn;

#if defined (UNICODE)
char* TCHARToANSI(const TCHAR* pszInString, char* pszOutString, int nOutSize)
{

	static char szStringBuffer[1024];
	memset(szStringBuffer, 0, sizeof(szStringBuffer));

	char* pszBuffer = pszOutString ? pszOutString : szStringBuffer;
	int nBufferSize = pszOutString ? nOutSize * 2 : sizeof(szStringBuffer);

	if (WideCharToMultiByte(CP_ACP, 0, pszInString, -1, pszBuffer, nBufferSize, NULL, NULL)) {
		return pszBuffer;
	}

	return NULL;
}

TCHAR* ANSIToTCHAR(const char* pszInString, TCHAR* pszOutString, int nOutSize)
{
	static TCHAR szStringBuffer[1024];

	TCHAR* pszBuffer = pszOutString ? pszOutString : szStringBuffer;
	int nBufferSize  = pszOutString ? nOutSize * 2 : sizeof(szStringBuffer);

	if (MultiByteToWideChar(CP_ACP, 0, pszInString, -1, pszBuffer, nBufferSize)) {
		return pszBuffer;
	}

	return NULL;
}
#else
char* TCHARToANSI(const TCHAR* pszInString, char* pszOutString, int /*nOutSize*/)
{
	if (pszOutString) {
		strcpy(pszOutString, pszInString);
		return pszOutString;
	}

	return (char*)pszInString;
}

TCHAR* ANSIToTCHAR(const char* pszInString, TCHAR* pszOutString, int /*nOutSize*/)
{
	if (pszOutString) {
		_tcscpy(pszOutString, pszInString);
		return pszOutString;
	}

	return (TCHAR*)pszInString;
}
#endif

CHAR *astring_from_utf8(const char *utf8string)
{
	WCHAR *wstring;
	int char_count;
	CHAR *result;

	// convert MAME string (UTF-8) to UTF-16
	char_count = MultiByteToWideChar(CP_UTF8, 0, utf8string, -1, NULL, 0);
	wstring = (WCHAR *)malloc(char_count * sizeof(*wstring));
	MultiByteToWideChar(CP_UTF8, 0, utf8string, -1, wstring, char_count);

	// convert UTF-16 to "ANSI code page" string
	char_count = WideCharToMultiByte(CP_ACP, 0, wstring, -1, NULL, 0, NULL, NULL);
	result = (CHAR *)malloc(char_count * sizeof(*result));
	if (result != NULL)
		WideCharToMultiByte(CP_ACP, 0, wstring, -1, result, char_count, NULL, NULL);

	if (wstring) {
		free(wstring);
		wstring = NULL;
	}
	return result;
}

char *utf8_from_astring(const CHAR *astring)
{
	WCHAR *wstring;
	int char_count;
	CHAR *result;

	// convert "ANSI code page" string to UTF-16
	char_count = MultiByteToWideChar(CP_ACP, 0, astring, -1, NULL, 0);
	wstring = (WCHAR *)malloc(char_count * sizeof(*wstring));
	MultiByteToWideChar(CP_ACP, 0, astring, -1, wstring, char_count);

	// convert UTF-16 to MAME string (UTF-8)
	char_count = WideCharToMultiByte(CP_UTF8, 0, wstring, -1, NULL, 0, NULL, NULL);
	result = (CHAR *)malloc(char_count * sizeof(*result));
	if (result != NULL)
		WideCharToMultiByte(CP_UTF8, 0, wstring, -1, result, char_count, NULL, NULL);

	if (wstring) {
		free(wstring);
		wstring = NULL;
	}
	return result;
}

WCHAR *wstring_from_utf8(const char *utf8string)
{
	int char_count;
	WCHAR *result;

	// convert MAME string (UTF-8) to UTF-16
	char_count = MultiByteToWideChar(CP_UTF8, 0, utf8string, -1, NULL, 0);
	result = (WCHAR *)malloc(char_count * sizeof(*result));
	if (result != NULL)
		MultiByteToWideChar(CP_UTF8, 0, utf8string, -1, result, char_count);

	return result;
}

char *utf8_from_wstring(const WCHAR *wstring)
{
	int char_count;
	char *result;

	// convert UTF-16 to MAME string (UTF-8)
	char_count = WideCharToMultiByte(CP_UTF8, 0, wstring, -1, NULL, 0, NULL, NULL);
	result = (char *)malloc(char_count * sizeof(*result));
	if (result != NULL)
		WideCharToMultiByte(CP_UTF8, 0, wstring, -1, result, char_count, NULL, NULL);

	return result;
}

#if defined (FBA_DEBUG)
 static TCHAR szConsoleBuffer[1024];
 static int nPrevConsoleStatus = -1;

 static HANDLE DebugBuffer;
 static FILE *DebugLog = NULL;
 static bool bEchoLog = true; // false;
#endif

void tcharstrreplace(TCHAR *pszSRBuffer, const TCHAR *pszFind, const TCHAR *pszReplace)
{
	if (pszSRBuffer == NULL || pszFind == NULL || pszReplace == NULL)
		return;

	int lenFind = _tcslen(pszFind);
	int lenReplace = _tcslen(pszReplace);
	int lenSRBuffer = _tcslen(pszSRBuffer)+1;

	for(int i = 0; (lenSRBuffer > lenFind) && (i < lenSRBuffer - lenFind); i++) {
		if (!memcmp(pszFind, &pszSRBuffer[i], lenFind * sizeof(TCHAR))) {
			if (lenFind == lenReplace) {
				memcpy(&pszSRBuffer[i], pszReplace, lenReplace * sizeof(TCHAR));
				i += lenReplace - 1;
			} else if (lenFind > lenReplace) {
				memcpy(&pszSRBuffer[i], pszReplace, lenReplace * sizeof(TCHAR));
				i += lenReplace;
				int delta = lenFind - lenReplace;
				lenSRBuffer -= delta;
				memmove(&pszSRBuffer[i], &pszSRBuffer[i + delta], (lenSRBuffer - i) * sizeof(TCHAR));
				i--;
			} else { /* this part only works on dynamic buffers - the replacement string length must be smaller or equal to the find string length if this is commented out!
				int delta = lenReplace - lenFind;
				pszSRBuffer = (TCHAR *)realloc(pszSRBuffer, (lenSRBuffer + delta) * sizeof(TCHAR));
				memmove(&pszSRBuffer[i + lenReplace], &pszSRBuffer[i + lenFind], (lenSRBuffer - i - lenFind) * sizeof(TCHAR));
				lenSRBuffer += delta;
				memcpy(&pszSRBuffer[i], pszReplace, lenReplace * sizeof(TCHAR));
				i += lenReplace - 1; */
			}
		}
	}
}

#if defined (FBA_DEBUG)
// Debug printf to a file
static int __cdecl AppDebugPrintf(int nStatus, TCHAR* pszFormat, ...)
{
	va_list vaFormat;

	va_start(vaFormat, pszFormat);

	if (DebugLog) {

		if (nStatus != nPrevConsoleStatus) {
			switch (nStatus) {
				case PRINT_ERROR:
					_ftprintf(DebugLog, _T("</div><div class=\"error\">"));
					break;
				case PRINT_IMPORTANT:
					_ftprintf(DebugLog, _T("</div><div class=\"important\">"));
					break;
				case PRINT_LEVEL1:
					_ftprintf(DebugLog, _T("</div><div class=\"level1\">"));
					break;
				case PRINT_LEVEL2:
					_ftprintf(DebugLog, _T("</div><div class=\"level2\">"));
					break;
				case PRINT_LEVEL3:
					_ftprintf(DebugLog, _T("</div><div class=\"level3\">"));
					break;
				case PRINT_LEVEL4:
					_ftprintf(DebugLog, _T("</div><div class=\"level4\">"));
					break;
				case PRINT_LEVEL5:
					_ftprintf(DebugLog, _T("</div><div class=\"level5\">"));
					break;
				case PRINT_LEVEL6:
					_ftprintf(DebugLog, _T("</div><div class=\"level6\">"));
					break;
				case PRINT_LEVEL7:
					_ftprintf(DebugLog, _T("</div><div class=\"level7\">"));
					break;
				case PRINT_LEVEL8:
					_ftprintf(DebugLog, _T("</div><div class=\"level8\">"));
					break;
				case PRINT_LEVEL9:
					_ftprintf(DebugLog, _T("</div><div class=\"level9\">"));
					break;
				case PRINT_LEVEL10:
					_ftprintf(DebugLog, _T("</div><div class=\"level10\">"));
					break;
				default:
					_ftprintf(DebugLog, _T("</div><div class=\"normal\">"));
			}
		}
		_vftprintf(DebugLog, pszFormat, vaFormat);
		fflush(DebugLog);
	}

	if (!DebugLog || bEchoLog) {
		_vsntprintf(szConsoleBuffer, 1024, pszFormat, vaFormat);

		if (nStatus != nPrevConsoleStatus) {
			switch (nStatus) {
				case PRINT_UI:
					SetConsoleTextAttribute(DebugBuffer,                                                       FOREGROUND_INTENSITY);
					break;
				case PRINT_IMPORTANT:
				case PRINT_LEVEL1:
				case PRINT_LEVEL2:
				case PRINT_LEVEL3:
				case PRINT_LEVEL4:
				case PRINT_LEVEL5:
				case PRINT_LEVEL6:
				case PRINT_LEVEL7:
				case PRINT_LEVEL8:
				case PRINT_LEVEL9:
				case PRINT_LEVEL10:
					SetConsoleTextAttribute(DebugBuffer, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
					break;
				case PRINT_ERROR:
					SetConsoleTextAttribute(DebugBuffer, FOREGROUND_RED | FOREGROUND_GREEN |                   FOREGROUND_INTENSITY);
					break;
				default:
					SetConsoleTextAttribute(DebugBuffer,                  FOREGROUND_GREEN |                   FOREGROUND_INTENSITY);
			}
		}

		tcharstrreplace(szConsoleBuffer, _T(SEPERATOR_1), _T(" * "));
		WriteConsole(DebugBuffer, szConsoleBuffer, _tcslen(szConsoleBuffer), NULL, NULL);
	}

	nPrevConsoleStatus = nStatus;

	va_end(vaFormat);

	return 0;
}
#endif

int dprintf(TCHAR* pszFormat, ...)
{
#if defined (FBA_DEBUG)
	va_list vaFormat;
	va_start(vaFormat, pszFormat);

	_vsntprintf(szConsoleBuffer, 1024, pszFormat, vaFormat);

	if (nPrevConsoleStatus != PRINT_UI) {
		if (DebugLog) {
			_ftprintf(DebugLog, _T("</div><div class=\"ui\">"));
		}
		SetConsoleTextAttribute(DebugBuffer, FOREGROUND_INTENSITY);
		nPrevConsoleStatus = PRINT_UI;
	}

	if (DebugLog) {
		_ftprintf(DebugLog, szConsoleBuffer);
		fflush(DebugLog);
	}

	tcharstrreplace(szConsoleBuffer, _T(SEPERATOR_1), _T(" * "));
	WriteConsole(DebugBuffer, szConsoleBuffer, _tcslen(szConsoleBuffer), NULL, NULL);
	va_end(vaFormat);
#else
	(void)pszFormat;
#endif

	return 0;
}

void CloseDebugLog()
{
#if defined (FBA_DEBUG)
	if (DebugLog) {

		_ftprintf(DebugLog, _T("</pre></body></html>"));

		fclose(DebugLog);
		DebugLog = NULL;
	}

	if (DebugBuffer) {
		CloseHandle(DebugBuffer);
		DebugBuffer = NULL;
	}

	FreeConsole();
#endif
}

int OpenDebugLog()
{
#if defined (FBA_DEBUG)
 #if defined (APP_DEBUG_LOG)

    time_t nTime;
	tm* tmTime;

	time(&nTime);
	tmTime = localtime(&nTime);

	{
		// Initialise the debug log file

  #ifdef _UNICODE
		DebugLog = _tfopen(_T("zzBurnDebug.html"), _T("wb"));

		if (ftell(DebugLog) == 0) {
			WRITE_UNICODE_BOM(DebugLog);

			_ftprintf(DebugLog, _T("<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0 Transitional//EN\">"));
			_ftprintf(DebugLog, _T("<html><head><meta http-equiv=Content-Type content=\"text/html; charset=unicode\"><script src=\"https://ajax.googleapis.com/ajax/libs/jquery/3.2.1/jquery.min.js\"></script>"));
			_ftprintf(DebugLog, _T("<style>body{color:#333333;}.error,.error_chb{color:#ff3f3f;}.important,.important_chb{color:#000000;}.normal,.level1,.level2,.level3,.level4,.level5,.level6,.level7,.level8,.level9,.level10,.normal_chb,.level1_chb,.level2_chb,.level3_chb,.level4_chb,.level5_chb,.level6_chb,.level7_chb,.level8_chb,.level9_chb,.level10_chb{color:#009f00;}.ui{color:9f9f9f;}</style>"));
			_ftprintf(DebugLog, _T("</head><body><pre>"));
		}
  #else
		DebugLog = _tfopen(_T("zzBurnDebug.html"), _T("wt"));

		if (ftell(DebugLog) == 0) {
			_ftprintf(DebugLog, _T("<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0 Transitional//EN\">"));
			_ftprintf(DebugLog, _T("<html><head><meta http-equiv=Content-Type content=\"text/html; charset=windows-%i\"></head><body><pre>"), GetACP());
		}
  #endif

		_ftprintf(DebugLog, _T("<div style=\"font-size:16px;font-weight:bold;\">"));
		_ftprintf(DebugLog, _T("Debug log created by ") _T(APP_TITLE) _T(" v%.20s on %s"), szAppBurnVer, _tasctime(tmTime));
		
		_ftprintf(DebugLog, _T("</div><div style=\"margin-top:12px;\"><input type=\"checkbox\" id=\"chb_all\" onclick=\"$('input:checkbox').not(this).prop('checked', this.checked);\" checked>&nbsp;<label for=\"chb_all\">(Un)check All</label>"));
		_ftprintf(DebugLog, _T("</div><div style=\"margin-top:12px;\"><input type=\"checkbox\" id=\"chb_ui\" onclick=\"if(this.checked==true){$(\'.ui\').show();}else{$(\'.ui\').hide();}\" checked>&nbsp;<label for=\"chb_ui\" class=\"ui_chb\">UI</label>"));
		_ftprintf(DebugLog, _T("</div><div><input type=\"checkbox\" id=\"chb_normal\" onclick=\"if(this.checked==true){$(\'.normal\').show();}else{$(\'.normal\').hide();}\" checked>&nbsp;<label for=\"chb_normal\" class=\"normal_chb\">Normal</label>"));
		_ftprintf(DebugLog, _T("</div><div><input type=\"checkbox\" id=\"chb_important\" onclick=\"if(this.checked==true){$(\'.important\').show();}else{$(\'.important\').hide();}\" checked>&nbsp;<label for=\"chb_important\" class=\"important_chb\">Important</label>"));
		_ftprintf(DebugLog, _T("</div><div><input type=\"checkbox\" id=\"chb_error\" onclick=\"if(this.checked==true){$(\'.error\').show();}else{$(\'.error\').hide();}\" checked>&nbsp;<label for=\"chb_error\" class=\"error_chb\">Error</label>"));
		_ftprintf(DebugLog, _T("</div><div><input type=\"checkbox\" id=\"chb_level1\" onclick=\"if(this.checked==true){$(\'.level1\').show();}else{$(\'.level1\').hide();}\" checked>&nbsp;<label for=\"chb_level1\" class=\"level1_chb\">Level 1</label>"));
		_ftprintf(DebugLog, _T("</div><div><input type=\"checkbox\" id=\"chb_level2\" onclick=\"if(this.checked==true){$(\'.level2\').show();}else{$(\'.level2\').hide();}\" checked>&nbsp;<label for=\"chb_level2\" class=\"level2_chb\">Level 2</label>"));
		_ftprintf(DebugLog, _T("</div><div><input type=\"checkbox\" id=\"chb_level3\" onclick=\"if(this.checked==true){$(\'.level3\').show();}else{$(\'.level3\').hide();}\" checked>&nbsp;<label for=\"chb_level3\" class=\"level3_chb\">Level 3</label>"));
		_ftprintf(DebugLog, _T("</div><div><input type=\"checkbox\" id=\"chb_level4\" onclick=\"if(this.checked==true){$(\'.level4\').show();}else{$(\'.level4\').hide();}\" checked>&nbsp;<label for=\"chb_level4\" class=\"level4_chb\">Level 4</label>"));
		_ftprintf(DebugLog, _T("</div><div><input type=\"checkbox\" id=\"chb_level5\" onclick=\"if(this.checked==true){$(\'.level5\').show();}else{$(\'.level5\').hide();}\" checked>&nbsp;<label for=\"chb_level5\" class=\"level5_chb\">Level 5</label>"));
		_ftprintf(DebugLog, _T("</div><div><input type=\"checkbox\" id=\"chb_level6\" onclick=\"if(this.checked==true){$(\'.level6\').show();}else{$(\'.level6\').hide();}\" checked>&nbsp;<label for=\"chb_level6\" class=\"level6_chb\">Level 6</label>"));
		_ftprintf(DebugLog, _T("</div><div><input type=\"checkbox\" id=\"chb_level7\" onclick=\"if(this.checked==true){$(\'.level7\').show();}else{$(\'.level7\').hide();}\" checked>&nbsp;<label for=\"chb_level7\" class=\"level7_chb\">Level 7</label>"));
		_ftprintf(DebugLog, _T("</div><div><input type=\"checkbox\" id=\"chb_level8\" onclick=\"if(this.checked==true){$(\'.level8\').show();}else{$(\'.level8\').hide();}\" checked>&nbsp;<label for=\"chb_level8\" class=\"level8_chb\">Level 8</label>"));
		_ftprintf(DebugLog, _T("</div><div><input type=\"checkbox\" id=\"chb_level9\" onclick=\"if(this.checked==true){$(\'.level9\').show();}else{$(\'.level9\').hide();}\" checked>&nbsp;<label for=\"chb_level9\" class=\"level9_chb\">Level 9</label>"));
		_ftprintf(DebugLog, _T("</div><div style=\"margin-bottom:20px;\"><input type=\"checkbox\" id=\"chb_level10\" onclick=\"if(this.checked==true){$(\'.level10\').show();}else{$(\'.level10\').hide();}\" checked>&nbsp;<label for=\"chb_level10\" class=\"level10_chb\">Level 10</label>"));
	}
 #endif

	{
		// Initialise the debug console

		COORD DebugBufferSize = { 80, 1000 };

		{

			// Since AttachConsole is only present in Windows XP, import it manually

#if _WIN32_WINNT >= 0x0500 && defined (_MSC_VER)
// #error Manually importing AttachConsole() function, but compiling with _WIN32_WINNT >= 0x0500
			if (!AttachConsole(ATTACH_PARENT_PROCESS)) {
				AllocConsole();
			}
#else

#ifdef ATTACH_PARENT_PROCESS
#undef ATTACH_PARENT_PROCESS
#endif
#define ATTACH_PARENT_PROCESS ((DWORD)-1)

			BOOL (WINAPI* pAttachConsole)(DWORD dwProcessId) = NULL;
			HINSTANCE hKernel32DLL = LoadLibrary(_T("kernel32.dll"));

			if (hKernel32DLL) {
				pAttachConsole = (BOOL (WINAPI*)(DWORD))GetProcAddress(hKernel32DLL, "AttachConsole");
			}
			if (pAttachConsole) {
				if (!pAttachConsole(ATTACH_PARENT_PROCESS)) {
					AllocConsole();
				}
			} else {
				AllocConsole();
			}
			if (hKernel32DLL) {
				FreeLibrary(hKernel32DLL);
			}

 #undef ATTACH_PARENT_PROCESS
#endif

		}

		DebugBuffer = CreateConsoleScreenBuffer(GENERIC_WRITE, FILE_SHARE_READ, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
		SetConsoleScreenBufferSize(DebugBuffer, DebugBufferSize);
		SetConsoleActiveScreenBuffer(DebugBuffer);
		SetConsoleTitle(_T(APP_TITLE) _T(" Debug console"));

		SetConsoleTextAttribute(DebugBuffer, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
		_sntprintf(szConsoleBuffer, 1024, _T("Welcome to the ") _T(APP_TITLE) _T(" debug console.\n"));
		WriteConsole(DebugBuffer, szConsoleBuffer, _tcslen(szConsoleBuffer), NULL, NULL);

		SetConsoleTextAttribute(DebugBuffer, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
		if (DebugLog) {
			_sntprintf(szConsoleBuffer, 1024, _T("Debug messages are logged in zzBurnDebug.html"));
			if (!DebugLog || bEchoLog) {
				_sntprintf(szConsoleBuffer + _tcslen(szConsoleBuffer), 1024 - _tcslen(szConsoleBuffer), _T(", and echod to this console"));
			}
			_sntprintf(szConsoleBuffer + _tcslen(szConsoleBuffer), 1024 - _tcslen(szConsoleBuffer), _T(".\n\n"));
		} else {
			_sntprintf(szConsoleBuffer, 1024, _T("Debug messages are echod to this console.\n\n"));
		}
		WriteConsole(DebugBuffer, szConsoleBuffer, _tcslen(szConsoleBuffer), NULL, NULL);
	}

	nPrevConsoleStatus = -1;

	bprintf = AppDebugPrintf;							// Redirect Burn library debug to our function
#endif

	return 0;
}

void MonitorAutoCheck()
{
	RECT rect;
	int x, y;

	SystemParametersInfo(SPI_GETWORKAREA, 0, &rect, 0);

	x = GetSystemMetrics(SM_CXSCREEN);
	y = GetSystemMetrics(SM_CYSCREEN);
	
	// default full-screen resolution to this size
	nVidHorWidth = x;
	nVidHorHeight = y;
	nVidVerWidth = x;
	nVidVerHeight = y;
	
	// also add this to the presets
	VidPreset[3].nWidth = x;
	VidPreset[3].nHeight = y;
	VidPresetVer[3].nWidth = x;
	VidPresetVer[3].nHeight = y;

	TCHAR szResXY[256] = _T("");
	_stprintf(szResXY, _T("%dx%d"), x, y);

	// Normal CRT (4:3) ( Verified at Wikipedia.Org )
	if( !_tcscmp(szResXY, _T("320x240"))	|| 
		!_tcscmp(szResXY, _T("512x384"))	||
		!_tcscmp(szResXY, _T("640x480"))	|| 
		!_tcscmp(szResXY, _T("800x600"))	||
		!_tcscmp(szResXY, _T("832x624"))	||
		!_tcscmp(szResXY, _T("1024x768"))	||
		!_tcscmp(szResXY, _T("1120x832"))	|| 
		!_tcscmp(szResXY, _T("1152x864"))	|| 
		!_tcscmp(szResXY, _T("1280x960"))	||
		!_tcscmp(szResXY, _T("1280x1024"))	|| 
		!_tcscmp(szResXY, _T("1400x1050"))	||  
		!_tcscmp(szResXY, _T("1600x1200"))	||
		!_tcscmp(szResXY, _T("2048x1536"))	||
		!_tcscmp(szResXY, _T("2800x2100"))	||
		!_tcscmp(szResXY, _T("3200x2400"))	|| 
		!_tcscmp(szResXY, _T("4096x3072"))	||
		!_tcscmp(szResXY, _T("6400x4800")) ){
		nVidScrnAspectX = 4; 
		nVidScrnAspectY = 3;
	}

	// Normal LCD (5:4) ( Verified at Wikipedia.Org )
	if( !_tcscmp(szResXY, _T("320x256"))	||
		!_tcscmp(szResXY, _T("640x512"))	||
		!_tcscmp(szResXY, _T("1280x1024"))	|| 
		!_tcscmp(szResXY, _T("2560x2048"))	||
		!_tcscmp(szResXY, _T("5120x4096")) ){
		nVidScrnAspectX = 5; 
		nVidScrnAspectY = 4;
	}

	// CRT Widescreen (16:9) ( Verified at Wikipedia.Org )
	if( !_tcscmp(szResXY, _T("480x270"))  ||
		!_tcscmp(szResXY, _T("1280x720")) ||
		!_tcscmp(szResXY, _T("1360x768")) ||
		!_tcscmp(szResXY, _T("1366x768")) ||
		!_tcscmp(szResXY, _T("1920x1080"))) {
		nVidScrnAspectX = 16; 
		nVidScrnAspectY = 9;
	}

	// LCD Widescreen (16:10) ( Verified at Wikipedia.Org )
	if(	!_tcscmp(szResXY, _T("320x200"))	|| 
		!_tcscmp(szResXY, _T("1280x800"))	||
		!_tcscmp(szResXY, _T("1440x900"))	|| 
		!_tcscmp(szResXY, _T("1680x1050"))	||
		!_tcscmp(szResXY, _T("1920x1200"))	||
		!_tcscmp(szResXY, _T("2560x1600"))	||
		!_tcscmp(szResXY, _T("3840x2400"))	||
		!_tcscmp(szResXY, _T("5120x3200"))	||
		!_tcscmp(szResXY, _T("7680x4800")) ){
		nVidScrnAspectX = 16; 
		nVidScrnAspectY = 10;
	}
}

bool SetNumLock(bool bState)
{
	BYTE keyState[256];
	
	if (bNoChangeNumLock) return 0;

	GetKeyboardState(keyState);
	if ((bState && !(keyState[VK_NUMLOCK] & 1)) || (!bState && (keyState[VK_NUMLOCK] & 1))) {
		keybd_event(VK_NUMLOCK, 0, KEYEVENTF_EXTENDEDKEY, 0 );

		keybd_event(VK_NUMLOCK, 0, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);
	}

	return keyState[VK_NUMLOCK] & 1;
}

#include <wininet.h>

static int AppInit()
{
#if defined (_MSC_VER) && defined (_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_CHECK_ALWAYS_DF);			// Check for memory corruption
	_CrtSetDbgFlag(_CRTDBG_DELAY_FREE_MEM_DF);			//
	_CrtSetDbgFlag(_CRTDBG_LEAK_CHECK_DF);				//
#endif

#if defined (FBA_DEBUG)
	OpenDebugLog();
#endif

	// Create a handle to the main thread of execution
	DuplicateHandle(GetCurrentProcess(), GetCurrentThread(), GetCurrentProcess(), &hMainThread, 0, false, DUPLICATE_SAME_ACCESS);
	
	// Init the Burn library
	BurnLibInit();
	
	// Load config for the application
	ConfigAppLoad();

	FBALocaliseInit(szLocalisationTemplate);
	BurnerDoGameListLocalisation();

	if (bMonitorAutoCheck) MonitorAutoCheck();

#if 1 || !defined (FBA_DEBUG)
	// print a warning if we're running for the 1st time
	if (nIniVersion < nBurnVer) {
		ScrnInit();
		//SplashDestroy(1);
		FirstUsageCreate();

		ConfigAppSave();								// Create initial config file
	}
#endif

	// Set the thread priority for the main thread
	SetThreadPriority(GetCurrentThread(), nAppThreadPriority);

	bCheatsAllowed = true;

#ifdef USE_SDL
	SDL_Init(0);
#endif

	ComputeGammaLUT();

	if (VidSelect(nVidSelect)) {
		nVidSelect = 0;
		VidSelect(nVidSelect);
	}

	hAccel = LoadAccelerators(hAppInst, MAKEINTRESOURCE(IDR_ACCELERATOR));

	// Build the ROM information
	CreateROMInfo(NULL);
	
	// Write a clrmame dat file if we are verifying roms
#if defined (ROM_VERIFY)
	create_datfile(_T("fba.dat"), 0);	
#endif

	bNumlockStatus = SetNumLock(false);
	
	if(bEnableIcons && !bIconsLoaded) {
		// load driver icons
		LoadDrvIcons();
		bIconsLoaded = 1;
	}

	return 0;
}

static int AppExit()
{
	if(bIconsLoaded) {
		// unload driver icons
		UnloadDrvIcons();
		bIconsLoaded = 0;
	}
	
	SetNumLock(bNumlockStatus);

	DrvExit();						// Make sure any game driver is exitted
	FreeROMInfo();
	MediaExit();
	BurnLibExit();					// Exit the Burn library

	DisableHighResolutionTiming();

#ifdef USE_SDL
	SDL_Quit();
#endif

	FBALocaliseExit();
	BurnerExitGameListLocalisation();

	if (hAccel) {
		DestroyAcceleratorTable(hAccel);
		hAccel = NULL;
	}

	SplashDestroy(1);

	CloseHandle(hMainThread);

	CloseDebugLog();

	return 0;
}

void AppCleanup()
{
	StopReplay();
	WaveLogStop();

	AppExit();
}

int AppMessage(MSG *pMsg)
{
	if (IsDialogMessage(hInpdDlg, pMsg))	 return 0;
	if (IsDialogMessage(hInpCheatDlg, pMsg)) return 0;
	if (IsDialogMessage(hInpDIPSWDlg, pMsg)) return 0;
	if (IsDialogMessage(hDbgDlg, pMsg))		 return 0;

	if (IsDialogMessage(hInpsDlg, pMsg))	 return 0;
	if (IsDialogMessage(hInpcDlg, pMsg))	 return 0;

	return 1; // Didn't process this message
}

bool AppProcessKeyboardInput()
{
	if (hwndChat) {
		return false;
	}

	return true;
}

int ProcessCmdLine()
{
	unsigned int i;
	int nOptX = 0, nOptY = 0, nOptD = 0;
	int nOpt1Size;
	TCHAR szOpt2[3] = _T("");
	TCHAR szName[MAX_PATH];

	if (szCmdLine[0] == _T('\"')) {
		int nLen = _tcslen(szCmdLine);
		nOpt1Size = 1;
		while (szCmdLine[nOpt1Size] != _T('\"') && nOpt1Size < nLen) {
			nOpt1Size++;
		}
		if (nOpt1Size == nLen) {
			szName[0] = 0;
		} else {
			nOpt1Size++;
			_tcsncpy(szName, szCmdLine + 1, nOpt1Size - 2);
			szName[nOpt1Size - 2] = 0;
		}
	} else {
		int nLen = _tcslen(szCmdLine);
		nOpt1Size = 0;
		while (szCmdLine[nOpt1Size] != _T(' ') && nOpt1Size < nLen) {
			nOpt1Size++;
		}
		_tcsncpy(szName, szCmdLine, nOpt1Size);
		szName[nOpt1Size] = 0;
	}

	if (_tcslen(szName)) {
		if (_tcscmp(szName, _T("-listinfo")) == 0) {
			write_datfile(DAT_ARCADE_ONLY, stdout);
			return 1;
		}
		
		if (_tcscmp(szName, _T("-listinfomdonly")) == 0) {
			write_datfile(DAT_MEGADRIVE_ONLY, stdout);
			return 1;
		}
		
		if (_tcscmp(szName, _T("-listinfopceonly")) == 0) {
			write_datfile(DAT_PCENGINE_ONLY, stdout);
			return 1;
		}
		
		if (_tcscmp(szName, _T("-listinfotg16only")) == 0) {
			write_datfile(DAT_TG16_ONLY, stdout);
			return 1;
		}
		
		if (_tcscmp(szName, _T("-listinfosgxonly")) == 0) {
			write_datfile(DAT_SGX_ONLY, stdout);
			return 1;
		}

		if (_tcscmp(szName, _T("-listinfosg1000only")) == 0) {
			write_datfile(DAT_SG1000_ONLY, stdout);
			return 1;
		}

		if (_tcscmp(szName, _T("-listinfocolecoonly")) == 0) {
			write_datfile(DAT_COLECO_ONLY, stdout);
			return 1;
		}
		
		if (_tcscmp(szName, _T("-listinfosmsonly")) == 0) {
			write_datfile(DAT_MASTERSYSTEM_ONLY, stdout);
			return 1;
		}
		
		if (_tcscmp(szName, _T("-listinfoggonly")) == 0) {
			write_datfile(DAT_GAMEGEAR_ONLY, stdout);
			return 1;
		}

		if (_tcscmp(szName, _T("-listinfomsxonly")) == 0) {
			write_datfile(DAT_MSX_ONLY, stdout);
			return 1;
		}
		
		if (_tcscmp(szName, _T("-listinfospectrumonly")) == 0) {
			write_datfile(DAT_SPECTRUM_ONLY, stdout);
			return 1;
		}
		
		if (_tcscmp(szName, _T("-listextrainfo")) == 0) {
			int nWidth;
			int nHeight;
			int nAspectX;
			int nAspectY;
			for (i = 0; i < nBurnDrvCount; i++) {
				nBurnDrvActive = i;
				BurnDrvGetVisibleSize(&nWidth, &nHeight);
				BurnDrvGetAspect(&nAspectX, &nAspectY);
				printf("%s\t%ix%i\t%i:%i\t0x%08X\t\"%s\"\t%i\t%i\t%x\t%x\t\"%s\"\n", BurnDrvGetTextA(DRV_NAME), nWidth, nHeight, nAspectX, nAspectY, BurnDrvGetHardwareCode(), BurnDrvGetTextA(DRV_SYSTEM), BurnDrvIsWorking(), BurnDrvGetMaxPlayers(), BurnDrvGetGenreFlags(), BurnDrvGetFamilyFlags(), BurnDrvGetTextA(DRV_COMMENT));
			}			
			return 1;
		}
	}

	_stscanf(&szCmdLine[nOpt1Size], _T("%2s %i x %i x %i"), szOpt2, &nOptX, &nOptY, &nOptD);

	if (_tcslen(szName)) {
		bool bFullscreen = 1;
		bCmdOptUsed = 1;

		if (_tcscmp(szOpt2, _T("-r")) == 0) {
			if (nOptX && nOptY) {
				nVidWidth = nOptX;
				nVidHeight = nOptY;
			}
			if (nOptD) {
				nVidDepth = nOptD;
			}
		} else {
			if (_tcscmp(szOpt2, _T("-a")) == 0) {
				bVidArcaderes = 1;
			} else {
				if (_tcscmp(szOpt2, _T("-w")) == 0) {
					bCmdOptUsed = 2;
					bFullscreen = 0;
				}
			}
		}

		if (bFullscreen) {
			nVidFullscreen = 1;
		}

		if (_tcscmp(&szName[_tcslen(szName) - 3], _T(".fs")) == 0) {
			if (BurnStateLoad(szName, 1, &DrvInitCallback)) {
				return 1;
			} else {
//				bRunPause = 1;
			}
		} else {
			if (_tcscmp(&szName[_tcslen(szName) - 3], _T(".fr")) == 0) {
				if (StartReplay(szName)) {
					return 1;
				}
			} else {
				bQuietLoading = true;

				for (i = 0; i < nBurnDrvCount; i++) {
					nBurnDrvActive = i;
					if ((_tcscmp(BurnDrvGetText(DRV_NAME), szName) == 0) && (!(BurnDrvGetFlags() & BDF_BOARDROM))){
						if (DrvInit(i, true)) { // failed (bad romset, etc.)
							nVidFullscreen = 0; // Don't get stuck in fullscreen mode
						}
						break;
					}
				}

				bQuietLoading = false;

				if (i == nBurnDrvCount) {
					FBAPopupAddText(PUF_TEXT_DEFAULT, MAKEINTRESOURCE(IDS_ERR_UI_NOSUPPORT), szName, _T(APP_TITLE));
					FBAPopupDisplay(PUF_TYPE_ERROR);
					return 1;
				}
			}
		}
	}

	POST_INITIALISE_MESSAGE;

	if (!nVidFullscreen) {
		MenuEnableItems();
	}

	return 0;
}

static void CreateSupportFolders()
{
	TCHAR szSupportDirs[34][MAX_PATH] = {
		{_T("support/")},
		{_T("support/previews/")},
		{_T("support/titles/")},
		{_T("support/icons/")},
		{_T("support/cheats/")},
		{_T("support/hiscores/")},
		{_T("support/samples/")},
		{_T("support/hdd/")},
		{_T("support/ips/")},
		{_T("support/neocdz/")},
		{_T("support/blend/")},
		{_T("support/select/")},
		{_T("support/versus/")},
		{_T("support/howto/")},
		{_T("support/scores/")},
		{_T("support/bosses/")},
		{_T("support/gameover/")},
		{_T("support/flyers/")},
		{_T("support/marquees/")},
		{_T("support/cpanel/")},
		{_T("support/cabinets/")},
		{_T("support/pcbs/")},
		{_T("support/history/")},
		{_T("neocdiso/")},
		// the below are named after the MESS software lists
		{_T("megadriv/")},
		{_T("pce/")},
		{_T("sgx/")},
		{_T("tg16/")},
		{_T("sg1000/")},
		{_T("coleco/")},
		{_T("sms/")},
		{_T("gamegear/")},
		{_T("msx/")},
		{_T("spectrum/")},
	};
	
	for(int x = 0; x < 34; x++) {
		CreateDirectory(szSupportDirs[x], NULL);
	}
}

// Main program entry point
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR lpCmdLine, int nShowCmd)
{
	DSCore_Init();
	DICore_Init();
	DDCore_Init();
	Dx9Core_Init();

	// Provide a custom exception handler
	SetUnhandledExceptionFilter(ExceptionFilter);

	hAppInst = hInstance;

	// Make version string
	if (nBurnVer & 0xFF) {
		// private version (alpha)
		_stprintf(szAppBurnVer, _T("%x.%x.%x.%02x"), nBurnVer >> 20, (nBurnVer >> 16) & 0x0F, (nBurnVer >> 8) & 0xFF, nBurnVer & 0xFF);
	} else {
		// public version
		_stprintf(szAppBurnVer, _T("%x.%x.%x"), nBurnVer >> 20, (nBurnVer >> 16) & 0x0F, (nBurnVer >> 8) & 0xFF);
	}
	
#if !defined (DONT_DISPLAY_SPLASH)
//	if (lpCmdLine[0] == 0) SplashCreate();
#endif

	nAppShowCmd = nShowCmd;

	AppDirectory();								// Set current directory to be the applications directory

#ifdef INCLUDE_AVI_RECORDING
#define DIRCNT 10
#else
#define DIRCNT 9
#endif
	// Make sure there are roms and cfg subdirectories
	TCHAR szDirs[DIRCNT][MAX_PATH] = {
		{_T("config")},
		{_T("config/games")},
		{_T("config/ips")},
		{_T("config/localisation")},
		{_T("config/presets")},
		{_T("recordings")},
		{_T("roms")},
		{_T("savestates")},
		{_T("screenshots")},
#ifdef INCLUDE_AVI_RECORDING
		{_T("avi")},
#endif
	};

	for(int x = 0; x < DIRCNT; x++) {
		CreateDirectory(szDirs[x], NULL);
	}
#undef DIRCNT

	//
	
	{
		INITCOMMONCONTROLSEX initCC = {
			sizeof(INITCOMMONCONTROLSEX),
			ICC_BAR_CLASSES | ICC_COOL_CLASSES | ICC_LISTVIEW_CLASSES | ICC_PROGRESS_CLASS | ICC_TREEVIEW_CLASSES,
		};
		InitCommonControlsEx(&initCC);
	}

	if (lpCmdLine) {
		_tcscpy(szCmdLine, ANSIToTCHAR(lpCmdLine, NULL, 0));
	}

	if (!(AppInit())) {							// Init the application
		if (bAlwaysCreateSupportFolders) CreateSupportFolders();
		if (!(ProcessCmdLine())) {
			DetectWindowsVersion();
			EnableHighResolutionTiming();

			MediaInit();

			RunMessageLoop();					// Run the application message loop
		}
	}

	NeoCDZRateChangeback();                     // Change back temp CDZ rate before saving

	ConfigAppSave();							// Save config for the application

	AppExit();									// Exit the application	

	return 0;
}
