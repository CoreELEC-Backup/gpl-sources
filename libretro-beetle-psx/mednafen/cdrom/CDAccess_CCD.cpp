/* Mednafen - Multi-system Emulator
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "../mednafen.h"
#include "../error.h"
#include "../general.h"
#include <compat/msvc.h>
#include "CDAccess_CCD.h"
#include "CDUtility.h"

#include <limits>
#include <limits.h>
#include <map>

typedef std::map<std::string, std::string> CCD_Section;

template<typename T>
static T CCD_ReadInt(CCD_Section &s, const std::string &propname, const bool have_defval = false, const int defval = 0)
{
   const char *vp;
   char *ep           = NULL;
   int scan_base      = 10;
   size_t scan_offset = 0;
   long ret           = 0;
   CCD_Section::iterator zit = s.find(propname);

   if(zit == s.end())
   {
      if(have_defval)
         return defval;
      throw MDFN_Error(0, "Missing property: %s", propname.c_str());
   }

   const std::string &v = zit->second;

   if(v.length() >= 3 && v[0] == '0' && v[1] == 'x')
   {
      scan_base = 16;
      scan_offset = 2;
   }

   vp = v.c_str() + scan_offset;

   if(std::numeric_limits<T>::is_signed)
      ret = strtol(vp, &ep, scan_base);
   else
      ret = strtoul(vp, &ep, scan_base);

   if(!vp[0] || ep[0])
      throw MDFN_Error(0, "Property %s: Malformed integer: %s", propname.c_str(), v.c_str());

   return ret;
}


CDAccess_CCD::CDAccess_CCD(bool *success, const char *path, bool image_memcache) : img_stream(NULL), sub_stream(NULL), img_numsectors(0)
{
   TOC_Clear(&tocd);
   if (!Load(path, image_memcache))
      *success = false;
}

bool CDAccess_CCD::Load(const char *path, bool image_memcache)
{
   FileStream cf(path, MODE_READ);
   std::map<std::string, CCD_Section> Sections;
   std::string linebuf;
   std::string cur_section_name;
   std::string dir_path, file_base, file_ext;
   char img_extsd[4] = { 'i', 'm', 'g', 0 };
   char sub_extsd[4] = { 's', 'u', 'b', 0 };

   MDFN_GetFilePathComponents(path, &dir_path, &file_base, &file_ext);

   if(file_ext.length() == 4 && file_ext[0] == '.')
   {
      int i;
      signed char        av = -1;
      signed char extupt[3] = { -1, -1, -1 };

      for(i = 1; i < 4; i++)
      {
         if(file_ext[i] >= 'A' && file_ext[i] <= 'Z')
            extupt[i - 1] = 'A' - 'a';
         else if(file_ext[i] >= 'a' && file_ext[i] <= 'z')
            extupt[i - 1] = 0;
      }

      for(i = 0; i < 3; i++)
      {
         if(extupt[i] != -1)
            av = extupt[i];
         else
            extupt[i] = av;
      }

      if(av == -1)
         av = 0;

      for(i = 0; i < 3; i++)
      {
         if(extupt[i] == -1)
            extupt[i] = av;
      }

      for(i = 0; i < 3; i++)
      {
         img_extsd[i] += extupt[i];
         sub_extsd[i] += extupt[i];
      }
   }

   //printf("%s %d %d %d\n", file_ext.c_str(), extupt[0], extupt[1], extupt[2]);

   linebuf.reserve(256);

   while(cf.get_line(linebuf) >= 0)
   {
      MDFN_trim(linebuf);

      if(linebuf.length() == 0)	// Skip blank lines.
         continue;

      if(linebuf[0] == '[')
      {
         if(linebuf.length() < 3 || linebuf[linebuf.length() - 1] != ']')
         {
            MDFN_Error(0, "Malformed section specifier: %s", linebuf.c_str());
            return false;
         }

         cur_section_name = linebuf.substr(1, linebuf.length() - 2);
         MDFN_strtoupper(cur_section_name);
      }
      else
      {
         std::string k, v;
         const size_t feqpos = linebuf.find('=');
         const size_t leqpos = linebuf.rfind('=');

         if(feqpos == std::string::npos || feqpos != leqpos)
         {
            MDFN_Error(0, "Malformed value pair specifier: %s", linebuf.c_str());
            return false;
         }

         k = linebuf.substr(0, feqpos);
         v = linebuf.substr(feqpos + 1);

         MDFN_trim(k);
         MDFN_trim(v);

         MDFN_strtoupper(k);

         Sections[cur_section_name][k] = v;
      }
   }

   {
      unsigned te;
      CCD_Section            &ds = Sections["DISC"];
      unsigned       toc_entries = CCD_ReadInt<unsigned>(ds, "TOCENTRIES");
      unsigned      num_sessions = CCD_ReadInt<unsigned>(ds, "SESSIONS");
      bool data_tracks_scrambled = CCD_ReadInt<unsigned>(ds, "DATATRACKSSCRAMBLED");

      if(num_sessions != 1)
      {
         MDFN_Error(0, "Unsupported number of sessions: %u", num_sessions);
         return false;
      }

      if(data_tracks_scrambled)
      {
         MDFN_Error(0, "Scrambled CCD data tracks currently not supported.");
         return false;
      }

      //printf("MOO: %d\n", toc_entries);

      for(te = 0; te < toc_entries; te++)
      {
         char tmpbuf[64];
         snprintf(tmpbuf, sizeof(tmpbuf), "ENTRY %u", te);
         CCD_Section & ts = Sections[std::string(tmpbuf)];
         unsigned session = CCD_ReadInt<unsigned>(ts, "SESSION");
         uint8_t    point = CCD_ReadInt<uint8>(ts, "POINT");
         uint8_t      adr = CCD_ReadInt<uint8>(ts, "ADR");
         uint8_t  control = CCD_ReadInt<uint8>(ts, "CONTROL");
         uint8_t     pmin = CCD_ReadInt<uint8>(ts, "PMIN");
         uint8_t     psec = CCD_ReadInt<uint8>(ts, "PSEC");
         uint8_t   pframe = CCD_ReadInt<uint8>(ts, "PFRAME");
         signed      plba = CCD_ReadInt<signed>(ts, "PLBA");

         if(session != 1)
         {
            MDFN_Error(0, "Unsupported TOC entry Session value: %u", session);
            return false;
         }

         // Reference: ECMA-394, page 5-14
         if (point >= 1 && point <= 99)
         {
            tocd.tracks[point].adr = adr;
            tocd.tracks[point].control = control;
            tocd.tracks[point].lba = plba;
         }
         else
            switch(point)
            {
               default:
                  MDFN_Error(0, "Unsupported TOC entry Point value: %u", point);
                  return false;
               case 0xA0:
                  tocd.first_track = pmin;
                  tocd.disc_type = psec;
                  break;

               case 0xA1:
                  tocd.last_track = pmin;
                  break;

               case 0xA2:
                  tocd.tracks[100].adr = adr;
                  tocd.tracks[100].control = control;
                  tocd.tracks[100].lba = plba;
                  break;
            }
      }
   }

   /* Convenience leadout track duplication. */
   if(tocd.last_track < 99)
      tocd.tracks[tocd.last_track + 1] = tocd.tracks[100];

   /* Open image stream. */
   {
      std::string image_path = MDFN_EvalFIP(dir_path, file_base + std::string(".") + std::string(img_extsd), true);
      FileStream *str        = new FileStream(image_path.c_str(), MODE_READ);

      if(image_memcache)
         img_stream = new MemoryStream(str);
      else
         img_stream = str;

      int64 ss = img_stream->size();

      if(ss % 2352)
      {
         MDFN_Error(0, "CCD image size is not evenly divisible by 2352.");
         return false;
      }

      img_numsectors = ss / 2352;  
   }

   {
      /* Open subchannel stream */
      std::string sub_path = MDFN_EvalFIP(dir_path, file_base + std::string(".") + std::string(sub_extsd), true);
      FileStream *str      = new FileStream(sub_path.c_str(), MODE_READ);

      if(image_memcache)
         sub_stream = new MemoryStream(str);
      else
         sub_stream = str;

      if(sub_stream->size() != (int64)img_numsectors * 96)
      {
         MDFN_Error(0, "CCD SUB file size mismatch.");
         return false;
      }
   }

   CheckSubQSanity();

   return true;
}

