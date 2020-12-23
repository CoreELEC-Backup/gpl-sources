/*
 *      vdr-plugin-robotv - roboTV server plugin for VDR
 *
 *      Copyright (C) 2016 Alexander Pipelka
 *
 *      https://github.com/pipelka/vdr-plugin-robotv
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
 *  along with this program; if not, write to the Free Software
 *  the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 *  http://www.gnu.org/copyleft/gpl.html
 *
 */

#ifndef ROBOTV_CHANNELCACHE_H
#define ROBOTV_CHANNELCACHE_H

#include <thread>
#include <string>
#include <vdr/channels.h>

#include "config/config.h"
#include "db/storage.h"
#include "robotvdmx/streambundle.h"

class ChannelCache : public roboTV::Storage {
public:

    void add(uint32_t channeluid, const StreamBundle& channel);

    StreamBundle lookup(uint32_t channeluid);

    void enable(const cChannel* channel, bool enabled = true);

    void enable(uint32_t channeluid, bool enabled = true);

    bool isEnabled(const cChannel* channel);

    bool isEnabled(uint32_t channeluid);

    void gc();

    static ChannelCache& instance();

protected:

    ChannelCache();

private:

    void addDb(uint32_t channeluid, const StreamBundle& channel);

    void createDb();

    std::string createStringLiteral(uint8_t* data, int length);

};

#endif // ROBOTV_CHANNELCACHE_H
