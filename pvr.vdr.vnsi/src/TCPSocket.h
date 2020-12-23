/*
 *  Copyright (C) 2005-2020 Team Kodi
 *  https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#pragma once

#include "kissnet/kissnet.hpp"

#include <memory>

namespace vdrvnsi
{

enum class SocketError
{
  None,
  Errored,
  ReadFailure,
  TimeOut
};

class TCPSocket
{
public:
  TCPSocket() = delete;
  TCPSocket(const std::string& host, uint16_t port);

  virtual ~TCPSocket();

  bool Open(uint64_t iTimeoutMs = 0);

  bool IsOpen() const { return m_socket != nullptr; }

  SocketError LastError() const { return m_lastSocketError; }

  void Shutdown();

  void Close();

  int64_t Read(void* data, size_t len, uint64_t iTimeoutMs = 0);

  int64_t Write(void* data, size_t len);

private:
  SocketError m_lastSocketError = SocketError::None;
  const kissnet::endpoint m_endpoint;
  std::unique_ptr<kissnet::tcp_socket> m_socket = nullptr;
};

} // namespace vdrvnsi
