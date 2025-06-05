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

#ifndef OHOS_ADAPTER_MULTIPROCESS_UNIXDOMAINSOCKETHELPER_H_
#define OHOS_ADAPTER_MULTIPROCESS_UNIXDOMAINSOCKETHELPER_H_

#include <cstddef>
#include <string>
#include <vector>

#include "ohos/adapter/multiprocess/scoped_socket.h"
namespace ohos::adapter::multiprocess {
class UnixDomainSocketHelper {
 public:
  static const size_t kMaxFileDescriptors;

  static ScopedSocket CreateServerUnixDomainSocket(const std::string& path);

  static ScopedSocket CreateClientUnixDomainSocket(const std::string& path);

  // Use to enable receiving process IDs in RecvMsgWithPid.  Should be called on
  // the receiving socket (i.e., the socket passed to RecvMsgWithPid). Returns
  // true if successful.
  static bool EnableReceiveProcessId(int fd);

  // Use recvmsg to read a message and an array of file descriptors. Returns
  // -1 on failure. Note: will read, at most, |kMaxFileDescriptors| descriptors.
  static int RecvMsg(int fd, void* msg, size_t length, std::vector<int>& fds);

  // Same as RecvMsg above, but also returns the sender's process ID (as seen
  // from the caller's namespace).  However, before using this function to
  // receive process IDs, EnableReceiveProcessId() should be called on the
  // receiving socket.
  static int RecvMsgWithPid(int fd,
                            void* msg,
                            size_t length,
                            std::vector<int>& fds,
                            pid_t* pid);

  // Use sendmsg to write the given msg and include a vector of file
  // descriptors. Returns true if successful.
  static bool SendMsg(int fd,
                      const void* msg,
                      size_t length,
                      const std::vector<int>& fds);

  static void DeleteFile(const std::string& path);

  static const int KRecvMsgFailed = -1;
 private:
  // Similar to RecvMsg, but allows to specify |flags| for recvmsg(2).
  static int RecvMsgWithFlags(int fd,
                              void* msg,
                              size_t length,
                              int flags,
                              std::vector<int>& fds,
                              pid_t* pid);
};
}  // namespace ohos::adapter::multiprocess
#endif  // OHOS_ADAPTER_MULTIPROCESS_UNIXDOMAINSOCKETHELPER_H_
