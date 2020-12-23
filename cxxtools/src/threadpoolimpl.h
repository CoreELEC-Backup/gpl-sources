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

#ifndef CXXTOOLS_THREADPOOLIMPL_H
#define CXXTOOLS_THREADPOOLIMPL_H

#include <cxxtools/queue.h>
#include <cxxtools/thread.h>
#include <vector>

namespace cxxtools
{
    class ThreadPoolImpl
    {
        public:
            explicit ThreadPoolImpl(unsigned size)
                : _state(Stopped),
                  _size(size)
                  { }

            ~ThreadPoolImpl();

            void start();

            void stop(bool cancel);

            void schedule(const Callable<void>& cb);

            bool running() const
            { return _state == Running; }

            bool stopped() const
            { return _state == Stopped; }

        private:
            void threadFunc();

            enum {
                Stopped,
                Starting,
                Running,
                Stopping
            } _state;

            Queue<Callable<void>*> _queue;
            typedef std::vector<AttachedThread*> ThreadsType;
            ThreadsType _threads;
            unsigned _size;
    };
}

#endif // CXXTOOLS_THREADPOOLIMPL_H
