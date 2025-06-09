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

#include "ohos/adapter/web_entry/chrome/chrome_loader.h"

#include "ohos/adapter/common/logging.h"
#include "ohos/adapter/common/shared_library.h"
#include "ohos/adapter/dev_config/dev_config.h"
#include "ohos/adapter/device_info/device_info.h"

namespace ohos::adapter::web_entry {
namespace {
__attribute__((no_sanitize("cfi", "cfi-icall")))
bool CheckAdvSecMode() {
  uint32_t mode = -1;
  common::SharedLibrary shared_library_("/system/lib64/platformsdk/",
                                        "dsmm_innersdk.z");
  auto fn = shared_library_.GetFunction<int(const char*, uint32_t, const char*,
                                            uint32_t, uint32_t*)>(
      "AdvancedSecurityModeGetStateByFeature");
  if (fn != nullptr) {
    // returns 0 if success or errcode
    int32_t ret = fn("default", 7, "default", 7, &mode); // len:7
    if (ret == 0 && mode == 1) {
      return true;
    }
  }
  return false;
}
}  // namespace

std::string ChromeMainLoader::GetEntryPoint(int process_type) {
  std::string entry_point;
  switch (process_type) {
    case ProcessType::kMainProcess:
    case ProcessType::kRenderProcess:
    case ProcessType::kGpuProcess:
      entry_point = "ChromeMain";
      break;
    case ProcessType::kCrashpadHandler:
      entry_point = "CrashpadHandlerMain";
      break;
    default:
      LOGE("GetEntryPoint nothing to do, unknown process type: %{public}d",
           process_type);
      break;
  }
  return entry_point;
}

const std::vector<std::string> ChromeMainLoader::GetEntryArgs() {
  std::vector<std::string> args = {
      "chrome",
      "--use-gl=egl",
      "--enable-features=UseOzonePlatform",
      "--ozone-platform=ohos",
      "--enable-logging",
      "--ozone-dump-file=/data/storage/el2/base/cache/",
      "--no-zygote",
      "--user-data-dir=/data/storage/el2/base/files/",
      "--log-net-log",
      "--force-renderer-accessibility=basic",
  };
  ohos::adapter::device_info::DeviceType device_type =
    ohos::adapter::device_info::DeviceInfo::GetInstance().GetDeviceType();
  // pad not support gpu process
  if (device_type == ohos::adapter::device_info::DeviceType::_TABLET) {
    args.push_back("--in-process-gpu");
    args.push_back("--js-flags=--jitless");
  }
  if (CheckAdvSecMode()) {
    args.emplace_back("--js-flags=--jitless");
  }
  for (const auto& command : GetDevCommandLines()) {
    args.emplace_back(command);
  }
  return args;
}

}  // namespace ohos::adapter::web_entry
