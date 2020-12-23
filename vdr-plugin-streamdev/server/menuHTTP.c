#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <vdr/channels.h>
#include "server/menuHTTP.h"

//**************************** cRecordingIterator **************
#if APIVERSNUM >= 20300
cRecordingsIterator::cRecordingsIterator(eStreamType StreamType)
#else
cRecordingsIterator::cRecordingsIterator(eStreamType StreamType): RecordingsLock(&Recordings)
#endif
{	
	streamType = StreamType;
#if APIVERSNUM >= 20300
	LOCK_RECORDINGS_READ;
	first = NextSuitable(Recordings->First());
#else
	first = NextSuitable(Recordings.First());
#endif
	current = NULL;
}

const cRecording* cRecordingsIterator::NextSuitable(const cRecording *Recording)
{
	while (Recording)
	{
		bool isPes = Recording->IsPesRecording();
		if (!isPes || (isPes && streamType == stPES))
			break;
#if APIVERSNUM >= 20300
		LOCK_RECORDINGS_READ;
		Recording = Recordings->Next(Recording);
#else
		Recording = Recordings.Next(Recording);
#endif
	}
	return Recording;
}

bool cRecordingsIterator::Next()
{
#if APIVERSNUM >= 20300
	LOCK_RECORDINGS_READ;
#endif
	if (first)
	{
		current = first;
		first = NULL;
	}
	else
#if APIVERSNUM >= 20300
		current = NextSuitable(Recordings->Next(current));
#else
		current = NextSuitable(Recordings.Next(current));
#endif
	return current;
}

const cString cRecordingsIterator::ItemRessource() const
{
	struct stat st;
	if (stat(current->FileName(), &st) == 0)
		return cString::sprintf("%lu:%llu.rec", (unsigned long) st.st_dev, (unsigned long long) st.st_ino);
	return "";
}

//**************************** cChannelIterator **************
cChannelIterator::cChannelIterator(const cChannel *First)
{	
	first = First;
	current = NULL;
}

bool cChannelIterator::Next()
{
	if (first)
	{
		current = first;
		first = NULL;
	}
	else
		current = NextChannel(current);
	return current;
}

const cString cChannelIterator::ItemId() const
{
	if (current)
	{
		if (current->GroupSep())
		{
			int index = 0;
#if APIVERSNUM >= 20300
			LOCK_CHANNELS_READ;
			for (int curr = Channels->GetNextGroup(-1); curr >= 0; curr = Channels->GetNextGroup(curr))
			{
				if (Channels->Get(curr) == current)
#else
			for (int curr = Channels.GetNextGroup(-1); curr >= 0; curr = Channels.GetNextGroup(curr))
			{
				if (Channels.Get(curr) == current)
#endif
					return itoa(index);
				index++;
			}
		}
		else
		{
			return itoa(current->Number());
		}
	}
	return cString("-1");
}

const cChannel* cChannelIterator::GetGroup(const char* GroupId)
{
	int group = -1;
#if APIVERSNUM >= 20300
	LOCK_CHANNELS_READ;
#endif
	if (GroupId)
	{
		int Index = atoi(GroupId);
#if APIVERSNUM >= 20300
		group = Channels->GetNextGroup(-1);
		while (Index-- && group >= 0)
			group = Channels->GetNextGroup(group);
	}
	return group >= 0 ? Channels->Get(group) : NULL;
#else
		group = Channels.GetNextGroup(-1);
		while (Index-- && group >= 0)
			group = Channels.GetNextGroup(group);
	}
	return group >= 0 ? Channels.Get(group) : NULL;
#endif
}

const cChannel* cChannelIterator::FirstChannel()
{
const cChannel *Channel;
#if APIVERSNUM >= 20300
	LOCK_CHANNELS_READ;
	Channel = Channels->First();
#else
	Channel = Channels.First();
#endif
	return Channel;
}

