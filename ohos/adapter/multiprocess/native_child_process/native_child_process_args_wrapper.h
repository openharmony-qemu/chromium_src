/*
 * Copyright (c) 2023-2025 Huawei Device Co., Ltd.
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

#ifndef OHOS_ADAPTER_MULTIPROCESS_ISOLATE_PROCESS_NATIVECHILDPROCESSARGSWRAPPER_H_
#define OHOS_ADAPTER_MULTIPROCESS_ISOLATE_PROCESS_NATIVECHILDPROCESSARGSWRAPPER_H_

#include <string>
#include <vector>

#include <AbilityKit/native_child_process.h>

namespace ohos::adapter::multiprocess {

struct NodeDeleter {
  void operator()(NativeChildProcess_Fd* node) const noexcept;
};

using UniqueNode = std::unique_ptr<NativeChildProcess_Fd, NodeDeleter>;

class NativeChildProcessArgsWrapper {
 public:
  NativeChildProcessArgsWrapper(
      const std::vector<std::string>& command,
      const std::vector<std::pair<int, int>>& fds_to_remap);
  ~NativeChildProcessArgsWrapper() = default;

  NativeChildProcess_Args Get();

  static void Parse(NativeChildProcess_Args args,
                    std::vector<std::string>& commands,
                    std::vector<std::pair<int32_t, int32_t>>& fds_to_remap);

 private:
  std::string params_;
  UniqueNode fd_list_;
  NativeChildProcess_Args native_args_ = {};
};
}  // namespace ohos::adapter::multiprocess

#endif  // OHOS_ADAPTER_MULTIPROCESS_ISOLATE_PROCESS_NATIVECHILDPROCESSARGSWRAPPER_H_
