/*
 * Copyright (C) 2006-2008 Marc Boris Duerner
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
#include "fileimpl.h"
#include "cxxtools/file.h"
#include "cxxtools/directory.h"

namespace cxxtools {

File::File()
{
}


File::File(const std::string& path)
: _path(path)
{
    if( ! File::exists( path.c_str() ) )
        throw FileNotFound(path);
}


File::File(const FileInfo& fi)
: _path( fi.path() )
{
    if( ! fi.isFile() )
        throw FileNotFound(fi.path());
}


File::File(const File& file)
: _path( file.path() )
{
}


File::~File()
{
}


File& File::operator=(const File& file)
{
    _path = file.path();
    return *this;
}


std::size_t File::size() const
{
    return FileImpl::size( path().c_str() );
}


void File::resize(std::size_t newSize)
{
    FileImpl::resize(path().c_str(), newSize);
}


void File::remove()
{
    FileImpl::remove( path().c_str() );
}


void File::move(const std::string& to)
{
    FileImpl::move(path().c_str(), to.c_str());
    _path = to;
}

void File::link(const std::string& to)
{
    FileImpl::link(path().c_str(), to.c_str());
}

void File::symlink(const std::string& to)
{
    FileImpl::symlink(path().c_str(), to.c_str());
}

// TODO This should be done on a file system basis. If we'd have a relative file here,
// with no path, and try to determine the parent, an empty string would be returned,
// though a parent is available.
// TODO This is identical to Directory::parentPath(). Maybe this should be moved into
// the common base class FileSystemNode.
std::string File::dirName() const
{
    // Find last slash. This separates the file name from the path.
    std::string::size_type separatorPos = path().find_last_of( Directory::sep() );

    // If there is no separator, the file is relative to the current directory. So an empty path is returned.
    if (separatorPos == std::string::npos)
    {
        return "";
    }

    // Include trailing separator to be able to distinguish between no path ("") and a path
    // which is relative to the root ("/"), for example.
    return path().substr(0, separatorPos + 1);
}


// TODO This is identical to Directory::name(). Maybe this should be moved into
// the common base class FileSystemNode.
std::string File::name() const
{
    std::string::size_type separatorPos = path().rfind( Directory::sep() );

    if (separatorPos != std::string::npos)
    {
        return path().substr(separatorPos + 1);
    }
    else
    {
        return path();
    }
}


std::string File::baseName() const
{
    std::string fileName = this->name();

    std::string::size_type extensionPointPos = fileName.rfind('.');

    if (extensionPointPos != std::string::npos)
    {
        return fileName.substr(0, extensionPointPos);
    }
    else
    {
        return fileName;
    }
}


std::string File::extension() const
{
    std::string fileName = this->name();

    std::string::size_type extensionPointPos = fileName.rfind('.');

    if (extensionPointPos != std::string::npos)
    {
        return fileName.substr(extensionPointPos + 1);
    }
    else
    {
        return "";
    }
}


File File::create(const std::string& path)
{
    FileImpl::create( path.c_str() );
    return File(path);
}


bool File::exists(const std::string& path)
{
    return FileInfo::getType( path.c_str() ) == FileInfo::File;
}

} // namespace cxxtools
