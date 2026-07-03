/*
 * Copyright (c) 2023-2026 Huawei Device Co., Ltd.
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors
 * may be used to endorse or promote products derived from this software without
 * specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "unix_domain_socket.h"

#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#include "ohos/adapter/common/logging.h"
#include "ohos/adapter/multiprocess/scoped_socket.h"

namespace ohos::adapter::multiprocess {

namespace {
void DeleteSocketFile(const std::string& path) {
  unlink(path.c_str());
}
}  // namespace

UnixDomainSocket::UnixDomainSocket() : fd_(-1) {}

UnixDomainSocket::UnixDomainSocket(int fd) : fd_(fd) {}

UnixDomainSocket::~UnixDomainSocket() {
  Close();
}

UnixDomainSocket::UnixDomainSocket(UnixDomainSocket&& other) noexcept
    : fd_(other.fd_) {
  other.fd_ = -1;
}

UnixDomainSocket& UnixDomainSocket::operator=(
    UnixDomainSocket&& other) noexcept {
  if (this != &other) {
    Close();
    fd_ = other.fd_;
    other.fd_ = -1;
  }
  return *this;
}

UnixDomainSocket UnixDomainSocket::CreateServer(const std::string& path) {
  DeleteSocketFile(path);

  int server_socket = socket(AF_UNIX, SOCK_STREAM, 0);
  if (server_socket == -1) {
    LOGE(
        "UnixDomainSocket::CreateServer socket error, errno=%{public}d, "
        "Reason: %{public}s",
        errno, strerror(errno));
    return UnixDomainSocket(-1);
  }

  sockaddr_un addr{};
  addr.sun_family = AF_UNIX;
  strncpy(addr.sun_path, path.c_str(), sizeof(addr.sun_path) - 1);

  if (bind(server_socket, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) ==
      -1) {
    LOGE(
        "UnixDomainSocket::CreateServer bind error, errno=%{public}d, Reason: "
        "%{public}s",
        errno, strerror(errno));
    close(server_socket);
    return UnixDomainSocket(-1);
  }

  if (listen(server_socket, 1) == -1) {
    LOGE(
        "UnixDomainSocket::CreateServer listen error, errno=%{public}d, "
        "Reason: %{public}s",
        errno, strerror(errno));
    close(server_socket);
    return UnixDomainSocket(-1);
  }

  return UnixDomainSocket(server_socket);
}

UnixDomainSocket UnixDomainSocket::CreateClient(const std::string& path) {
  int remote_fd = socket(AF_UNIX, SOCK_STREAM, 0);
  if (remote_fd == -1) {
    LOGE(
        "UnixDomainSocket::CreateClient socket error, errno=%{public}d, "
        "Reason: %{public}s",
        errno, strerror(errno));
    return UnixDomainSocket(-1);
  }

  sockaddr_un addr{};
  addr.sun_family = AF_UNIX;
  strncpy(addr.sun_path, path.c_str(), sizeof(addr.sun_path) - 1);
  unsigned length = strlen(addr.sun_path) + sizeof(addr.sun_family);

  int ret =
      connect(remote_fd, reinterpret_cast<const sockaddr*>(&addr), length);
  if (ret == -1) {
    LOGE(
        "UnixDomainSocket::CreateClient connect failed, errno=%{public}d, "
        "Reason: %{public}s",
        errno, strerror(errno));
    close(remote_fd);
    return UnixDomainSocket(-1);
  }

  return UnixDomainSocket(remote_fd);
}

void UnixDomainSocket::Close() {
  if (fd_ >= 0) {
    close(fd_);
    fd_ = -1;
  }
}

int UnixDomainSocket::Accept() {
  return accept(fd_, nullptr, nullptr);
}

bool UnixDomainSocket::SetPassCredentials(bool enable) {
  if (fd_ < 0) {
    return false;
  }
  int val = enable ? 1 : 0;
  return setsockopt(fd_, SOL_SOCKET, SO_PASSCRED, &val, sizeof(val)) == 0;
}

}  // namespace ohos::adapter::multiprocess
