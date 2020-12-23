;--------------------------------
;Include Modern UI

  !include "MUI2.nsh"

;--------------------------------
;General

	;Name and file
	Name "NcFTP 3.2.6"
	OutFile "Setup NcFTP 3.2.6.exe"

	;Default installation folder
	InstallDir "$LOCALAPPDATA\NcFTP"
	;InstallDir "$DESKTOP"
  
	;Get installation folder from registry if available
	InstallDirRegKey HKCU "Software\NcFTP" ""

	;Request application privileges for Windows Vista
	;RequestExecutionLevel user
	RequestExecutionLevel admin
	
;--------------------------------
;Interface Settings

	!define MUI_ABORTWARNING

;--------------------------------
;Pages

!define MUI_WELCOMEPAGE_TEXT "Hello!$\n$\n\
NcFTP is a free set of programs for your Command Prompt (or PowerShell) that implement the File Transfer Protocol.  It's been around for UNIX for quite some time now (circa 1992) and many people still use the program and contribute suggestions and feedback.$\n$\n\
The main program is simply called ncftp.  There are also separate utility programs for one-shot FTP operations (i.e. for shell scripts and command-line junkies);  these include ncftpget, ncftpput, and ncftpls.  Run each command without any arguments to see the usage screen, or read the man page.\
"

	!insertmacro MUI_PAGE_WELCOME
	; !insertmacro MUI_PAGE_LICENSE "..\..\doc\LICENSE.txt"
	; !insertmacro MUI_PAGE_COMPONENTS
	!insertmacro MUI_PAGE_DIRECTORY
	!insertmacro MUI_PAGE_INSTFILES
	; !insertmacro MUI_PAGE_FINISH
	
	!insertmacro MUI_UNPAGE_CONFIRM
	!insertmacro MUI_UNPAGE_INSTFILES
  
;--------------------------------
;Languages
 
	!insertmacro MUI_LANGUAGE "English"

;--------------------------------
;Installer Sections

Section "Main Section" SecMain
	SetOutPath "$SYSDIR"

	File ..\..\ncftp\Release\ncftp.exe
	File ..\..\sh_util\Release\ncftpget.exe
	File ..\..\sh_util\Release\ncftpput.exe
	File ..\..\sh_util\Release\ncftpls.exe
	File ..\..\sh_util\Release\ncftpbatch.exe
	
	CreateDirectory "$SMPROGRAMS\NcFTP"
	CreateShortCut "$SMPROGRAMS\NcFTP\NcFTP.lnk" "$SYSDIR\ncftp.exe"
	
	CreateDirectory "$INSTDIR\doc"
	SetOutPath "$INSTDIR\doc"
	File ..\..\doc\html\index.html
	File ..\..\doc\html\ncftp.html
	File ..\..\doc\html\ncftpput.html
	File ..\..\doc\html\ncftpget.html
	File ..\..\doc\html\ncftpls.html
	File ..\..\doc\html\ncftpbatch.html
	CreateShortCut "$SMPROGRAMS\NcFTP\Documentation.lnk" "$INSTDIR\doc\index.html"
	
	SetOutPath "$INSTDIR"
	File ..\..\win\bmed\Release\ncftpbookmarks.exe
	
	;Store installation folder
	WriteRegStr HKCU "Software\NcFTP" "" $INSTDIR
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\App Paths\ncftp.exe" "" "$INSTDIR\ncftp.exe"
  
	;Create uninstaller
	CreateShortCut "$SMPROGRAMS\NcFTP\Uninstall.lnk" "$INSTDIR\Uninstall.exe"
	WriteUninstaller "$INSTDIR\Uninstall.exe"

SectionEnd

;--------------------------------
;Descriptions

  ;Language strings
  LangString DESC_SecMain ${LANG_ENGLISH} "Main"

  ;Assign language strings to sections
  ; !insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
  ;   !insertmacro MUI_DESCRIPTION_TEXT ${SecMain} $(DESC_SecMain)
  ; !insertmacro MUI_FUNCTION_DESCRIPTION_END

;--------------------------------
;Uninstaller Section

Section "Uninstall"

	Delete "$SYSDIR\ncftp.exe"
	Delete "$SYSDIR\ncftpput.exe"
	Delete "$SYSDIR\ncftpget.exe"
	Delete "$SYSDIR\ncftpls.exe"
	Delete "$SYSDIR\ncftpbatch.exe"

	Delete "$INSTDIR\Uninstall.exe"
	Delete "$INSTDIR\ncftpbookmarks.exe"

	RMDir /r "$INSTDIR"
	RMDir /r "$SMPROGRAMS\NcFTP"
	DeleteRegKey /ifempty HKCU "Software\NcFTP"
	DeleteRegKey /ifempty HKLM "Software\Microsoft\Windows\CurrentVersion\App Paths\ncftp.exe" 
SectionEnd