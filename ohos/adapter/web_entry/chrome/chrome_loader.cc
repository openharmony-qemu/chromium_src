// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ohos/adapter/web_entry/chrome/chrome_loader.h"

#include "ohos/adapter/common/logging.h"
#include "ohos/adapter/dev_config/dev_config.h"

namespace ohos::adapter::web_entry {

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
      "--v=2",
      "--ozone-dump-file=/data/storage/el2/base/cache/",
      "--log-file=/data/storage/el2/base/cache/test.log",
      "--no-zygote",
      "--user-data-dir=/data/storage/el2/base/files/",
  };
  for (const auto& command : GetDevCommandLines()) {
    args.emplace_back(command);
  }
  return args;
}

}  // namespace ohos::adapter::web_entry
