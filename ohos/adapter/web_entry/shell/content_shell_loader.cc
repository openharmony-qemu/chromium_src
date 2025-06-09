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

#include "ohos/adapter/web_entry/shell/content_shell_loader.h"

#include "ohos/adapter/common/logging.h"

namespace ohos::adapter::web_entry {

std::string ContentShellLoader::GetEntryPoint(int process_type) {
  std::string entry_point;
  switch (process_type) {
    case ProcessType::kMainProcess:
    case ProcessType::kRenderProcess:
    case ProcessType::kGpuProcess:
      entry_point = "ContentMain";
      break;
    default:
      LOGE("GetEntryPoint nothing to do, unknown process type: %{public}d",
           process_type);
      break;
  }
  return entry_point;
}

const std::vector<std::string> ContentShellLoader::GetEntryArgs() {
  const std::vector<std::string> args = {
      "content_shell",
      "--use-gl=egl",
      "--enable-features=UseOzonePlatform",
      "--ozone-platform=ohos",
      "--enable-logging",
      "--v=2",
      "--ozone-dump-file=/data/storage/el2/base/cache/",
      "--log-file=/data/storage/el2/base/cache/test.log",
      "--in-process-gpu",
      "--no-zygote",
      "--content-shell-host-window-size=1139x654",
      "--remote-debugging-port=9222",  // for remote dubug and autotest
      "--remote-allow-origins=http://localhost:9222",  // for remote dubug and
                                                       // autotest
  };
  return args;
}

}  // namespace ohos::adapter::web_entry
