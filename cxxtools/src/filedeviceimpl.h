/*
 * Copyright (C) 2006-2007 Laurentiu-Gheorghe Crisan
 * Copyright (C) 2006-2007 Marc Boris Duerner
 * Copyright (C) 2006-2007 PTV AG
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * As a special exception, you may use this file as part of a free
 * software library without restriction. Specifically, if other files
 * instantiate templates or use macros or inline functions from this
 * file, or you compile this file and link it with other files to
 * produce an executable, this file does not by itself cause the
 * resulting executable to be covered by the GNU General Public
 * License. This exception does not however invalidate any other
 * reasons why the executable file might be covered by the GNU Library
 * General Public License.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */
#include "cxxtools/api.h"
#include "cxxtools/ioerror.h"
#include "cxxtools/filedevice.h"
#include "cxxtools/iodevice.h"
#include "iodeviceimpl.h"

namespace cxxtools
{

class FileDeviceImpl : public IODeviceImpl
{
    public:
        typedef FileDevice::pos_type pos_type;
        typedef FileDevice::off_type off_type;

    public:
        FileDeviceImpl(FileDevice& device);

        ~FileDeviceImpl();

        bool seekable() const;

        pos_type seek(off_type offset, std::ios::seekdir sd);

        void resize(off_type size);

        size_t size();

        size_t peek(char* buffer, size_t count);
};

} //namespace cxxtools
