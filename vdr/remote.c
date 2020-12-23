/*
 * remote.c: General Remote Control handling
 *
 * See the main source file 'vdr.c' for copyright information and
 * how to reach the author.
 *
 * $Id: remote.c 4.1 2019/05/13 13:01:44 kls Exp $
 */

#include "remote.h"
#include <fcntl.h>
#define __STDC_FORMAT_MACROS // Required for format specifiers
#include <inttypes.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/types.h>
#include <sys/time.h>
#include <unistd.h>
#include "tools.h"

// --- cRemote ---------------------------------------------------------------

#define INITTIMEOUT   10000 // ms
#define REPEATTIMEOUT  1000 // ms

eKeys cRemote::keys[MaxKeys];
int cRemote::in = 0;
int cRemote::out = 0;
cTimeMs cRemote::repeatTimeout(-1);
cRemote *cRemote::learning = NULL;
char *cRemote::unknownCode = NULL;
cMutex cRemote::mutex;
cCondVar cRemote::keyPressed;
const char *cRemote::keyMacroPlugin = NULL;
const char *cRemote::callPlugin = NULL;
bool cRemote::enabled = true;
time_t cRemote::lastActivity = 0;

cRemote::cRemote(const char *Name)
{
  name = Name ? strdup(Name) : NULL;
  Remotes.Add(this);
}

cRemote::~cRemote()
{
  Remotes.Del(this, false);
  free(name);
}

const char *cRemote::GetSetup(void)
{
  return Keys.GetSetup(Name());
}

void cRemote::PutSetup(const char *Setup)
{
  Keys.PutSetup(Name(), Setup);
}

bool cRemote::Initialize(void)
{
  if (Ready()) {
     char *NewCode = NULL;
     eKeys Key = Get(INITTIMEOUT, &NewCode);
     if (Key != kNone || NewCode)
        return true;
     }
  return false;
}

void cRemote::Clear(void)
{
  cMutexLock MutexLock(&mutex);
  in = out = 0;
  if (learning) {
     free(unknownCode);
     unknownCode = NULL;
     }
}

bool cRemote::Put(eKeys Key, bool AtFront)
{
  if (Key != kNone) {
     cMutexLock MutexLock(&mutex);
     if (in != out && (keys[out] & k_Repeat) && (Key & k_Release))
        Clear();
     int d = out - in;
     if (d <= 0)
        d = MaxKeys + d;
     if (d - 1 > 0) {
        if (AtFront) {
           if (--out < 0)
              out = MaxKeys - 1;
           keys[out] = Key;
           }
        else {
           keys[in] = Key;
           if (++in >= MaxKeys)
              in = 0;
           }
        keyPressed.Broadcast();
        return true;
        }
     return false;
     }
  return true; // only a real key shall report an overflow!
}

bool cRemote::PutMacro(eKeys Key)
{
  const cKeyMacro *km = KeyMacros.Get(Key);
  if (km) {
     keyMacroPlugin = km->Plugin();
     cMutexLock MutexLock(&mutex);
     for (int i = km->NumKeys(); --i > 0; ) {
         if (!Put(km->Macro()[i], true))
            return false;
         }
     }
  return true;
}

bool cRemote::Put(uint64_t Code, bool Repeat, bool Release)
{
  char buffer[32];
  snprintf(buffer, sizeof(buffer), "%016" PRIX64, Code);
  return Put(buffer, Repeat, Release);
}

bool cRemote::Put(const char *Code, bool Repeat, bool Release)
{
  if (learning && this != learning)
     return false;
  eKeys Key = Keys.Get(Name(), Code);
  if (Key != kNone) {
     if (Repeat)
        Key = eKeys(Key | k_Repeat);
     if (Release)
        Key = eKeys(Key | k_Release);
     return Put(Key);
     }
  if (learning) {
     free(unknownCode);
     unknownCode = strdup(Code);
     keyPressed.Broadcast();
     }
  return false;
}

bool cRemote::CallPlugin(const char *Plugin)
{
  cMutexLock MutexLock(&mutex);
  if (!callPlugin) {
     callPlugin = Plugin;
     Put(k_Plugin);
     return true;
     }
  return false;
}

const char *cRemote::GetPlugin(void)
{
  cMutexLock MutexLock(&mutex);
  const char *p = keyMacroPlugin;
  if (p)
     keyMacroPlugin = NULL;
  else {
     p = callPlugin;
     callPlugin = NULL;
     }
  return p;
}