//
// Checks for Q subchannel mode 1(current time) data that has a correct checksum, but the data is nonsensical or corrupted nonetheless; this is the
// case for some bad rips floating around on the Internet.  Allowing these bad rips to be used will cause all sorts of problems during emulation, so we
// error out here if a bad rip is detected.
//
// This check is not as aggressive or exhaustive as it could be, and will not detect all potential Q subchannel rip errors; as such, it should definitely NOT be
// used in an effort to "repair" a broken rip.
//
bool CDAccess_CCD::CheckSubQSanity(void)
{
   size_t s;
   size_t checksum_pass_counter = 0;
   int                 prev_lba = INT_MAX;
   uint8_t           prev_track = 0;

   // Silence GCC warning
   (void)prev_lba;

   for(s = 0; s < img_numsectors; s++)
   {
      uint8_t adr;
      union
      {
         uint8 full[96];
         struct
         {
            uint8 pbuf[12];
            uint8 qbuf[12];
         };
      } buf;

      sub_stream->seek(s * 96, SEEK_SET);
      sub_stream->read(buf.full, 96);

      if(!subq_check_checksum(buf.qbuf))
         continue;

      adr = buf.qbuf[0] & 0xF;

      if(adr == 0x01)
      {
         int lba;
         uint8_t track;
         uint8_t track_bcd = buf.qbuf[1];
         uint8_t index_bcd = buf.qbuf[2];
         uint8_t rm_bcd = buf.qbuf[3];
         uint8_t rs_bcd = buf.qbuf[4];
         uint8_t rf_bcd = buf.qbuf[5];
         uint8_t am_bcd = buf.qbuf[7];
         uint8_t as_bcd = buf.qbuf[8];
         uint8_t af_bcd = buf.qbuf[9];

         //printf("%2x %2x %2x\n", am_bcd, as_bcd, af_bcd);

         if(!BCD_is_valid(track_bcd) || !BCD_is_valid(index_bcd) || !BCD_is_valid(rm_bcd) || !BCD_is_valid(rs_bcd) || !BCD_is_valid(rf_bcd) ||
               !BCD_is_valid(am_bcd) || !BCD_is_valid(as_bcd) || !BCD_is_valid(af_bcd) ||
               rs_bcd > 0x59 || rf_bcd > 0x74 || as_bcd > 0x59 || af_bcd > 0x74)
         {
            MDFN_Error(0, "Garbage subchannel Q data detected(bad BCD/out of range): %02x:%02x:%02x %02x:%02x:%02x", rm_bcd, rs_bcd, rf_bcd, am_bcd, as_bcd, af_bcd);
            return false;
         }

         lba = ((BCD_to_U8(am_bcd) * 60 + BCD_to_U8(as_bcd)) * 75 + BCD_to_U8(af_bcd)) - 150;
         track = BCD_to_U8(track_bcd);

         prev_lba = lba;

         if(track < prev_track)
         {
            MDFN_Error(0, "Garbage subchannel Q data detected(bad track number)");
            return false;
         }

         prev_track = track;
         checksum_pass_counter++;
      }
   }

   //printf("%u/%u\n", checksum_pass_counter, img_numsectors);
   
   return true;
}

