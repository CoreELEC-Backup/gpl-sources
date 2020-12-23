/*
 * dummydevice.c: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id$
 */

#include <vdr/config.h>
#include <vdr/device.h>

#include <vdr/osd.h>

class cDummyOsd : public cOsd {
  public:
    cDummyOsd(int Left, int Top, uint Level) : cOsd(Left, Top, Level) {}
    virtual ~cDummyOsd() {}

    virtual cPixmap *CreatePixmap(int Layer, const cRect &ViewPort, const cRect &DrawPort = cRect::Null) {return NULL;}
    virtual void DestroyPixmap(cPixmap *Pixmap) {}
    virtual void DrawImage(const cPoint &Point, const cImage &Image) {}
    virtual void DrawImage(const cPoint &Point, int ImageHandle) {}
    virtual eOsdError CanHandleAreas(const tArea *Areas, int NumAreas) {return oeOk;}
    virtual eOsdError SetAreas(const tArea *Areas, int NumAreas) {return oeOk;}
    virtual void SaveRegion(int x1, int y1, int x2, int y2) {}
    virtual void RestoreRegion(void) {}
    virtual eOsdError SetPalette(const cPalette &Palette, int Area) {return oeOk;}
    virtual void DrawPixel(int x, int y, tColor Color) {}
    virtual void DrawBitmap(int x, int y, const cBitmap &Bitmap, tColor ColorFg = 0, tColor ColorBg = 0, bool ReplacePalette = false, bool Overlay = false) {}
    virtual void DrawText(int x, int y, const char *s, tColor ColorFg, tColor ColorBg, const cFont *Font, int Width = 0, int Height = 0, int Alignment = taDefault) {}
    virtual void DrawRectangle(int x1, int y1, int x2, int y2, tColor Color) {}
    virtual void DrawEllipse(int x1, int y1, int x2, int y2, tColor Color, int Quadrants = 0) {}
    virtual void DrawSlope(int x1, int y1, int x2, int y2, tColor Color, int Type) {}
    virtual void Flush(void) {}
};

class cDummyOsdProvider : public cOsdProvider {
  protected:
    virtual cOsd *CreateOsd(int Left, int Top, uint Level) { return new cDummyOsd(Left, Top, Level); }
    virtual bool ProvidesTrueColor(void) {return true;}
    virtual int StoreImageData(const cImage &Image) {return 0;}
    virtual void DropImageData(int ImageHandle) {}

  public:
    cDummyOsdProvider() : cOsdProvider() {}
    virtual ~cDummyOsdProvider() {}
};


class cDummyDevice : cDevice {
public:
    cDummyDevice() : cDevice() {}
    virtual ~cDummyDevice() {}

    virtual bool HasDecoder(void) const { return true; }

    virtual bool SetPlayMode(ePlayMode PlayMode) {return true;}
    virtual int  PlayVideo(const uchar *Data, int Length) {return Length;}

    virtual int  PlayAudio(const uchar *Data, int Length, uchar Id) {return Length;}

    virtual int PlayTsVideo(const uchar *Data, int Length) {return Length;}
    virtual int PlayTsAudio(const uchar *Data, int Length) {return Length;}
    virtual int PlayTsSubtitle(const uchar *Data, int Length) {return Length;}

    virtual int PlayPes(const uchar *Data, int Length, bool VideoOnly = false) {return Length;}
    virtual int PlayTs(const uchar *Data, int Length, bool VideoOnly = false) {return Length;}

    virtual bool Poll(cPoller &Poller, int TimeoutMs = 0) {return true;}
    virtual bool Flush(int TimeoutMs = 0) {return true;}
    bool Start(void) {return true;}

  protected:
    virtual void MakePrimaryDevice(bool On) { if (On) new cDummyOsdProvider(); cDevice::MakePrimaryDevice(On); }
};

#include <vdr/plugin.h>

static const char *VERSION        = "2.0.0";
static const char *DESCRIPTION    = "Output device that does nothing";

class cPluginDummydevice : public cPlugin {
private:
public:
  cPluginDummydevice(void) {}
  virtual ~cPluginDummydevice() {}
  virtual const char *Version(void) { return VERSION; }
  virtual const char *Description(void) { return DESCRIPTION; }
  virtual const char *CommandLineHelp(void) { return NULL; }
  virtual bool ProcessArgs(int argc, char *argv[]) { return true; }
  virtual bool Initialize(void);
  virtual bool Start(void) { return true; }
  virtual void Housekeeping(void) {}
  virtual const char *MainMenuEntry(void) { return NULL; }
  virtual cOsdObject *MainMenuAction(void) { return NULL; }
  virtual cMenuSetupPage *SetupMenu(void) { return NULL; }
  virtual bool SetupParse(const char *Name, const char *Value) { return false; };
};

bool cPluginDummydevice::Initialize(void)
{
  // Initialize any background activities the plugin shall perform.
  new cDummyDevice();

  return true;
}

VDRPLUGINCREATOR(cPluginDummydevice); // Don't touch this!