bool cRemote::HasKeys(void)
{
  cMutexLock MutexLock(&mutex);
  return in != out && !(keys[out] & k_Repeat);
}

eKeys cRemote::Get(int WaitMs, char **UnknownCode)
{
  for (;;) {
      cMutexLock MutexLock(&mutex);
      if (in != out) {
         eKeys k = keys[out];
         if (++out >= MaxKeys)
            out = 0;
         if ((k & k_Repeat) != 0)
            repeatTimeout.Set(REPEATTIMEOUT);
         TriggerLastActivity();
         return enabled ? k : kNone;
         }
      else if (!WaitMs || !keyPressed.TimedWait(mutex, WaitMs) && repeatTimeout.TimedOut())
         return kNone;
      else if (learning && UnknownCode && unknownCode) {
         *UnknownCode = unknownCode;
         unknownCode = NULL;
         return kNone;
         }
      }
}

void cRemote::TriggerLastActivity(void)
{
  lastActivity = time(NULL);
}

// --- cRemotes --------------------------------------------------------------

cRemotes Remotes;

// --- cKbdRemote ------------------------------------------------------------

struct tKbdMap {
  eKbdFunc func;
  uint64_t code;
  };

static tKbdMap KbdMap[] = {
  { kfF1,     0x0000001B5B31317EULL },
  { kfF2,     0x0000001B5B31327EULL },
  { kfF3,     0x0000001B5B31337EULL },
  { kfF4,     0x0000001B5B31347EULL },
  { kfF5,     0x0000001B5B31357EULL },
  { kfF6,     0x0000001B5B31377EULL },
  { kfF7,     0x0000001B5B31387EULL },
  { kfF8,     0x0000001B5B31397EULL },
  { kfF9,     0x0000001B5B32307EULL },
  { kfF10,    0x0000001B5B32317EULL },
  { kfF11,    0x0000001B5B32327EULL },
  { kfF12,    0x0000001B5B32337EULL },
  { kfUp,     0x00000000001B5B41ULL },
  { kfDown,   0x00000000001B5B42ULL },
  { kfLeft,   0x00000000001B5B44ULL },
  { kfRight,  0x00000000001B5B43ULL },
  { kfHome,   0x00000000001B5B48ULL },
  { kfEnd,    0x00000000001B5B46ULL },
  { kfPgUp,   0x000000001B5B357EULL },
  { kfPgDown, 0x000000001B5B367EULL },
  { kfIns,    0x000000001B5B327EULL },
  { kfDel,    0x000000001B5B337EULL },
  { kfNone,   0x0000000000000000ULL }
  };

bool cKbdRemote::kbdAvailable = false;
bool cKbdRemote::rawMode = false;

cKbdRemote::cKbdRemote(void)
:cRemote("KBD")
,cThread("KBD remote control")
{
  tcgetattr(STDIN_FILENO, &savedTm);
  struct termios tm;
  if (tcgetattr(STDIN_FILENO, &tm) == 0) {
     tm.c_iflag = 0;
     tm.c_lflag &= ~(ICANON | ECHO);
     tm.c_cc[VMIN] = 0;
     tm.c_cc[VTIME] = 0;
     tcsetattr(STDIN_FILENO, TCSANOW, &tm);
     }
  kbdAvailable = true;
  systemIsUtf8 = !cCharSetConv::SystemCharacterTable() || strcmp(cCharSetConv::SystemCharacterTable(), "UTF-8") == 0;
  Start();
}

cKbdRemote::~cKbdRemote()
{
  kbdAvailable = false;
  Cancel(3);
  tcsetattr(STDIN_FILENO, TCSANOW, &savedTm);
}

void cKbdRemote::SetRawMode(bool RawMode)
{
  rawMode = RawMode;
}

uint64_t cKbdRemote::MapFuncToCode(int Func)
{
  for (tKbdMap *p = KbdMap; p->func != kfNone; p++) {
      if (p->func == Func)
         return p->code;
      }
  return (Func <= 0xFF) ? Func : 0;
}

int cKbdRemote::MapCodeToFunc(uint64_t Code)
{
  for (tKbdMap *p = KbdMap; p->func != kfNone; p++) {
      if (p->code == Code)
         return p->func;
      }
  if (Code <= 0xFF)
     return Code;
  return kfNone;
}

