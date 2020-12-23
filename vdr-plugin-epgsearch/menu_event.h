/*                                                                  -*- c++ -*-
Copyright (C) 2004-2013 Christian Wieninger

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
Or, point your browser to http://www.gnu.org/licenses/old-licenses/gpl-2.0.html

The author can be reached at cwieninger@gmx.de

The project's page is at http://winni.vdr-developer.org/epgsearch
*/

#ifndef __EPGSEARCH_MENUEVENT_H
#define __EPGSEARCH_MENUEVENT_H

#include <list>

typedef enum {
    SurfModeUnknown,
    SurfModeTime,
    SurfModeChannel
} MenuEventSurfMode;

class cEventObj
{
    const cEvent *event;
    bool selected;
public:
    cEventObj(const cEvent* Event, bool Selected = false) : event(Event), selected(Selected) {}
    const cEvent* Event() {
        return event;
    }
    bool Selected() {
        return selected;
    }
    void Select(bool Selected) {
        selected = Selected;
    }
};

class cEventObjects
{
public:
    typedef std::list< cEventObj* > EventObjectList;
    typedef EventObjectList::size_type size_type;
    typedef EventObjectList::iterator iterator;
    typedef EventObjectList::const_iterator const_iterator;

    cEventObjects() {}
    ~cEventObjects() {
        for (EventObjectList::iterator i = m_list.begin(); i != m_list.end(); ++i)
            delete(*i);
    }

    size_type size() const {
        return m_list.size();
    }

    iterator begin() {
        return m_list.begin();
    }
    const_iterator begin() const {
        return m_list.begin();
    }
    iterator end() {
        return m_list.end();
    }
    const_iterator end() const {
        return m_list.end();
    }

    void Add(const cEvent* Event) {
        m_list.push_back(new cEventObj(Event));
    }
    void Clear() {
        m_list.clear();
    }
    void SetCurrent(const cEvent* Event) {
        for (EventObjectList::iterator i = m_list.begin(); i != m_list.end(); ++i)
            (*i)->Select((*i)->Event() == Event);
    }
    cEventObj* GetCurrent() {
        for (EventObjectList::iterator i = m_list.begin(); i != m_list.end(); ++i)
            if ((*i)->Selected())
                return (*i);
        return NULL;
    }

private:
    EventObjectList m_list;
};

class cMenuEventSearch : public cOsdMenu
{
protected:
    const cEvent *event;
    char* szGreen;
    char* szYellow;
    cEventObjects& eventObjects;
    MenuEventSurfMode surfMode;

    virtual void Set();
    cEventObj* GetPrev(const cEvent* Event);
    cEventObj* GetNext(const cEvent* Event);

    virtual void Display(void);
    virtual eOSState ProcessKey(eKeys Key);
    eOSState Commands(eKeys Key);
public:
    cMenuEventSearch(const cEvent* Event, cEventObjects& EventObjects, MenuEventSurfMode SurfMode = SurfModeUnknown);
    ~cMenuEventSearch();
#ifdef USE_GRAPHTFT
    const char* MenuKind() {
        return "MenuEvent";
    }
#endif
};

class cMenuEventSearchSimple : public cMenuEventSearch
{
private:
    virtual void Set();
public:
    cMenuEventSearchSimple(const cEvent* Event, cEventObjects& EventObjects);
};

#endif
