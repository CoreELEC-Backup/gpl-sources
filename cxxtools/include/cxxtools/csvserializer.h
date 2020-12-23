/*
 * Copyright (C) 2010 Tommi Maekitalo
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

#ifndef CXXTOOLS_CSVSERIALIZER_H
#define CXXTOOLS_CSVSERIALIZER_H

#include <cxxtools/csvformatter.h>
#include <cxxtools/decomposer.h>

namespace cxxtools
{
    class CsvSerializer
    {
            CsvSerializer(const CsvSerializer&);
            CsvSerializer& operator= (const CsvSerializer&);

        public:
            CsvSerializer(std::ostream& os, TextCodec<Char, char>* codec = new Utf8Codec())
                : _formatter(new CsvFormatter(os, codec))
            { }

            CsvSerializer(TextOStream& os)
                : _formatter(new CsvFormatter(os))
            { }

            ~CsvSerializer()
            {
                delete _formatter;
            }

            void selectColumn(const std::string& title)
            {
                _formatter->selectColumn(title);
            }

            void selectColumn(const std::string& memberName, const std::string& title)
            {
                _formatter->selectColumn(memberName, title);
            }

            void delimiter(Char delimiter)
            {
                _formatter->delimiter(delimiter);
            }

            void quote(Char quote)
            {
                _formatter->quote(quote);
            }

            void lineEnding(const String& le)
            {
                _formatter->lineEnding(le);
            }

            template <typename T>
            void serialize(const T& type)
            {
                Decomposer<T> decomposer;
                decomposer.begin(type);
                decomposer.format(*_formatter);
                _formatter->finish();
            }

        private:
            CsvFormatter* _formatter;
    };
}

#endif // CXXTOOLS_CSVSERIALIZER_H
