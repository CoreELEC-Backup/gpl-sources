/*
 * Copyright 2008-2019 Max Kellermann <max.kellermann@gmail.com>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * - Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *
 * - Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the
 * distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE
 * FOUNDATION OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "UriRelative.hxx"
#include "UriExtract.hxx"
#include "StringAPI.hxx"
#include "StringCompare.hxx"

#include <cassert>

#include <string.h>

bool
uri_is_child(const char *parent, const char *child) noexcept
{
#if !CLANG_CHECK_VERSION(3,6)
	/* disabled on clang due to -Wtautological-pointer-compare */
	assert(parent != nullptr);
	assert(child != nullptr);
#endif

	const char *suffix = StringAfterPrefix(child, parent);
	return suffix != nullptr && *suffix != 0 &&
		(suffix == child || suffix[-1] == '/' || *suffix == '/');
}


bool
uri_is_child_or_same(const char *parent, const char *child) noexcept
{
	return StringIsEqual(parent, child) || uri_is_child(parent, child);
}

std::string
uri_apply_base(const std::string &uri, const std::string &base) noexcept
{
	if (uri.front() == '/') {
		/* absolute path: replace the whole URI path in base */

		auto i = base.find("://");
		if (i == base.npos)
			/* no scheme: override base completely */
			return uri;

		/* find the first slash after the host part */
		i = base.find('/', i + 3);
		if (i == base.npos)
			/* there's no URI path - simply append uri */
			i = base.length();

		return base.substr(0, i) + uri;
	}

	std::string out(base);
	if (out.back() != '/')
		out.push_back('/');

	out += uri;
	return out;
}

static void
ClearFilename(StringView &path) noexcept
{
	const char *slash = path.FindLast('/');
	if (slash != nullptr)
		path.SetEnd(slash + 1);
	else
		path.size = 0;
}

static bool
ConsumeLastSegment(StringView &path) noexcept
{
	assert(!path.empty());
	assert(path.back() == '/');

	path.pop_back();
	const char *slash = path.FindLast('/');
	if (slash == nullptr)
		return false;

	path.SetEnd(slash + 1);
	return true;
}

static bool
ConsumeSpecial(const char *&relative_path, StringView &base_path) noexcept
{
	while (true) {
		if (const char *a = StringAfterPrefix(relative_path, "./")) {
			while (*a == '/')
				++a;
			relative_path = a;
		} else if (const char *b = StringAfterPrefix(relative_path, "../")) {
			while (*b == '/')
				++b;
			relative_path = b;

			if (!ConsumeLastSegment(base_path))
				return false;
		} else if (StringIsEqual(relative_path, ".")) {
			++relative_path;
			return true;
		} else
			return true;
	}
}

std::string
uri_apply_relative(const std::string &relative_uri,
		   const std::string &base_uri) noexcept
{
	if (relative_uri.empty())
		return base_uri;

	if (uri_has_scheme(relative_uri.c_str()))
		return relative_uri;

	const char *relative_path = relative_uri.c_str();

	// TODO: support double slash at beginning of relative_uri
	if (relative_uri.front() == '/') {
		/* absolute path: replace the whole URI path in base */

		auto i = base_uri.find("://");
		if (i == base_uri.npos)
			/* no scheme: override base completely */
			return relative_uri;

		/* find the first slash after the host part */
		i = base_uri.find('/', i + 3);
		if (i == base_uri.npos)
			/* there's no URI path - simply append uri */
			i = base_uri.length();

		return base_uri.substr(0, i) + relative_uri;
	}

	const auto _base_path = uri_get_path(base_uri);
	if (_base_path.data() == nullptr) {
		std::string result(base_uri);
		if (relative_uri.front() != '/')
			result.push_back('/');
		while (const char *a = StringAfterPrefix(relative_path, "./"))
			relative_path = a;
		if (StringStartsWith(relative_path, "../"))
			return {};
		if (!StringIsEqual(relative_path, "."))
			result += relative_uri;
		return result;
	}

	StringView base_path(_base_path);
	ClearFilename(base_path);

	if (!ConsumeSpecial(relative_path, base_path))
		return {};

	std::string result(base_uri.data(), _base_path.data());
	result.append(base_path.data, base_path.size);
	result.append(relative_path);
	return result;
}