const cChannel* cChannelIterator::NextNormal()
{
const cChannel *Channel;
#if APIVERSNUM >= 20300
	LOCK_CHANNELS_READ;
	Channel = Channels->Get(Channels->GetNextNormal(-1));
#else
	Channel = Channels.Get(Channels.GetNextNormal(-1));
#endif
	return Channel;
}

const cChannel* cChannelIterator::NextGroup()
{
const cChannel *Channel;
#if APIVERSNUM >= 20300
	LOCK_CHANNELS_READ;
	Channel = Channels->Get(Channels->GetNextGroup(-1));
#else
	Channel = Channels.Get(Channels.GetNextGroup(-1));
#endif
	return Channel;
}

//**************************** cListAll **************
cListAll::cListAll(): cChannelIterator(FirstChannel())
{}

const cChannel* cListAll::NextChannel(const cChannel *Channel)
{
#if APIVERSNUM >= 20300
	LOCK_CHANNELS_READ;
	if (Channel)
		Channel = SkipFakeGroups(Channels->Next(Channel));
#else
	if (Channel)
		Channel = SkipFakeGroups(Channels.Next(Channel));
#endif
	return Channel;
}

//**************************** cListChannels **************
cListChannels::cListChannels(): cChannelIterator(NextNormal())
{}

const cChannel* cListChannels::NextChannel(const cChannel *Channel)
{
#if APIVERSNUM >= 20300
	LOCK_CHANNELS_READ;
	if (Channel)
		Channel = Channels->Get(Channels->GetNextNormal(Channel->Index()));
#else
	if (Channel)
		Channel = Channels.Get(Channels.GetNextNormal(Channel->Index()));
#endif
	return Channel;
}

// ********************* cListGroups ****************
cListGroups::cListGroups(): cChannelIterator(NextGroup())
{}

const cChannel* cListGroups::NextChannel(const cChannel *Channel)
{
#if APIVERSNUM >= 20300
	LOCK_CHANNELS_READ;
	if (Channel)
		Channel = Channels->Get(Channels->GetNextGroup(Channel->Index()));
#else
	if (Channel)
		Channel = Channels.Get(Channels.GetNextGroup(Channel->Index()));
#endif
	return Channel;
}
//
// ********************* cListGroup ****************
cListGroup::cListGroup(const char *GroupId): cChannelIterator(GetNextChannelInGroup(GetGroup(GroupId)))
{}

const cChannel* cListGroup::GetNextChannelInGroup(const cChannel *Channel)
{
#if APIVERSNUM >= 20300
	LOCK_CHANNELS_READ;
	if (Channel)
		Channel = SkipFakeGroups(Channels->Next(Channel));
#else
	if (Channel)
		Channel = SkipFakeGroups(Channels.Next(Channel));
#endif
	return Channel && !Channel->GroupSep() ? Channel : NULL;
}

const cChannel* cListGroup::NextChannel(const cChannel *Channel)
{
	return GetNextChannelInGroup(Channel);
}
//
// ********************* cListTree ****************
cListTree::cListTree(const char *SelectedGroupId): cChannelIterator(NextGroup())
{
	selectedGroup = GetGroup(SelectedGroupId);
#if APIVERSNUM >= 20300
	LOCK_CHANNELS_READ;
	currentGroup = Channels->Get(Channels->GetNextGroup(-1));
#else
	currentGroup = Channels.Get(Channels.GetNextGroup(-1));
#endif
}

const cChannel* cListTree::NextChannel(const cChannel *Channel)
{
	if (currentGroup == selectedGroup)
	{
#if APIVERSNUM >= 20300
		LOCK_CHANNELS_READ;
		if (Channel)
			Channel = SkipFakeGroups(Channels->Next(Channel));
#else
		if (Channel)
			Channel = SkipFakeGroups(Channels.Next(Channel));
#endif
		if (Channel && Channel->GroupSep())
			currentGroup = Channel;
	}
	else
	{
#if APIVERSNUM >= 20300
		LOCK_CHANNELS_READ;
		if (Channel)
			Channel = Channels->Get(Channels->GetNextGroup(Channel->Index()));
#else
		if (Channel)
			Channel = Channels.Get(Channels.GetNextGroup(Channel->Index()));
#endif
		currentGroup = Channel;
	}
	return Channel;
}

