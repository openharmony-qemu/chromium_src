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

#ifndef OHOS_ADAPTER_MULTIPROCESS_MESSAGE_HELPER_H_
#define OHOS_ADAPTER_MULTIPROCESS_MESSAGE_HELPER_H_

#include <sys/socket.h>
#include <sys/types.h>
#include <cstddef>
#include <vector>

namespace ohos::adapter::multiprocess {

class MessageHelper {
 public:
  static constexpr size_t kMaxFileDescriptors = 16;
  static constexpr size_t kControlBufferSize =
      CMSG_SPACE(sizeof(int) * kMaxFileDescriptors) +
      CMSG_SPACE(sizeof(struct ucred));

  static bool SendMsg(int fd,
                      const void* data,
                      size_t len,
                      const std::vector<int>& fds);

  static ssize_t RecvMsg(int fd, void* data, size_t len, std::vector<int>& fds);

  static ssize_t RecvMsgWithPid(int fd,
                                void* data,
                                size_t len,
                                std::vector<int>& fds,
                                pid_t* pid);
};

}  // namespace ohos::adapter::multiprocess

#endif  // OHOS_ADAPTER_MULTIPROCESS_MESSAGE_HELPER_H_
