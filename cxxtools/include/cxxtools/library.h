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
#ifndef CXXTOOLS_LIBRARY_H
#define CXXTOOLS_LIBRARY_H

#include <string>

namespace cxxtools {

class Symbol;

/** @brief Shared library loader

    This class can be used to dynamically load shared libraries and
    resolve symbols from it. The example below shows how to retrieve
    the address of the function 'myProcedure' in library 'MyLibrary':

    @code
        Library shlib("MyLibrary.dll");
        void* procAddr = shlib["myProcedure"];

        typedef int (*MyProcType)();
        MyProcType proc =(MyProcType)procAddr;
        int result = proc();
    @endcode
*/
class Library
{
    public:
        /** @brief Default Constructor which does not load a library.
         */
        Library();

        /** @brief Loads a shared library.

             If a file could not be found at the given path, the path will be extended
             by the platform-specific shared library extension first and then also by the
             shared library prefix. If still no file can be found an exception is thrown.

             The library is loaded immediately.
        */
        explicit Library(const std::string& path);

        Library(const Library& other);

        Library& operator=(const Library& other);

        /** @brief The destructor unloads the shared library from memory.
         */
        ~Library();

        /** @brief Loads a shared library.

             If a file could not be found at the given path, the path will be extended
             by the platform-specific shared library extension first and then also by the
             shared library prefix. If still no file can be found an exception is thrown.
             Calling this method twice might close the previously loaded library.
        */
        Library& open(const std::string& path);

        void close();

        /** @brief Resolves the symbol \a symbol from the shared library
            Returns the address of the symbol or 0 if it was not found.
         */
        void* operator[](const char* symbol) const
        { return this->resolve(symbol); }

        /** @brief Resolves the symbol \a symbol from the shared library
            Returns the address of the symbol or 0 if it was not found.
         */
        void* resolve(const char* symbol) const;

        /** @brief Returns null if invalid
         */
        operator const void*() const;

        Symbol getSymbol(const char* symbol) const;

        /** @brief Returns true if invalid
         */
        bool operator!() const;

        /** @brief Returns the path to the shared library image
         */
        const std::string& path() const;

        /** @brief Returns the extension for shared libraries

            Returns ".so" on Linux, ".dll" on Windows.
        */
        static std::string suffix();

        /**  @brief Returns the prefix for shared libraries

             Returns "lib" on Linux, "" on Windows 
        */
        static std::string prefix();

    protected:
        void detach();

    private:
        //! @internal
        class LibraryImpl* _impl;

        //! @internal
        std::string _path;
};

/** @brief Symbol resolved from a shared library
*/
class Symbol
{
    public:
        Symbol()
        : _sym(0)
        { }

        Symbol(const Library& lib, void* sym)
        : _lib(lib), _sym(sym)
        { }

        void* sym() const
        { return _sym; }

        const Library& library() const
        { return _lib; }

        operator void*() const
        { return _sym; }

    private:
        Library _lib;
        void* _sym;
};

} // namespace cxxtools

#endif