// ******************** cMenuList ******************
cMenuList::cMenuList(cItemIterator *Iterator) : iterator(Iterator)
{}

cMenuList::~cMenuList()
{
	delete iterator;
}

// ******************** cHtmlMenuList ******************
const char* cHtmlMenuList::menu =
	"[<a href=\"/\">Home</a> (<a href=\"all.html\" tvid=\"RED\">no script</a>)] "
	"[<a href=\"tree.html\" tvid=\"GREEN\">Tree View</a>] "
	"[<a href=\"groups.html\" tvid=\"YELLOW\">Groups</a> (<a href=\"groups.m3u\">Playlist</a> | <a href=\"groups.rss\">RSS</a>)] "
	"[<a href=\"channels.html\" tvid=\"BLUE\">Channels</a> (<a href=\"channels.m3u\">Playlist</a> | <a href=\"channels.rss\">RSS</a>)] "
	"[<a href=\"recordings.html\">Recordings</a> (<a href=\"recordings.m3u\">Playlist</a> | <a href=\"recordings.rss\">RSS</a>)] ";

const char* cHtmlMenuList::css =
	"<style type=\"text/css\">\n"
	"<!--\n"
	"a:link, a:visited, a:hover, a:active, a:focus { color:#333399; }\n"
	"body { font:100% Verdana, Arial, Helvetica, sans-serif; background-color:#9999FF; margin:1em; }\n"
	".menu { position:fixed; top:0px; left:1em; right:1em; height:3em; text-align:center; background-color:white; border:inset 2px #9999ff; }\n"
	"h2 { font-size:150%; margin:0em; padding:0em 1.5em; }\n"
	".contents { margin-top:5em; background-color:white; }\n"
	".group { background:url(data:image/gif;base64,R0lGODdhAQAeAIQeAJub/5yc/6Cf/6Oj/6am/6qq/66u/7Gx/7S0/7i4/7u8/7+//8LD/8bG/8nK/83N/9HQ/9TU/9fX/9va/97e/+Lh/+Xl/+no/+3t//Dw//Pz//b3//v7//7+/////////ywAAAAAAQAeAAAFGCAQCANRGAeSKAvTOA8USRNVWReWaRvXhQA7) repeat-x; border:inset 2px #9999ff; }\n"
	".items { border-top:dashed 1px; margin-top:0px; margin-bottom:0px; padding:0.7em 5em; }\n"
	".apid { padding-left:28px; margin:0.5em; background:url(data:image/gif;base64,R0lGODlhGwASAKEBAAAAAP///////////yH5BAEKAAEALAAAAAAbABIAAAJAjI+pywj5WgPxVAmpNRcHqnGb94FPhE7m+okts7JvusSmSys2iLv6TstldjMfhhUkcXi+zjFUVFo6TiVVij0UAAA7) no-repeat; }\n"
	".dpid { padding-left:28px; margin:0.5em; background:url(data:image/gif;base64,R0lGODlhGwASAKEBAAAAAP///////////yH5BAEKAAEALAAAAAAbABIAAAJFjI+py+0BopwAUoqivRvr83UaZ4RWMnVoBbLZaJbuqcCLGcv0+t5Vvgu2hLrh6pfDzVSpnlGEbAZhnIutZaVmH9yuV1EAADs=) no-repeat; }\n"
	"button { width:2em; margin:0.2em 0.5em; vertical-align:top; }\n"
	"-->\n"
	"</style>";

