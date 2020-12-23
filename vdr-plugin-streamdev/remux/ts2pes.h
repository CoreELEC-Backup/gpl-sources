/*
 * ts2pes.h: A streaming MPEG2 remultiplexer
 *
 * This file is based on a copy of remux.h from Klaus Schmidinger's
 * VDR, version 1.6.0.
 *
 * $Id: ts2pes.h,v 1.4 2009/07/06 06:11:11 schmirl Exp $
 */

#ifndef VDR_STREAMDEV_TS2PES_H
#define VDR_STREAMDEV_TS2PES_H

#include "remux/tsremux.h"
#include "server/streamer.h"

#define MAXTRACKS 64

namespace Streamdev {

class cTS2PES;

class cTS2PESRemux: public cTSRemux {
private:
  bool noVideo;
  bool synced;
  int skipped;
  cTS2PES *ts2pes[MAXTRACKS];
  int numTracks;
  cStreamdevBuffer *resultBuffer;
  int resultSkipped;
public:
  cTS2PESRemux(int VPid, const int *APids, const int *DPids, const int *SPids);
       ///< Creates a new remuxer for the given PIDs. VPid is the video PID, while
       ///< APids, DPids and SPids are pointers to zero terminated lists of audio,
       ///< dolby and subtitle PIDs (the pointers may be NULL if there is no such
       ///< PID).
  virtual ~cTS2PESRemux();
  int Put(const uchar *Data, int Count);
       ///< Puts at most Count bytes of Data into the remuxer.
       ///< \return Returns the number of bytes actually consumed from Data.
  uchar *Get(int &Count);
       ///< Gets all currently available data from the remuxer.
       ///< \return Count contains the number of bytes the result points to, and
  void Del(int Count);
       ///< Deletes Count bytes from the remuxer. Count must be the number returned
       ///< from a previous call to Get(). Several calls to Del() with fractions of
       ///< a previously returned Count may be made, but the total sum of all Count
       ///< values must be exactly what the previous Get() has returned.
  void Clear(void);
       ///< Clears the remuxer of all data it might still contain, keeping the PID
       ///< settings as they are.
  };

} // namespace Streamdev

#endif // VDR_STREAMDEV_TS2PES_H
