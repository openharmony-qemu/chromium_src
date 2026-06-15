// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ohos/adapter/web_entry/chrome/chrome_loader.h"

#include <accesstoken/ability_access_control.h>

#include "ohos/adapter/common/logging.h"
#include "ohos/adapter/common/shared_library.h"
#include "ohos/adapter/dev_config/dev_config.h"
#include "ohos/adapter/device_info/device_info.h"
#include "ohos/adapter/web_entry/permission_constants.h"

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

bool ShouldDisableJit() {
  if (CheckAdvSecMode()) {
    return true;
  }
  
  if (!OH_AT_CheckSelfPermission(
      PermissionConstants::ALLOW_WRITABLE_CODE_MEMORY)) {
    return true;
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

JSBIND_GLOBAL() {
  JSBIND_FUNCTION(ShouldDisableJit);
}

}  // namespace ohos::adapter::web_entry