const char* cHtmlMenuList::js =
	"<script language=\"JavaScript\">\n"
	"<!--\n"

	"function eventTarget(evt) {\n"
	"  if (!evt) evt = window.event;\n"
	"  if (evt.target) return evt.target;\n"
	"  else if (evt.srcElement) return evt.srcElement;\n"
	"  else return null;\n"
	"}\n"

	// toggle visibility of a group
	"function clickHandler(evt) {\n"
	"  var button = eventTarget(evt);\n"
	"  if (button) {\n"
	"    var group = document.getElementById('c' + button.id);\n"
	"    if (group) {\n"
	"      button.removeChild(button.firstChild);\n"
	"      if (group.style.display == 'block') {\n"
	"        button.appendChild(document.createTextNode(\"+\"));\n"
	"        group.style.display = 'none';\n"
	"      } else {\n"
	"        button.appendChild(document.createTextNode(\"-\"));\n"
	"        group.style.display = 'block';\n"
	"      }\n"
	"    }\n"
	"  }\n"
	"}\n"

        // insert a click button infront of each h2 and an id to the corresponding list
	"function init() {\n"
	"  var titles = document.getElementsByTagName('h2');\n"
	"  for (var i = 0; i < titles.length; i++) {\n"
	"    var button = document.createElement('button');\n"
	"    button.id = 'g' + i;\n"
	"    button.onclick = clickHandler;\n"
	"    button.appendChild(document.createTextNode('+'));\n"
	"    titles[i].insertBefore(button, titles[i].firstChild);\n"
	"    var group = titles[i].nextSibling;\n"
	"    while (group) {\n"
	"      if (group.className && group.className == 'items') {\n"
	"        group.id = 'cg' + i;\n"
	"        break;\n"
	"      }\n"
	"    group = group.nextSibling;\n"
	"    }\n"
	"  }\n"
	"}\n"

	"window.onload = init;\n"

        // hide lists before the browser renders it
	"if (document.styleSheets[0].insertRule)\n"
	"  document.styleSheets[0].insertRule('.items { display:none }', 0);\n"
	"else if (document.styleSheets[0].addRule)\n"
	"  document.styleSheets[0].addRule('.items', 'display:none');\n"

	"//-->\n"
	"</script>";


std::string cHtmlMenuList::StreamTypeMenu()
{
	std::string typeMenu;
	typeMenu += (streamType == stTS ? (std::string) "[TS] " :
			(std::string) "[<a href=\"/TS/" + self + "\">TS</a>] ");
#ifdef STREAMDEV_PS
	typeMenu += (streamType == stPS ? (std::string) "[PS] " :
			(std::string) "[<a href=\"/PS/" + self + "\">PS</a>] ");
#endif
	typeMenu += (streamType == stPES ? (std::string) "[PES] " :
			(std::string) "[<a href=\"/PES/" + self + "\">PES</a>] ");
	typeMenu += (streamType == stES ? (std::string) "[ES] " :
			(std::string) "[<a href=\"/ES/" + self + "\">ES</a>] ");
	typeMenu += (streamType == stEXT ? (std::string) "[EXT] " :
			(std::string) "[<a href=\"/EXT/" + self + "\">EXT</a>] ");
	return typeMenu;
}

cHtmlMenuList::cHtmlMenuList(cItemIterator *Iterator, eStreamType StreamType, const char *Self, const char *Rss, const char *GroupTarget): cMenuList(Iterator)
{
	streamType = StreamType;
	self = strdup(Self);
	rss = strdup(Rss);
	groupTarget = (GroupTarget && *GroupTarget) ? strdup(GroupTarget) : NULL;
	htmlState = hsRoot;
	onItem = true;
}

cHtmlMenuList::~cHtmlMenuList()
{
	free((void *) self);
	free((void *) rss);
	free((void *) groupTarget);
}

bool cHtmlMenuList::HasNext()
{
	return htmlState != hsPageBottom;
}

