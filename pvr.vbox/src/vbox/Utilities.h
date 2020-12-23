/*
 *  Copyright (C) 2015-2020 Team Kodi
 *  Copyright (C) 2015 Sam Stenvall
 *  https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#pragma once

#include <algorithm>
#include <memory>

#include <kodi/Filesystem.h>

namespace utilities
{

  /**
   * Compares two containers for equality based on the equality of their
   * dereferenced contents (i.e. the containers should contain some kind of
   * pointers).
   */
  template<class Container>
  bool deref_equals(const Container& left, const Container& right)
  {
    return !(left.size() != right.size() ||
             !std::equal(left.begin(), left.end(), right.begin(),
                         [](const typename Container::value_type& leftItem,
                            const typename Container::value_type& rightItem)
                          {
                            return *leftItem == *rightItem;
                          }));
  }

  /**
   * Reads the contents of the file pointed to by the handle and returns it.
   * The file handle must be opened before calling this method.
   * @param fileHandle the file handle
   * @return the contents (unique pointer)
   */
  inline std::unique_ptr<std::string> ReadFileContents(kodi::vfs::CFile& fileHandle)
  {
    std::unique_ptr<std::string> content(new std::string());

    char buffer[1024];
    int bytesRead = 0;

    // Read until EOF or explicit error
    while ((bytesRead = fileHandle.Read(buffer, sizeof(buffer) - 1)) > 0)
      content->append(buffer, bytesRead);

    return content;
  }
} // namespace utilities
