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

#include "message_helper.h"

#include <sys/socket.h>
#include <sys/uio.h>
#include <unistd.h>

#include <cerrno>
#include <cstring>

#include "ohos/adapter/common/logging.h"

namespace ohos::adapter::multiprocess {

bool MessageHelper::SendMsg(int fd,
                            const void* data,
                            size_t len,
                            const std::vector<int>& fds) {
  struct msghdr msg = {};
  struct iovec iov = {const_cast<void*>(data), len};
  msg.msg_iov = &iov;
  msg.msg_iovlen = 1;

  std::vector<char> control_buffer;
  if (!fds.empty()) {
    size_t control_len = CMSG_SPACE(sizeof(int) * fds.size());
    control_buffer.resize(control_len);

    msg.msg_control = control_buffer.data();
    msg.msg_controllen = control_len;

    struct cmsghdr* cmsg = CMSG_FIRSTHDR(&msg);
    cmsg->cmsg_level = SOL_SOCKET;
    cmsg->cmsg_type = SCM_RIGHTS;
    cmsg->cmsg_len = CMSG_LEN(sizeof(int) * fds.size());
    memcpy(CMSG_DATA(cmsg), fds.data(), sizeof(int) * fds.size());
  }

  ssize_t sent = sendmsg(fd, &msg, MSG_NOSIGNAL);
  if (sent != static_cast<ssize_t>(len)) {
    LOGE("MessageHelper::SendMsg error, errno=%{public}d, Reason: %{public}s",
         errno, strerror(errno));
    return false;
  }
  return true;
}

ssize_t MessageHelper::RecvMsg(int fd,
                               void* data,
                               size_t len,
                               std::vector<int>& fds) {
  return RecvMsgWithPid(fd, data, len, fds, nullptr);
}

ssize_t MessageHelper::RecvMsgWithPid(int fd,
                                      void* data,
                                      size_t len,
                                      std::vector<int>& fds,
                                      pid_t* pid) {
  fds.clear();

  struct msghdr msg = {};
  struct iovec iov = {data, len};
  msg.msg_iov = &iov;
  msg.msg_iovlen = 1;

  std::vector<char> control_buffer(kControlBufferSize);
  msg.msg_control = control_buffer.data();
  msg.msg_controllen = control_buffer.size();

  ssize_t received = recvmsg(fd, &msg, 0);
  if (received == -1) {
    LOGE("MessageHelper::RecvMsg error, errno=%{public}d, Reason: %{public}s",
         errno, strerror(errno));
    return -1;
  }

  int* wire_fds = nullptr;
  size_t wire_fds_len = 0;
  int recv_pid = -1;

  if (msg.msg_controllen > 0) {
    for (struct cmsghdr* cmsg = CMSG_FIRSTHDR(&msg); cmsg;
         cmsg = reinterpret_cast<struct cmsghdr*>(CMSG_NXTHDR(&msg, cmsg))) {
      size_t payload_len = cmsg->cmsg_len - CMSG_LEN(0);

      if (cmsg->cmsg_level == SOL_SOCKET && cmsg->cmsg_type == SCM_RIGHTS) {
        wire_fds = reinterpret_cast<int*>(CMSG_DATA(cmsg));
        wire_fds_len = payload_len / sizeof(int);
      }

      if (cmsg->cmsg_level == SOL_SOCKET &&
          cmsg->cmsg_type == SCM_CREDENTIALS) {
        recv_pid = reinterpret_cast<struct ucred*>(CMSG_DATA(cmsg))->pid;
      }
    }
  }

  if ((msg.msg_flags & MSG_TRUNC) || (msg.msg_flags & MSG_CTRUNC)) {
    if (msg.msg_flags & MSG_CTRUNC) {
      LOGE(
          "MessageHelper::RecvMsg returned MSG_CTRUNC flag, buffer len is "
          "%{public}u",
          msg.msg_controllen);
    }
    for (size_t i = 0; i < wire_fds_len; ++i) {
      close(wire_fds[i]);
    }
    errno = EMSGSIZE;
    return -1;
  }

  if (wire_fds && wire_fds_len > 0) {
    fds.assign(wire_fds, wire_fds + wire_fds_len);
  }

  if (pid) {
    *pid = recv_pid;
  }

  return received;
}

}  // namespace ohos::adapter::multiprocess
