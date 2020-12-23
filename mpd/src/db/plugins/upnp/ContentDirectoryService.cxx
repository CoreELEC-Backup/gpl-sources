/*
 * Copyright 2003-2020 The Music Player Daemon Project
 * http://www.musicpd.org
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include "lib/upnp/ContentDirectoryService.hxx"
#include "lib/upnp/ixmlwrap.hxx"
#include "lib/upnp/UniqueIxml.hxx"
#include "lib/upnp/Action.hxx"
#include "Directory.hxx"
#include "util/NumberParser.hxx"
#include "util/RuntimeError.hxx"
#include "util/ScopeExit.hxx"
#include "util/StringFormat.hxx"

static void
ReadResultTag(UPnPDirContent &dirbuf, IXML_Document *response)
{
	const char *p = ixmlwrap::getFirstElementValue(response, "Result");
	if (p == nullptr)
		p = "";

	dirbuf.Parse(p);
}

inline void
ContentDirectoryService::readDirSlice(UpnpClient_Handle hdl,
				      const char *objectId, unsigned offset,
				      unsigned count, UPnPDirContent &dirbuf,
				      unsigned &didreadp,
				      unsigned &totalp) const
{
	// Some devices require an empty SortCriteria, else bad params
	IXML_Document *request =
		MakeActionHelper("Browse", m_serviceType.c_str(),
				 "ObjectID", objectId,
				 "BrowseFlag", "BrowseDirectChildren",
				 "Filter", "*",
				 "SortCriteria", "",
				 "StartingIndex",
				 StringFormat<32>("%u", offset).c_str(),
				 "RequestedCount",
				 StringFormat<32>("%u", count).c_str());
	if (request == nullptr)
		throw std::runtime_error("UpnpMakeAction() failed");

	AtScopeExit(request) { ixmlDocument_free(request); };

	IXML_Document *response;
	int code = UpnpSendAction(hdl, m_actionURL.c_str(), m_serviceType.c_str(),
				  nullptr /*devUDN*/, request, &response);
	if (code != UPNP_E_SUCCESS)
		throw FormatRuntimeError("UpnpSendAction() failed: %s",
					 UpnpGetErrorMessage(code));

	AtScopeExit(response) { ixmlDocument_free(response); };

	const char *value = ixmlwrap::getFirstElementValue(response, "NumberReturned");
	didreadp = value != nullptr
		? ParseUnsigned(value)
		: 0;

	value = ixmlwrap::getFirstElementValue(response, "TotalMatches");
	if (value != nullptr)
		totalp = ParseUnsigned(value);

	ReadResultTag(dirbuf, response);
}

UPnPDirContent
ContentDirectoryService::readDir(UpnpClient_Handle handle,
				 const char *objectId) const
{
	UPnPDirContent dirbuf;
	unsigned offset = 0, total = -1, count;

	do {
		readDirSlice(handle, objectId, offset, m_rdreqcnt, dirbuf,
			     count, total);

		offset += count;
	} while (count > 0 && offset < total);

	return dirbuf;
}

UPnPDirContent
ContentDirectoryService::search(UpnpClient_Handle hdl,
				const char *objectId,
				const char *ss) const
{
	UPnPDirContent dirbuf;
	unsigned offset = 0, total = -1, count;

	do {
		UniqueIxmlDocument request(MakeActionHelper("Search", m_serviceType.c_str(),
							    "ContainerID", objectId,
							    "SearchCriteria", ss,
							    "Filter", "*",
							    "SortCriteria", "",
							    "StartingIndex",
							    StringFormat<32>("%u", offset).c_str(),
							    "RequestedCount", "0")); // Setting a value here gets twonky into fits
		if (!request)
			throw std::runtime_error("UpnpMakeAction() failed");

		IXML_Document *_response;
		auto code = UpnpSendAction(hdl, m_actionURL.c_str(),
					   m_serviceType.c_str(),
					   nullptr /*devUDN*/,
					   request.get(), &_response);
		if (code != UPNP_E_SUCCESS)
			throw FormatRuntimeError("UpnpSendAction() failed: %s",
						 UpnpGetErrorMessage(code));

		UniqueIxmlDocument response(_response);

		const char *value =
			ixmlwrap::getFirstElementValue(response.get(),
						       "NumberReturned");
		count = value != nullptr
			? ParseUnsigned(value)
			: 0;

		offset += count;

		value = ixmlwrap::getFirstElementValue(response.get(),
						       "TotalMatches");
		if (value != nullptr)
			total = ParseUnsigned(value);

		ReadResultTag(dirbuf, response.get());
	} while (count > 0 && offset < total);

	return dirbuf;
}

UPnPDirContent
ContentDirectoryService::getMetadata(UpnpClient_Handle hdl,
				     const char *objectId) const
{
	// Create request
	UniqueIxmlDocument request(MakeActionHelper("Browse", m_serviceType.c_str(),
						    "ObjectID", objectId,
						    "BrowseFlag", "BrowseMetadata",
						    "Filter", "*",
						    "SortCriteria", "",
						    "StartingIndex", "0",
						    "RequestedCount", "1"));
	if (request == nullptr)
		throw std::runtime_error("UpnpMakeAction() failed");

	IXML_Document *_response;
	auto code = UpnpSendAction(hdl, m_actionURL.c_str(),
				   m_serviceType.c_str(),
				   nullptr /*devUDN*/, request.get(), &_response);
	if (code != UPNP_E_SUCCESS)
		throw FormatRuntimeError("UpnpSendAction() failed: %s",
					 UpnpGetErrorMessage(code));

	UniqueIxmlDocument response(_response);
	UPnPDirContent dirbuf;
	ReadResultTag(dirbuf, response.get());
	return dirbuf;
}
