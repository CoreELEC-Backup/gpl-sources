/*
 *  Copyright (C) 2006 Team MediaPortal
 *  http://www.team-mediaportal.com
 *
 *  This Program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  This Program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with GNU Make; see the file COPYING.  If not, write to
 *  the Free Software Foundation, 51 Franklin Street, Fifth Floor, Boston,
 *  MA 02110-1335  USA
 *  http://www.gnu.org/copyleft/gpl.html
 *
 */

#include "os-dependent.h"
#include <stdint.h>
#include <kodi/General.h> //for kodi::Log
#include <time.h>
#include "PatParser.h"
#include "TSHeader.h"

namespace MPTV
{
    CPatParser::CPatParser(void)
    {
        m_packetsToSkip = 0;
        m_packetsReceived = 0;
        m_pCallback = NULL;
        Reset();
        SetPid(0);
        m_iState = Idle;
    }

    CPatParser::~CPatParser(void)
    {
        CleanUp();
    }

    void CPatParser::SetCallBack(IPatParserCallback* callback)
    {
        m_pCallback = callback;
    }

    void CPatParser::CleanUp()
    {
        for (size_t i = 0; i < m_pmtParsers.size(); ++i)
        {
            CPmtParser* parser = m_pmtParsers[i];
            delete parser;
        }
        m_pmtParsers.clear();
        m_iPatTableVersion = -1;
    }

    void  CPatParser::Reset()
    {
        // Dump();
        kodi::Log(ADDON_LOG_DEBUG, "PatParser:Reset()");
        CSectionDecoder::Reset();
        CleanUp();
        m_packetsReceived = 0;
        m_iPatTableVersion = -1;
        m_iState = Parsing;
    }


    size_t CPatParser::Count()
    {
        size_t count = m_pmtParsers.size();

        if (count == 0)
            return 0;

        for (size_t i = 0; i < m_pmtParsers.size(); ++i)
        {
            CPmtParser* parser = m_pmtParsers[i];
            if (true == parser->IsReady())
            {
                return count;
            }
        }
        return 0;
    }

    bool CPatParser::GetChannel(size_t index, CChannelInfo& info)
    {
        static CChannelInfo unknownChannel;

        if (index > Count())
        {
            return false;
        }

        CPmtParser* parser = m_pmtParsers[index];

        if (false == parser->IsReady())
        {
            return false;
        }

        info.PidTable = parser->GetPidInfo();
        m_iState = Idle;
        return true;
    }

    void CPatParser::SkipPacketsAtStart(int64_t packets)
    {
        m_packetsToSkip = packets;
        m_packetsReceived = 0;
    }

    void CPatParser::OnTsPacket(byte* tsPacket)
    {
        m_packetsReceived++;

        if (m_packetsReceived > m_packetsToSkip)
        {
            for (size_t i = 0; i < m_pmtParsers.size(); ++i)
            {
                CPmtParser* parser = m_pmtParsers[i];
                parser->OnTsPacket(tsPacket);
            }
            CSectionDecoder::OnTsPacket(tsPacket);
        }

        if (m_iState == Parsing && m_pCallback != NULL)
        {
            for (size_t i = 0; i < m_pmtParsers.size(); ++i)
            {
                CPmtParser* parser = m_pmtParsers[i];
                if (true == parser->IsReady())
                {
                    CChannelInfo info;
                    if (GetChannel(i, info))
                    {
                        m_iState = Idle;

                        info.PatVersion = m_iPatTableVersion;
                        m_pCallback->OnNewChannel(info);
                        m_iState = Parsing;
                        return;
                    }
                }
            }
        }
    }

    void CPatParser::OnNewSection(CSection& section)
    {
        if (section.table_id != 0)
            return;

        try
        {
            //int section_syntax_indicator = (section.Data[1]>>7) & 1;
            //int transport_stream_id = section.table_id_extension;

            if (section.version_number != m_iPatTableVersion)
            {
                kodi::Log(ADDON_LOG_DEBUG, "PatParser: new pat table %d->%d", m_iPatTableVersion, section.version_number); //was commented out
                CleanUp();
                m_iPatTableVersion = section.version_number;
                m_iState = Parsing;
            }
            //kodi::Log(ADDON_LOG_DEBUG, "DecodePat  %d section:%d lastsection:%d sectionlen:%d",
            //            version_number,section_number,last_section_number,section_length);

            int loop = (section.section_length - 9) / 4;
            for (int i = 0; i < loop; i++)
            {
                int offset = (8 + (i * 4));
                int pmtPid = ((section.Data[offset + 2] & 0x1F) << 8) + section.Data[offset + 3];
                if (pmtPid < 0x10 || pmtPid >= 0x1fff)
                {
                    //invalid pmt pid
                    return;
                }

                bool found = false;
                for (size_t idx = 0; idx < m_pmtParsers.size(); idx++)
                {
                    CPmtParser* pmtParser = m_pmtParsers[idx];
                    if (pmtParser->GetPid() == pmtPid)
                    {
                        found = true;
                        break;
                    }
                }
                if (!found && pmtPid >= 0x10)
                {
                    CPmtParser* pmtParser = new CPmtParser();
                    pmtParser->SetPid(pmtPid);
                    //pmtParser->SetPmtCallBack(this);
                    m_pmtParsers.push_back(pmtParser);
                    kodi::Log(ADDON_LOG_DEBUG, "PatParser:  add pmt# %u pid: %x", (unsigned int) m_pmtParsers.size(), pmtPid);
                }
            }
        }
        catch (...)
        {
            kodi::Log(ADDON_LOG_DEBUG, "Exception in PatParser");
        }
    }

    void CPatParser::Dump()
    {
        for (size_t i = 0; i < Count(); ++i)
        {
            CChannelInfo info;
            if (GetChannel(i, info))
            {
                kodi::Log(ADDON_LOG_DEBUG, "%u) onid:%x tsid:%x sid:%x major:%d minor:%x freq:%x type:%d provider:%s service:%s", (unsigned int) i,
                    info.NetworkId, info.TransportId, info.ServiceId, info.MajorChannel, info.MinorChannel, info.Frequency, info.ServiceType, info.ProviderName, info.ServiceName);
                info.PidTable.LogPIDs();
            }
            else
            {
                kodi::Log(ADDON_LOG_DEBUG, "%u) not found", (unsigned int) i);
            }
        }
    }
}

