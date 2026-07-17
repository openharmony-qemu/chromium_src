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

#include "app_spawn_communication.h"

#include <fcntl.h>
#include <sys/file.h>
#include <unistd.h>

#include <algorithm>
#include <memory>
#include <string>
#include <utility>

#include "fd_remap_types.h"
#include "ohos/adapter/common/logging.h"
#include "ohos/adapter/multiprocess/command_line/command_line.h"
#include "ohos/adapter/multiprocess/global_file_descriptor_mappings.h"
#include "ohos/adapter/multiprocess/message_helper.h"
#include "ohos/adapter/multiprocess/scoped_socket.h"
#include "ohos/adapter/multiprocess/unix_domain_socket.h"

namespace ohos::adapter::multiprocess {
namespace {
constexpr const char* kLockFilePath =
    "/data/storage/el2/base/files/start.sock.lock";
const std::string kDomainSocketPath = "/data/storage/el2/base/files/start.sock";
constexpr size_t kLengthHeaderSize = 4;

enum class RequestCommandType {
  kRequestUnknown = -1,
  kRequestData = 1
};

bool RecvRequestWithType(int fd, pid_t pid, RequestCommandType& out_type) {
  char buff[8] = {0};
  std::vector<int> ret_fds;
  pid_t ret_pid = -1;

  if (MessageHelper::RecvMsgWithPid(fd, buff, sizeof(buff), ret_fds,
                                    &ret_pid) == -1) {
    LOGE(
        "RecvMsgWithPid failed, pid = %{public}d, ret_pid = %{public}d, "
        "sizeof(buff) = %{public}lu",
        pid, ret_pid, sizeof(buff));
    return false;
  }

  if (pid != ret_pid) {
    LOGE("pid error, pid = %{public}d, ret_pid = %{public}d", pid, ret_pid);
    return false;
  }

  int cmd_value = 0;
  memcpy(&cmd_value, buff, sizeof(cmd_value));
  if (cmd_value != static_cast<int>(RequestCommandType::kRequestData)) {
    LOGE("%{public}s Invalid command type: %{public}d", __func__, cmd_value);
    out_type = RequestCommandType::kRequestUnknown;
    return false;
  }

  out_type = static_cast<RequestCommandType>(cmd_value);
  return true;
}
} // namespace

FileLocker::FileLocker(const std::string& path) : fd_(-1) {
  fd_ = open(path.c_str(), O_RDWR | O_CREAT, 0666);
  if (fd_ >= 0) {
    if (flock(fd_, LOCK_EX) < 0) {
      close(fd_);
      fd_ = -1;
    }
  }
}

FileLocker::~FileLocker() {
  if (fd_ >= 0) {
    flock(fd_, LOCK_UN);
    close(fd_);
  }
}

FileLocker::FileLocker(FileLocker&& other) noexcept
    : fd_(other.fd_) {
    other.fd_ = -1;
}

FileLocker& FileLocker::operator=(FileLocker&& other) noexcept {
    if (this != &other) {
        if (fd_ >= 0) {
            close(fd_);
        }

        fd_ = other.fd_;
        other.fd_ = -1;
    }
    return *this;
}

bool AppSpawnCommunication::PrepareHandleRequest(
    const std::vector<std::string>& commands,
    const FdRemapVector& fd_mappings) {
  if (!PrepareResponseData(commands, fd_mappings)) {
    LOGE("%{public}s, PrepareResponseData failed", __func__);
    return false;
  }
  return PrepareCommunication();
}

bool AppSpawnCommunication::PrepareResponseData(
    const std::vector<std::string>& commands,
    const FdRemapVector& fd_mappings) {
  std::vector<int> share_fds;
  std::string shared_fd_command = CommandLine::BuildFdRemapCommand(fd_mappings);
  for (auto& [rawfd, remap_fd] : fd_mappings) {
    LOGI("%{public}s fd %{public}d => %{public}d", __func__, rawfd, remap_fd);
    share_fds.push_back(rawfd);
  }

  std::vector<std::string> extra_commands = { "--ohos-args-begin" };
  if (!shared_fd_command.empty()) {
    extra_commands.push_back(shared_fd_command);
  }
  auto commandlines = CommandLine::Join(commands);
  commandlines += CommandLine::kArgSeparator;
  commandlines += CommandLine::Join(extra_commands);
  if (commandlines.size() >
      static_cast<size_t>(std::numeric_limits<int>::max())) {
    LOGE("send data length error");
    return false;
  }
  this->share_fds_ = share_fds;
  this->commandslines_ = commandlines;
  return true;
}

bool AppSpawnCommunication::PrepareCommunication() {
  server_socket_ = UnixDomainSocket::CreateServer(kDomainSocketPath);
  if (!server_socket_.IsValid()) {
    LOGE("%{public}s, create server socket failed", __func__);
    return false;
  }
  if (!server_socket_.SetPassCredentials(true)) {
    LOGE("%{public}s SetPassCredentials failed.", __func__);
    return false;
  }
  return true;
}

bool AppSpawnCommunication::HandleRequestAfterChildProcStart(pid_t child_pid) {
  ScopedSocket cs = this->GetClientSocket();
  auto commandType = RequestCommandType::kRequestUnknown;

  if (!RecvRequestWithType(cs.GetSocket(), child_pid, commandType)) {
    LOGE("%{public}s RecvRequestWithType failed.", __func__);
    return false;
  }

  if (commandType != RequestCommandType::kRequestData) {
    LOGE("%{public}s Invalid request type: %{public}d", __func__,
         static_cast<int>(commandType));
    return false;
  }

  uint32_t data_len = static_cast<uint32_t>(this->GetCmdLinesSize());
  char length_header[kLengthHeaderSize] = {0};
  memcpy(length_header, &data_len, sizeof(data_len));

  bool send_length_result = MessageHelper::SendMsg(
      cs.GetSocket(), length_header, sizeof(length_header), {});
  if (!send_length_result) {
    LOGE("%{public}s send length header failed", __func__);
    return false;
  }

  bool send_data_result =
      MessageHelper::SendMsg(cs.GetSocket(), this->GetCmdLinesData(),
                             this->GetCmdLinesSize(), this->share_fds_);
  if (!send_data_result) {
    LOGE("%{public}s send request data failed", __func__);
  }
  return send_data_result;
}

AppSpawnCommunication::AppSpawnCommunication() : file_locker_(kLockFilePath) {}

AppSpawnCommunication::~AppSpawnCommunication() {
  server_socket_.Close();
  unlink(kDomainSocketPath.c_str());
}

bool AppSpawnCommunication::RequestCommandline(
    std::vector<std::string>& commands,
    multiprocess::FdRemapVector& fd_mappings) {
  auto remote_socket = UnixDomainSocket::CreateClient(kDomainSocketPath);
  if (!remote_socket.IsValid()) {
    LOGE("%{public}s CreateClient failed.", __func__);
    return false;
  }
  int remote_fd = remote_socket.GetFd();

  int request = static_cast<int>(RequestCommandType::kRequestData);
  if (!MessageHelper::SendMsg(remote_fd, &request, sizeof(request), {})) {
    LOGE("%{public}s SendMsg request failed.", __func__);
    return false;
  }

  char length_header[kLengthHeaderSize] = {0};
  std::vector<int> fds;
  ssize_t recv_result = MessageHelper::RecvMsg(remote_fd, length_header,
                                               sizeof(length_header), fds);
  if (recv_result < 0) {
    LOGE("%{public}s Receive length header failed.", __func__);
    return false;
  }

  uint32_t data_len = 0;
  memcpy(&data_len, length_header, sizeof(data_len));
  if (data_len > static_cast<uint32_t>(std::numeric_limits<int>::max())) {
    LOGE("%{public}s Invalid data length: %{public}u", __func__, data_len);
    return false;
  }

  std::vector<char> data_buf(data_len);
  recv_result =
      MessageHelper::RecvMsg(remote_fd, data_buf.data(), data_len, fds);
  if (recv_result < 0) {
    LOGE("%{public}s Receive data failed.", __func__);
    return false;
  }

  LOGI("%{public}s Receive commandlines length: %{public}u, data: %{public}s",
       __func__, data_len, data_buf.data());

  commands = CommandLine::Split(std::string(data_buf.data(), data_len),
                                CommandLine::kArgSeparator);
  auto fd_ids = GetRemapFd(commands);
  if (fds.size() != fd_ids.size()) {
    LOGE(
        "%{public}s fd size not fit, fds size: %{public}zu ids size: "
        "%{public}zu",
        __func__, fds.size(), fd_ids.size());
    return false;
  }
  for (size_t j = 0; j < fds.size(); j++) {
    fd_mappings.emplace_back(std::make_pair(fds[j], fd_ids[j]));
  }

  return true;
}

std::vector<int> AppSpawnCommunication::GetRemapFd(
    const std::vector<std::string>& commands) {
  return CommandLine::ParseFdRemapIds(commands);
}
}  // namespace ohos::adapter::multiprocess
