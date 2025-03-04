// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "app_spawn_communication.h"

#include <unistd.h>

#include <algorithm>
#include <memory>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include "ohos/adapter/common/logging.h"
#include "ohos/adapter/multiprocess/scoped_socket.h"
#include "ohos/adapter/multiprocess/unix_domain_socket_helper.h"

namespace ohos::adapter::multiprocess {
namespace {
AppSpawnCommunication::FdRemapVector g_fd_id_remap;
}

enum class RequstCommandType {
  kRequestUnknown = -1,
  kRequestLength = 1,
  kRequestData,
  kRequestEnd
};

const std::vector<RequstCommandType> RequstCommandList = {
    RequstCommandType::kRequestLength, RequstCommandType::kRequestData,
    RequstCommandType::kRequestEnd};

const int DEFAULT_BUFFER_SIZE = 16;
const std::string FD_REMAP_COMMAND = "--fds_to_remap=";
const std::string FD_SEPARATOR = ",";
const std::string COMMAND_SEPARATOR = "#";
const std::string DOMAIN_SOCKET_PATH =
    "/data/storage/el2/base/files/start.sock";

int RecvRequest(int fd, pid_t pid) {
  char buff[8] = {0};
  std::vector<int> ret_fds;
  pid_t ret_pid = -1;

  if (UnixDomainSocketHelper::RecvMsgWithPid(fd, buff, sizeof(buff), ret_fds,
                                             &ret_pid) == -1) {
    return -1;
  }

  if (pid != ret_pid) {
    LOGE("pid error");
    return -1;
  }

  return std::stoi(buff);
}

std::string BuildSharedFDCommand(
    const std::vector<std::string>& commands,
    const AppSpawnCommunication::FdRemapVector& fds,
    std::vector<int>& share_fds) {
  std::string shared_fd_command = FD_REMAP_COMMAND;
  for (auto& [rawfd, remap_fd] : fds) {
    LOGD("HandleRequest fd %{public}d => %{public}d", rawfd, remap_fd);
    share_fds.push_back(rawfd);
    shared_fd_command += std::to_string(remap_fd) + FD_SEPARATOR;
  }
  shared_fd_command.pop_back();
  return shared_fd_command;
}

void FixInitClientFdForCrashDumpHandler(
    std::vector<std::string>& cmds,
    const AppSpawnCommunication::FdRemapVector& id_fds) {
  std::string init_fd;
  for (auto& [id, fd] : id_fds) {
    // refer to content/public/common/content_descriptors.h
    // id 0, is for crashdump
    if (id == 0) {
      init_fd = std::to_string((int)fd);
      break;
    }
  }
  for (int i = cmds.size() - 1; i >= 0; i--) {
    auto cmd = cmds[i];
    if (cmd.npos != cmd.find("--initial-client-fd=")) {
      std::string rep("--initial-client-fd=");
      rep.append(init_fd);
      cmds[i] = rep;
      return;
    }
  }
}

bool AppSpawnCommunication::PrepareHandleRequest(
    const std::vector<std::string>& commands,
    const FdRemapVector& fds) {
  if (!PrepareResponseData(commands, fds)) {
    return false;
  }
  return PrepareCommunication();
}

bool AppSpawnCommunication::PrepareResponseData(
    const std::vector<std::string>& commands,
    const FdRemapVector& fds) {
  std::vector<int> share_fds;
  auto shared_fd_command = BuildSharedFDCommand(commands, fds, share_fds);
  auto commandlines = BuildCommands(commands, {shared_fd_command});
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
  ScopedSocket server_socket =
      UnixDomainSocketHelper::CreateServerUnixDomainSocket(DOMAIN_SOCKET_PATH);
  if (!server_socket.Valid()) {
    return false;
  }
  if (!UnixDomainSocketHelper::EnableReceiveProcessId(
          server_socket.GetSocket())) {
    LOGE("EnableReceiveProcessId Failed");
    return false;
  }
  this->server_socket_ = std::make_unique<ScopedSocket>(server_socket);
  return true;
}

bool AppSpawnCommunication::HandleRequestAfterChildProcStart(pid_t child_pid) {
  ScopedSocket cs = this->GetClientSocket();
  auto commandType = RequstCommandType::kRequestUnknown;
  do {
    commandType = (RequstCommandType)RecvRequest(cs.GetSocket(), child_pid);
    switch (commandType) {
      case RequstCommandType::kRequestLength: {
        // step 1: Handle Requst Data Length Command and Send Data Length
        std::string length = std::to_string(this->GetCmdLinesSize());
        UnixDomainSocketHelper::SendMsg(cs.GetSocket(), length.data(),
                                        length.size(), {});
        break;
      }
      case RequstCommandType::kRequestData:
        // step 2: Handle Requst Data Command and Send Data
        UnixDomainSocketHelper::SendMsg(cs.GetSocket(), this->GetCmdLinesData(),
                                        this->GetCmdLinesSize(),
                                        this->share_fds_);
        break;
      case RequstCommandType::kRequestUnknown:
        LOGE("[%{public}s#%{public}s] Unknow Request: %{public}d", __FILE__,
             __FUNCTION__, static_cast<int>(commandType));
        return false;
      default:
        break;
    }
  } while (commandType != RequstCommandType::kRequestEnd);
  return true;
}

AppSpawnCommunication::~AppSpawnCommunication() {
  this->server_socket_->Close();
  UnixDomainSocketHelper::DeleteFile(DOMAIN_SOCKET_PATH);
}

bool AppSpawnCommunication::RequestCommandline(
    std::vector<std::string>& commands) {
  auto remote_fd =
      UnixDomainSocketHelper::CreateClientUnixDomainSocket(DOMAIN_SOCKET_PATH);

  size_t mem_length = DEFAULT_BUFFER_SIZE;
  std::vector<int> fds;
  for (size_t i = 0; i < RequstCommandList.size(); i++) {
    auto command = RequstCommandList[i];
    auto request = std::to_string((int)command);
    std::vector<char> mem_buf(mem_length);
    UnixDomainSocketHelper::SendMsg(remote_fd.GetSocket(), request.data(),
                                    request.size(), {});
    switch (command) {
      case RequstCommandType::kRequestLength:
        // step 1: Requst data length
        UnixDomainSocketHelper::RecvMsg(remote_fd.GetSocket(), &mem_buf[0],
                                        mem_length, fds);
        mem_length = std::stoi(&mem_buf[0]) + 1;
        if (mem_length > static_cast<size_t>(std::numeric_limits<int>::max())) {
          LOGE("receivce data length error");
          remote_fd.Close();
          return false;
        }
        break;
      case RequstCommandType::kRequestData: {
        // step 2: Requst command data and call dup2
        UnixDomainSocketHelper::RecvMsg(remote_fd.GetSocket(), &mem_buf[0],
                                        mem_length, fds);
        LOGD("receivce commandlines length: %{public}zu, data: %{public}s",
             mem_length, &mem_buf[0]);
        commands = Split(&mem_buf[0], COMMAND_SEPARATOR);
        auto fd_map_ids = GetRemapFd(commands);
        if (fds.size() != fd_map_ids.size()) {
          LOGE(
              "RequestCommandline fd size not fit, %{public}lu "
              "%{public}lu",
              fds.size(), fd_map_ids.size());
          remote_fd.Close();
          return false;
        }
        for (size_t j = 0; j < fds.size(); j++) {
          g_fd_id_remap.push_back(std::make_pair(fd_map_ids[j], fds[j]));
        }
        FixInitClientFdForCrashDumpHandler(commands, g_fd_id_remap);
        break;
      }
      default:
        break;
    }
  }
  remote_fd.Close();
  return true;
}

std::string AppSpawnCommunication::BuildCommands(
    const std::vector<std::string>& commands,
    const std::vector<std::string>& extra_commands) {
  const size_t min_commands_size = 1;
  if (commands.size() < min_commands_size) {
    return "";
  }
  std::stringstream argv_ss;
  for (size_t i = 0; i < commands.size() - 1; ++i) {
    argv_ss << commands[i] << COMMAND_SEPARATOR;
  }
  argv_ss << commands[commands.size() - 1];
  if (extra_commands.size() != 0) {
    argv_ss << COMMAND_SEPARATOR;
  }
  for (size_t i = 0; i < extra_commands.size() - 1; ++i) {
    argv_ss << extra_commands[i] << COMMAND_SEPARATOR;
  }
  argv_ss << extra_commands[extra_commands.size() - 1];
  return argv_ss.str();
}

std::vector<std::string> AppSpawnCommunication::Split(
    const std::string& src,
    const std::string& separator) {
  const size_t limit = 64;
  std::vector<std::string> res;
  if (src.empty()) {
    return res;
  }
  size_t start = 0;
  size_t end = 0;
  while (start < src.size() && (limit == 0 || res.size() < limit)) {
    end = src.find(separator, start);
    if (end == std::string::npos) {
      res.push_back(src.substr(start));
      return res;
    }
    res.push_back(src.substr(start, end - start));
    start = end + separator.length();
  }
  return res;
}

std::vector<int> AppSpawnCommunication::GetRemapFd(
    const std::vector<std::string>& commands) {
  std::string fd_string;
  for (size_t i = 0; i < commands.size(); i++) {
    auto command = commands[i];
    if (command.starts_with(FD_REMAP_COMMAND)) {
      fd_string = command.substr(FD_REMAP_COMMAND.size(),
                                 command.size() - FD_REMAP_COMMAND.size());
    }
  }
  LOGD("fd string %{public}s", fd_string.c_str());
  auto fs_strs = Split(fd_string, FD_SEPARATOR);
  std::vector<int> res;
  for (auto& fd_str : fs_strs) {
    if (fd_str.empty()) {
      continue;
    }
    res.push_back(std::stoi(fd_str));
  }
  return res;
}

AppSpawnCommunication::FdRemapVector AppSpawnCommunication::GetFdIdsRemap() {
  return g_fd_id_remap;
}

void AppSpawnCommunication::SetFdIdsRemap(FdRemapVector& fds) {
  g_fd_id_remap = fds;
}
}  // namespace ohos::adapter::multiprocess
