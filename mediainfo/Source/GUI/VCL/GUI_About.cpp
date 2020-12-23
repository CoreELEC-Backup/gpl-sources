/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license that can
 *  be found in the License.html file in the root of the source tree.
 */

//---------------------------------------------------------------------------
// Compilation condition
#ifndef MEDIAINFOGUI_ABOUT_NO
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
#include "GUI/VCL/GUI_About.h"
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#pragma resource "*.dfm"
TAboutF *AboutF;
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#include "Common/Preferences.h"
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
const ZenLib::Char* MEDIAINFO_ABOUT=     __T("MediaInfo X.X.X.X\\r\\nCopyright (c) MediaArea.net SARL");
const ZenLib::Char* MEDIAINFO_URL=       __T("http://MediaArea.net/MediaInfo");
const ZenLib::Char* MEDIAINFO_NEWVERSION=__T("http://MediaArea.net/MediaInfo");
const ZenLib::Char* MEDIAINFO_DONATE=    __T("http://MediaArea.net/MediaInfo/Donate");
const ZenLib::Char* MEDIAINFO_MAILTO=    __T("mailto:info@mediaarea.net");
//---------------------------------------------------------------------------

//***************************************************************************
// Class
//***************************************************************************

//---------------------------------------------------------------------------
__fastcall TAboutF::TAboutF(TComponent* Owner)
    : TForm(Owner)
{
}

//---------------------------------------------------------------------------
void __fastcall TAboutF::FormShow(TObject *Sender)
{
    //Information
    ZenLib::Ztring C1;
    C1+=MEDIAINFO_ABOUT;
    C1.FindAndReplace(__T("X.X.X.X"), MediaInfo_Version_GUI);
    C1+=__T("\r\n\r\n");
    C1+=Prefs->Translate(__T("MediaInfo_About")).c_str();
    C1.FindAndReplace(__T("\\r\\n"), __T("\r\n"), 0, ZenLib::Ztring_Recursive);
    Memo->Text=C1.c_str();

    //Translation
    Caption=Prefs->Translate(__T("About")).c_str();
    OK->Caption=Prefs->Translate(__T("OK")).c_str();
    WebSite->Caption=Prefs->Translate(__T("Go to WebSite")).c_str();
    NewVersion->Caption=Prefs->Translate(__T("CheckNewVersion")).c_str();
    Donate->Caption=Prefs->Translate(__T("Donate")).c_str();
    WriteMe->Caption=Prefs->Translate(__T("WriteMe")).c_str();
    if (Prefs->Translate(__T("  Author_Name")).size()==0)
        Translator->Visible=false;
    else
    {
        Translator->Caption=(Prefs->Translate(__T("Translator"))+Prefs->Translate(__T(": "))+Prefs->Translate(__T("  Author_Name"))).c_str();
        Translator->Visible=true;
    }
    Translator_Url=Prefs->Translate(__T("  Author_Email"));
    if (Translator_Url.size()==0 || Translator_Url==__T("Info@MediaArea.net"))
        WriteToTranslator->Visible=false;
    else
    {
        WriteToTranslator->Caption=Prefs->Translate(__T("WriteToTranslator")).c_str();
        WriteToTranslator->Visible=true;
    }

    //if (Prefs->Donated) //No more
    {
        Tektronix->Visible=false;
        Tektronix_Label->Visible=false;
    }
}

//---------------------------------------------------------------------------
void __fastcall TAboutF::NewVersionClick(TObject *Sender)
{
    ShellExecute(NULL, NULL, MEDIAINFO_NEWVERSION, NULL, NULL, SW_SHOWNORMAL);
}

//---------------------------------------------------------------------------
void __fastcall TAboutF::DonateClick(TObject *Sender)
{
    ShellExecute(NULL, NULL, MEDIAINFO_DONATE, NULL, NULL, SW_SHOWNORMAL);
}

//---------------------------------------------------------------------------
void __fastcall TAboutF::WriteMeClick(TObject *Sender)
{
    ShellExecute(NULL, NULL, MEDIAINFO_MAILTO, NULL, NULL, SW_SHOWNORMAL);
}

//---------------------------------------------------------------------------
void __fastcall TAboutF::WriteToTranslatorClick(TObject *Sender)
{
    ZenLib::Ztring Url=ZenLib::Ztring(__T("mailto:"))+Translator_Url;
    ShellExecute(NULL, NULL, Url.c_str(), NULL, NULL, SW_SHOWNORMAL);
}

//---------------------------------------------------------------------------
void __fastcall TAboutF::WebSiteClick(TObject *Sender)
{
    ShellExecute(NULL, NULL, MEDIAINFO_URL, NULL, NULL, SW_SHOWNORMAL);
}


//***************************************************************************
// C++
//***************************************************************************

#endif //MEDIAINFOGUI_ABOUT_NO



void __fastcall TAboutF::TektronixClick(TObject *Sender)
{
    ShellExecute(NULL, NULL, __T("http://www.tek.com/file-based-qc-solutions"), NULL, NULL, SW_SHOWNORMAL);
}
//---------------------------------------------------------------------------

