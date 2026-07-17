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

#ifndef OHOS_ADAPTER_MULTIPROCESS_APP_SPAWN_COMMUNICATION_H_
#define OHOS_ADAPTER_MULTIPROCESS_APP_SPAWN_COMMUNICATION_H_

#include <fcntl.h>
#include <sys/file.h>
#include <unistd.h>

#include <memory>
#include <string>

#include "fd_remap_types.h"
#include "ohos/adapter/export.h"
#include "ohos/adapter/multiprocess/scoped_socket.h"
#include "ohos/adapter/multiprocess/unix_domain_socket.h"

namespace ohos::adapter::multiprocess {
class FileLocker {
 public:
    explicit FileLocker(const std::string& path);
    ~FileLocker();

    FileLocker(FileLocker&& other) noexcept;
    FileLocker& operator=(FileLocker&& other) noexcept;

    bool IsLocked() const { return fd_ >= 0; }

 private:
    int fd_ = -1;
};

class ADAPTER_EXPORT_API AppSpawnCommunication {
 public:
  static bool RequestCommandline(std::vector<std::string>& commands,
                                 multiprocess::FdRemapVector& fd_mappings);
  bool PrepareHandleRequest(const std::vector<std::string>& commands,
                            const FdRemapVector& fd_mappings);

  bool HandleRequestAfterChildProcStart(pid_t child_pid);
  AppSpawnCommunication();
  ~AppSpawnCommunication();

 private:
  static std::vector<int> GetRemapFd(const std::vector<std::string>& commands);

  bool PrepareResponseData(const std::vector<std::string>& commands,
                           const FdRemapVector& fd_mappings);
  bool PrepareCommunication();

  int GetCmdLinesSize() const { return this->commandslines_.size(); }
  const void* GetCmdLinesData() const { return this->commandslines_.data(); }

  ScopedSocket GetClientSocket() {
    return ScopedSocket(this->server_socket_.Accept());
  }

  std::string commandslines_;
  std::vector<int> share_fds_;
  UnixDomainSocket server_socket_;
  // Protect unix domain socket file from accessed by different processes
  // concurrently.
  FileLocker file_locker_;
};
}  // namespace ohos::adapter::multiprocess
#endif  // OHOS_ADAPTER_MULTIPROCESS_APP_SPAWN_COMMUNICATION_H_