std::string cHtmlMenuList::Next()
{
	switch (htmlState)
	{
		case hsRoot:
			htmlState = hsHtmlHead;
			break;
		case hsHtmlHead:
			htmlState = hsCss;
			break;
		case hsCss:
			htmlState = *self ? hsPageTop : hsJs;
			break;
		case hsJs:
			htmlState = hsPageTop;
			break;
		case hsPageTop:
			onItem = NextItem();
			htmlState = onItem ? (IsGroup() ? hsGroupTop : hsPlainTop) : hsPageBottom;
			break;
		case hsPlainTop:
			htmlState = hsPlainItem;
			break;
		case hsPlainItem:
			onItem = NextItem();
			htmlState = onItem && !IsGroup() ? hsPlainItem : hsPlainBottom;
			break;
		case hsPlainBottom:
			htmlState = onItem ? hsGroupTop : hsPageBottom;
			break;
		case hsGroupTop:
			onItem = NextItem();
			htmlState = onItem && !IsGroup() ? hsItemsTop : hsGroupBottom;
			break;
		case hsItemsTop:
			htmlState = hsItem;
			break;
		case hsItem:
			onItem = NextItem();
			htmlState = onItem && !IsGroup() ? hsItem : hsItemsBottom;
			break;
		case hsItemsBottom:
			htmlState = hsGroupBottom;
			break;
		case hsGroupBottom:
			htmlState = onItem ? hsGroupTop : hsPageBottom;
			break;
		case hsPageBottom:
		default:
			esyslog("streamdev-server cHtmlMenuList: invalid call to Next()");
			break;
	}
	switch (htmlState)
	{
		// NOTE: JavaScript requirements:
		// Group title is identified by <h2> tag
		// Channel list must be a sibling of <h2> with class "items"
		case hsHtmlHead:	return "<html><head>" + HtmlHead();
		case hsCss:		return css;
		case hsJs:		return js;
		case hsPageTop:		return "</head><body>" + PageTop() + "<div class=\"contents\">";
		case hsGroupTop:	return "<div class=\"group\"><h2>" + GroupTitle() + "</h2>";
		case hsItemsTop:
		case hsPlainTop:	return "<ol class=\"items\">";
		case hsItem:
		case hsPlainItem:	return ItemText();
		case hsItemsBottom:
		case hsPlainBottom:	return "</ol>";
		case hsGroupBottom:	return "</div>";
		case hsPageBottom:	return "</div>" + PageBottom() + "</body></html>";
		default:		return "";
	}
}

std::string cHtmlMenuList::HtmlHead()
{
	return (std::string) "<link rel=\"alternate\" type=\"application/rss+xml\" title=\"RSS\" href=\"" + rss + "\"/>";
}

std::string cHtmlMenuList::PageTop()
{
	return (std::string) "<div class=\"menu\"><div>" + menu + "</div><div>" + StreamTypeMenu() + "</div></div>";
}

std::string cHtmlMenuList::PageBottom()
{
	return (std::string) "";
}

std::string cHtmlMenuList::GroupTitle()
{
	if (groupTarget)
	{
		return (std::string) "<a href=\"" + groupTarget + "?group=" + (const char*) ItemId() + "\">" +
			ItemTitle() + "</a>";
	}
	else
	{
		return (std::string) ItemTitle();
	}
}