void cKbdRemote::PutKey(uint64_t Code, bool Repeat, bool Release)
{
  if (rawMode || (!Put(Code, Repeat, Release) && !IsLearning())) {
     if (int func = MapCodeToFunc(Code))
        Put(KBDKEY(func), Repeat, Release);
     }
}

int cKbdRemote::ReadKey(void)
{
  cPoller Poller(STDIN_FILENO);
  if (Poller.Poll(Setup.RcRepeatDelta * 3 / 2)) {
     uchar ch = 0;
     int r = safe_read(STDIN_FILENO, &ch, 1);
     if (r == 1)
        return ch;
     if (r < 0)
        LOG_ERROR_STR("cKbdRemote");
     }
  return -1;
}

uint64_t cKbdRemote::ReadKeySequence(void)
{
  uint64_t k = 0;
  int key1;

  if ((key1 = ReadKey()) >= 0) {
     k = key1;
     if (systemIsUtf8 && (key1 & 0xC0) == 0xC0) {
        char bytes[4] = { 0 };
        bytes[0] = key1;
        int bytescount = 1;
        if ((key1 & 0xF0) == 0xF0)
           bytescount = 3;
        else if ((key1 & 0xE0) == 0xE0)
           bytescount = 2;
        for (int i = 0; i < bytescount; i++) {
            if ((key1 = ReadKey()) >= 0)
               bytes[i + 1] = key1;
            }
        k = Utf8CharGet(bytes);
        if (k > 0xFF)
           k = 0;
        }
     else if (key1 == 0x1B) {
        // Start of escape sequence
        if ((key1 = ReadKey()) >= 0) {
           k <<= 8;
           k |= key1 & 0xFF;
           switch (key1) {
             case 0x4F: // 3-byte sequence
                  if ((key1 = ReadKey()) >= 0) {
                     k <<= 8;
                     k |= key1 & 0xFF;
                     }
                  break;
             case 0x5B: // 3- or more-byte sequence
                  if ((key1 = ReadKey()) >= 0) {
                     k <<= 8;
                     k |= key1 & 0xFF;
                     switch (key1) {
                       case 0x31 ... 0x3F: // more-byte sequence
                       case 0x5B: // strange, may apparently occur
                            do {
                               if ((key1 = ReadKey()) < 0)
                                  break; // Sequence ends here
                               k <<= 8;
                               k |= key1 & 0xFF;
                               } while (key1 != 0x7E);
                            break;
                       default: ;
                       }
                     }
                  break;
             default: ;
             }
           }
        }
     }
  return k;
}

void cKbdRemote::Action(void)
{
  cTimeMs FirstTime;
  cTimeMs LastTime;
  uint64_t FirstCommand = 0;
  uint64_t LastCommand = 0;
  bool Delayed = false;
  bool Repeat = false;

  while (Running()) {
        uint64_t Command = ReadKeySequence();
        if (Command) {
           if (Command == LastCommand) {
              // If two keyboard events with the same command come in without an intermediate
              // timeout, this is a long key press that caused the repeat function to kick in:
              Delayed = false;
              FirstCommand = 0;
              if (FirstTime.Elapsed() < (uint)Setup.RcRepeatDelay)
                 continue; // repeat function kicks in after a short delay
              if (LastTime.Elapsed() < (uint)Setup.RcRepeatDelta)
                 continue; // skip same keys coming in too fast
              PutKey(Command, true);
              Repeat = true;
              LastTime.Set();
              }
           else if (Command == FirstCommand) {
              // If the same command comes in twice with an intermediate timeout, we
              // need to delay the second command to see whether it is going to be
              // a repeat function or a separate key press:
              Delayed = true;
              }
           else {
              // This is a totally new key press, so we accept it immediately:
              PutKey(Command);
              Delayed = false;
              FirstCommand = Command;
              FirstTime.Set();
              }
           }
        else if (Repeat) {
           // Timeout after a repeat function, so we generate a 'release':
           PutKey(LastCommand, false, true);
           Repeat = false;
           }
        else if (Delayed && FirstCommand) {
           // Timeout after two normal key presses of the same key, so accept the
           // delayed key:
           PutKey(FirstCommand);
           Delayed = false;
           FirstCommand = 0;
           FirstTime.Set();
           }
        else if (FirstCommand && FirstTime.Elapsed() > (uint)Setup.RcRepeatDelay) {
           // Don't wait too long for that second key press:
           Delayed = false;
           FirstCommand = 0;
           }
        LastCommand = Command;
        }
}
