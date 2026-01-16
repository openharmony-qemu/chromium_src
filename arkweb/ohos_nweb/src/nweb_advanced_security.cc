/*
* Copyright (c) 2025 Huawei Device Co., Ltd.
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/

#include "nweb_advanced_security.h"

#include "arkweb/chromium_ext/content/public/common/content_switches_ext.h"
#include "base/base_switches.h"
#include "base/check.h"
#include "base/command_line.h"
#include "content/public/common/content_switches.h"
#include <dlfcn.h>
#include <cstring>

#include "nweb_hilog.h"

namespace OHOS::NWeb {
const char* feature_name = "RESTRICTED_WEB_FEATURES";
const char* empty_param = "{\"url\":\"\"}";

typedef int32_t (*AdvSecModeGetPtr)(const char *feature, uint32_t featureLen,
      const char *param, uint32_t paramLen, uint32_t *state);

NO_SANITIZE("cfi-icall") NWebAdvancedSecurityHelper::NWebAdvancedSecurityHelper() {
  if (base::CommandLine::InitializedForCurrentProcess()) {
    base::CommandLine* cmd_line = base::CommandLine::ForCurrentProcess();
    std::string process_type = cmd_line->GetSwitchValueASCII(switches::kProcessType);
    if (process_type == switches::kRendererProcess) {
      if (cmd_line->HasSwitch(switches::kAdSecValue)) {
        std::string key = cmd_line->GetSwitchValueASCII(switches::kAdSecValue);
        adstat_ = static_cast<uint32_t>(std::stoi(key));
      }
      WVLOG_D("get advanced sec val %{public}d", adstat_);
      return;
    }
  }

  void *hdl = dlopen("/system/lib64/platformsdk/libdsmm_innersdk.z.so", RTLD_LAZY);
  if (!hdl) {
    WVLOG_E("dlopen failed for %{public}d", errno);
    return;
  }

  AdvSecModeGetPtr func = nullptr;
  func = (AdvSecModeGetPtr)dlsym(hdl, "AdvancedSecurityModeGetStateByFeature");
  if (!func) {
    WVLOG_E("dlsym failed for %{public}d", errno);
    dlclose(hdl);
    return;
  }

  uint32_t state = 0;
  int32_t ret = func(feature_name, strlen(feature_name), empty_param, strlen(empty_param), &state);
  dlclose(hdl);
  if (ret != 0) {
    WVLOG_E("AdvSecModeGetPtr failed for %{public}d", ret);
  } else {
    adstat_ = state;
    WVLOG_I("get advanced sec val %{public}d", adstat_);
  }
}
} // namespace OHOS::NWeb
