#ifndef VDR_STREAMDEV_SERVERS_MENUHTTP_H
#define VDR_STREAMDEV_SERVERS_MENUHTTP_H

#include <string>
#include "../common.h"
#include <vdr/recording.h>

class cChannel;

// ******************** cItemIterator ******************
class cItemIterator
{
	public:
		virtual bool Next() = 0;
		virtual bool IsGroup() const = 0;
		virtual const cString ItemId() const = 0;
		virtual const char* ItemTitle() const = 0;
		virtual const cString ItemRessource() const = 0;
		virtual const char* Alang(int i) const = 0;
		virtual const char* Dlang(int i) const = 0;
		virtual ~cItemIterator() {};
};

class cRecordingsIterator: public cItemIterator
{
	private:
		eStreamType streamType;
		const cRecording *first;
		const cRecording *current;
		cThreadLock RecordingsLock;
	protected:
		virtual const cRecording* NextSuitable(const cRecording *Recording);
	public:
		virtual bool Next();
		virtual bool IsGroup() const { return false; }
		virtual const cString ItemId() const { return current ? itoa(current->Index() + 1) : "0"; }
		virtual const char* ItemTitle() const { return current ? current->Title() : ""; }
		virtual const cString ItemRessource() const;
		virtual const char* Alang(int i) const { return NULL; }
		virtual const char* Dlang(int i) const { return NULL; }
		cRecordingsIterator(eStreamType StreamType);
		virtual ~cRecordingsIterator() {};
};

class cChannelIterator: public cItemIterator
{
	private:
		const cChannel *first;
		const cChannel *current;
	protected:
		virtual const cChannel* FirstChannel();
		virtual const cChannel* NextNormal();
		virtual const cChannel* NextGroup();
		virtual const cChannel* NextChannel(const cChannel *Channel) = 0;
		static inline const cChannel* SkipFakeGroups(const cChannel *Channel);
		// Helper which returns the group by its index
		static const cChannel* GetGroup(const char* GroupId);
	public:
		virtual bool Next();
		virtual bool IsGroup() const { return current && current->GroupSep(); }
		virtual const cString ItemId() const;
		virtual const char* ItemTitle() const { return current ? current->Name() : ""; }
		virtual const cString ItemRessource() const { return (current ? current->GetChannelID() : tChannelID::InvalidID).ToString(); }
		virtual const char* Alang(int i) const { return current && current->Apid(i) ? current->Alang(i) : NULL; }
		virtual const char* Dlang(int i) const { return current && current->Dpid(i) ? current->Dlang(i) : NULL; }
		cChannelIterator(const cChannel *First);
		virtual ~cChannelIterator() {};
};

class cListAll: public cChannelIterator
{
	protected:
		virtual const cChannel* NextChannel(const cChannel *Channel);
	public:
		cListAll();
		virtual ~cListAll() {};
};

class cListChannels: public cChannelIterator
{
	protected:
		virtual const cChannel* NextChannel(const cChannel *Channel);
	public:
		cListChannels();
		virtual ~cListChannels() {};
};

class cListGroups: public cChannelIterator
{
	protected:
		virtual const cChannel* NextChannel(const cChannel *Channel);
	public:
		cListGroups();
		virtual ~cListGroups() {};
};

class cListGroup: public cChannelIterator
{
	private:
		static const cChannel* GetNextChannelInGroup(const cChannel *Channel);
	protected:
		virtual const cChannel* NextChannel(const cChannel *Channel);
	public:
		cListGroup(const char *GroupId);
		virtual ~cListGroup() {};
};

class cListTree: public cChannelIterator
{
	private:
		const cChannel* selectedGroup;
		const cChannel* currentGroup;
	protected:
		virtual const cChannel* NextChannel(const cChannel *Channel);
	public:
		cListTree(const char *SelectedGroupId);
		virtual ~cListTree() {};
};

// ******************** cMenuList ******************
class cMenuList
{
	private:
		cItemIterator *iterator;
	protected:
		bool NextItem() { return iterator->Next(); }
		bool IsGroup() { return iterator->IsGroup(); }
		const cString ItemId() { return iterator->ItemId(); }
		const char* ItemTitle() { return iterator->ItemTitle(); }
		const cString ItemRessource() { return iterator->ItemRessource(); }
		const char* Alang(int i) { return iterator->Alang(i); }
		const char* Dlang(int i) { return iterator->Dlang(i); }
	public:
		virtual std::string HttpHeader() { return "HTTP/1.0 200 OK\r\n"; };
		virtual bool HasNext() = 0;
		virtual std::string Next() = 0;
		cMenuList(cItemIterator *Iterator);
		virtual ~cMenuList();
};

class cHtmlMenuList: public cMenuList
{
	private:
		static const char* menu;
		static const char* css;
		static const char* js;

		enum eHtmlState {
			hsRoot, hsHtmlHead, hsCss, hsJs, hsPageTop, hsPageBottom,
			hsGroupTop, hsGroupBottom,
			hsPlainTop, hsPlainItem, hsPlainBottom,
			hsItemsTop, hsItem, hsItemsBottom 
		};
		eHtmlState htmlState;
		bool onItem;
		eStreamType streamType;
		const char* self;
		const char* rss;
		const char* groupTarget;

		std::string StreamTypeMenu();
		std::string HtmlHead();
		std::string PageTop();
		std::string GroupTitle();
		std::string ItemText();
		std::string PageBottom();
	public:
		virtual std::string HttpHeader() {
			return cMenuList::HttpHeader()
				+ "Content-type: text/html; charset="
				+ (cCharSetConv::SystemCharacterTable() ? cCharSetConv::SystemCharacterTable() : "UTF-8")
				+ "\r\n";
		}
		virtual bool HasNext();
		virtual std::string Next();
		cHtmlMenuList(cItemIterator *Iterator, eStreamType StreamType, const char *Self, const char *Rss, const char *GroupTarget);
		virtual ~cHtmlMenuList();
};

class cM3uMenuList: public cMenuList
{
	private:
		char *base;
		enum eM3uState { msFirst, msContinue, msLast };
		eM3uState m3uState;
		cCharSetConv m_IConv;
	public:
		virtual std::string HttpHeader() { return cMenuList::HttpHeader() + "Content-type: audio/x-mpegurl; charset=UTF-8\r\n"; };
		virtual bool HasNext();
		virtual std::string Next();
		cM3uMenuList(cItemIterator *Iterator, const char* Base);
		virtual ~cM3uMenuList();
};

class cRssMenuList: public cMenuList
{
	private:
		char *base;
		char *html;
		enum eRssState { msFirst, msContinue, msLast };
		eRssState rssState;
		cCharSetConv m_IConv;
	public:
		virtual std::string HttpHeader() { return cMenuList::HttpHeader() + "Content-type: application/rss+xml\r\n"; };
		virtual bool HasNext();
		virtual std::string Next();
		
		cRssMenuList(cItemIterator *Iterator, const char *Base, const char *Html);
		virtual ~cRssMenuList();
};

inline const cChannel* cChannelIterator::SkipFakeGroups(const cChannel* Group)
{
#if APIVERSNUM >= 20300
	LOCK_CHANNELS_READ;
#endif
	while (Group && Group->GroupSep() && !*Group->Name())
#if APIVERSNUM >= 20300
		Group = Channels->Next(Group);
#else
		Group = Channels.Next(Group);
#endif
	return Group;
}

#endif
