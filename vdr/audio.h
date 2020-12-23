/*
 * audio.h: The basic audio interface
 *
 * See the main source file 'vdr.c' for copyright information and
 * how to reach the author.
 *
 * $Id: audio.h 4.0 2008/07/06 11:39:21 kls Exp $
 */

#ifndef __AUDIO_H
#define __AUDIO_H

#include "thread.h"
#include "tools.h"

class cAudio : public cListObject {
protected:
  cAudio(void);
public:
  virtual ~cAudio();
  virtual void Play(const uchar *Data, int Length, uchar Id) = 0;
       ///< Plays the given block of audio Data. Must return as soon as possible.
       ///< If the entire block of data can't be processed immediately, it must
       ///< be copied and processed in a separate thread. The Data is always a
       ///< complete PES audio packet. Id indicates the type of audio data this
       ///< packet holds.
  virtual void PlayTs(const uchar *Data, int Length) = 0;
       ///< Plays the given block of audio Data. Must return as soon as possible.
       ///< If the entire block of data can't be processed immediately, it must
       ///< be copied and processed in a separate thread. The Data is always a
       ///< complete TS audio packet.
  virtual void Mute(bool On) = 0;
       ///< Immediately sets the audio device to be silent (On==true) or to
       ///< normal replay (On==false).
  virtual void Clear(void) = 0;
       ///< Clears all data that might still be awaiting processing.
  };

class cAudios : public cList<cAudio> {
public:
  void PlayAudio(const uchar *Data, int Length, uchar Id);
  void PlayTsAudio(const uchar *Data, int Length);
  void MuteAudio(bool On);
  void ClearAudio(void);
  };

extern cAudios Audios;

class cExternalAudio : public cAudio {
private:
  char *command;
  cPipe pipe;
  bool mute;
public:
  cExternalAudio(const char *Command);
  virtual ~cExternalAudio();
  virtual void Play(const uchar *Data, int Length, uchar Id);
  virtual void PlayTs(const uchar *Data, int Length);
  virtual void Mute(bool On);
  virtual void Clear(void);
  };

#endif //__AUDIO_H