void CDAccess_CCD::Cleanup(void)
{
   if(img_stream)
   {
      delete img_stream;
      img_stream = NULL;
   }

   if(sub_stream)
   {
      delete sub_stream;
      sub_stream = NULL;
   }
}

CDAccess_CCD::~CDAccess_CCD()
{
   Cleanup();
}

bool CDAccess_CCD::Read_Raw_Sector(uint8 *buf, int32 lba)
{
   uint8_t sub_buf[96];

   if(lba < 0 || (size_t)lba >= img_numsectors)
   {
      MDFN_Error(0, "LBA out of range.");
      return false;
   }

   img_stream->seek(lba * 2352, SEEK_SET);
   img_stream->read(buf, 2352);

   sub_stream->seek(lba * 96, SEEK_SET);
   sub_stream->read(sub_buf, 96);

   subpw_interleave(sub_buf, buf + 2352);

   return true;
}

bool CDAccess_CCD::Read_Raw_PW(uint8_t *buf, int32_t lba)
{
   uint8_t sub_buf[96];

   if(lba < 0 || (size_t)lba >= img_numsectors)
   {
      MDFN_Error(0, "LBA out of range.");
      return false;
   }

   sub_stream->seek(lba * 96, SEEK_SET);
   sub_stream->read(sub_buf, 96);

   subpw_interleave(sub_buf, buf);

   return true;
}

bool CDAccess_CCD::Read_TOC(TOC *toc)
{
   *toc = tocd;
   return true;
}

void CDAccess_CCD::Eject(bool eject_status)
{

}