std::string cHtmlMenuList::ItemText()
{
	std::string line;
	std::string suffix;

	switch (streamType) {
		case stTS: suffix = (std::string) ".ts"; break;
#ifdef STREAMDEV_PS
		case stPS: suffix = (std::string) ".vob"; break;
#endif
		// for Network Media Tank
		case stPES: suffix = (std::string) ".vdr"; break; 
		default: suffix = "";
	}
	line += (std::string) "<li value=\"" + (const char*) ItemId() + "\">";
	line += (std::string) "<a href=\"" + (const char*) ItemRessource() + suffix + "\"";

	// for Network Media Tank
	line += (std::string) " vod ";
	if (strlen(ItemId()) < 4)
	    line += (std::string) " tvid=\"" + (const char*) ItemId() + "\""; 

	line += (std::string) ">" + ItemTitle() + "</a>";

	// TS always streams all PIDs
	if (streamType != stTS)
	{
		int index = 1;
		const char* lang;
		std::string pids;
		for (int i = 0; (lang = Alang(i)) != NULL; ++i, ++index) {
			pids += (std::string) " <a href=\"" + (const char*) ItemRessource() +
					"+" + (const char*)itoa(index) + suffix + "\" class=\"apid\" vod>" + (const char*) lang + "</a>";
		}
		for (int i = 0; (lang = Dlang(i)) != NULL; ++i, ++index) {
			pids += (std::string) " <a href=\"" + (const char*) ItemRessource() +
					"+" + (const char*)itoa(index) + suffix + "\" class=\"dpid\" vod>" + (const char*) lang + "</a>";
		}
		// always show audio PIDs for stES to select audio only
		if (index > 2 || streamType == stES)
			line += pids;
	}
	line += "</li>";
	return line;
}

// ******************** cM3uMenuList ******************
cM3uMenuList::cM3uMenuList(cItemIterator *Iterator, const char* Base)
: cMenuList(Iterator),
  m_IConv(cCharSetConv::SystemCharacterTable(), "UTF-8")
{
	base = strdup(Base);
	m3uState = msFirst;
}

cM3uMenuList::~cM3uMenuList()
{
	free(base);
}

bool cM3uMenuList::HasNext()
{
	return m3uState != msLast;
}

std::string cM3uMenuList::Next()
{
	if (m3uState == msFirst)
	{
		m3uState = msContinue;
		return "#EXTM3U";
	}

	if (!NextItem())
	{
		m3uState = msLast;
		return "";
	}

	std::string name = (std::string) m_IConv.Convert(ItemTitle());

	if (IsGroup())
	{
		return (std::string) "#EXTINF:-1," + name + "\r\n" +
			base + "group.m3u?group=" + (const char*) ItemId();
	}
	else
	{
		return (std::string) "#EXTINF:-1," +
			(const char*) ItemId() + " " + name + "\r\n" +
			base + (const char*) ItemRessource();
	}
}

// ******************** cRssMenuList ******************
cRssMenuList::cRssMenuList(cItemIterator *Iterator, const char *Base, const char *Html)
: cMenuList(Iterator),
  m_IConv(cCharSetConv::SystemCharacterTable(), "UTF-8")
{
	base = strdup(Base);
	html = strdup(Html);
	rssState = msFirst;
}

cRssMenuList::~cRssMenuList()
{
	free(base);
	free(html);
}

bool cRssMenuList::HasNext()
{
	return rssState != msLast;
}

std::string cRssMenuList::Next()
{
	std::string type_ext;

	if (rssState == msFirst)
	{
		rssState = msContinue;
		return (std::string) "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n<rss version=\"2.0\">\n\t<channel>\n"
				"\t\t<title>VDR</title>\n"
				"\t\t<link>" + base + html + "</link>\n"
				"\t\t<description>VDR channel list</description>\n"
				;
	}

	if (!NextItem())
	{
		rssState = msLast;
		return "\t</channel>\n</rss>\n";
	}

	std::string name = (std::string) m_IConv.Convert(ItemTitle());

	if (IsGroup())
	{
		return (std::string) "\t\t<item>\n\t\t\t<title>" +
			name + "</title>\n\t\t\t<link>" +
			base + "group.rss?group=" + (const char*) ItemId() + "</link>\n\t\t</item>\n";
	}
	else
	{
		return (std::string) "\t\t<item>\n\t\t\t<title>" +
			(const char*) ItemId() + " " + name + "</title>\n\t\t\t<link>" +
			base + (const char*) ItemRessource() + "</link>\n\t\t\t<enclosure url=\"" +
			base + (const char*) ItemRessource() + "\" type=\"video/mpeg\" />\n\t\t</item>\n";
	}
}


