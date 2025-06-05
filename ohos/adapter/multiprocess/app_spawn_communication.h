/*
 * Copyright (c) 2023-2025 Haitai FangYuan Co., Ltd.
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 *    conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 *    of conditions and the following disclaimer in the documentation and/or other materials
 *    provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 *    to endorse or promote products derived from this software without specific prior written
 *    permission.
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

#include "ohos/adapter/multiprocess/scoped_socket.h"

#include <memory>
#include <string>
#include <vector>

#include "ohos/adapter/export.h"

namespace ohos::adapter::multiprocess {

class ADAPTER_EXPORT_API AppSpawnCommunication {
 public:
  using FdRemapVector = std::vector<std::pair<int, int>>;
  // RequestCommandline is used in the child process
  // commands
  static bool RequestCommandline(std::vector<std::string>& commands);
  static FdRemapVector GetFdIdsRemap();
  static void SetFdIdsRemap(FdRemapVector& fds);
  // 1. Build commands response
  // 2. Start socket listen
  bool PrepareHandleRequest(const std::vector<std::string>& commands,
                            const FdRemapVector& fds);

  // Accept Request in blocking mode;
  // when recvice request, commands will pass to child process
  bool HandleRequestAfterChildProcStart(pid_t child_pid);
  ~AppSpawnCommunication();

 private:
  static std::string BuildCommands(
      const std::vector<std::string>& commands,
      const std::vector<std::string>& extra_commands = {});
  static std::vector<std::string> Split(const std::string& commands,
                                        const std::string& separator);
  static std::vector<int> GetRemapFd(const std::vector<std::string>& commands);

  bool PrepareResponseData(const std::vector<std::string>& commands,
                           const FdRemapVector& fds);
  bool PrepareCommunication();

  int GetCmdLinesSize() const { return this->commandslines_.size(); }
  const void* GetCmdLinesData() const { return this->commandslines_.data(); }

  ScopedSocket GetClientSocket() {
    return ScopedSocket(this->server_socket_->Accept());
  }

  std::string commandslines_;
  std::vector<int> share_fds_;
  std::unique_ptr<ScopedSocket> server_socket_;
};
}  // namespace ohos::adapter::multiprocess
#endif  // OHOS_ADAPTER_MULTIPROCESS_APP_SPAWN_COMMUNICATION_H_
