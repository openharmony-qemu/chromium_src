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

#include "net_config_adapter_impl.h"

#include <cstring>
#include <string>
#include <vector>

#include "arkweb/ohos_nweb/src/nweb_hilog.h"
#include "network/netstack/net_ssl/net_ssl_c.h"

using namespace OHOS::NWeb;

namespace OHOS::NWeb {

bool NetConfigAdapterImpl::GetIsCleartextPermittedByHostName(
    const std::string& hostname) {
  bool is_cleartext_permitted = true;
  int32_t ret = OH_Netstack_IsCleartextPermittedByHostName(
      hostname.c_str(), &is_cleartext_permitted);
  if (ret != 0) {
    WVLOG_E(
        "GetIsCleartextPermittedByHostName for hostname:%{public}s failed, "
        "ret:%{public}d",
        hostname.c_str(), ret);
    return true;
  }
  return is_cleartext_permitted;
}

NO_SANITIZE("cfi-icall")
bool NetConfigAdapterImpl::GetIsCleartextCfgByComponent(
    const std::string& component) {
  bool is_cleartext_cfg = false;
  int32_t ret = OH_Netstack_IsCleartextCfgByComponent(component.c_str(),
                                                      &is_cleartext_cfg);
  if (ret != 0) {
    WVLOG_E(
        "GetIsCleartextCfgByComponent for hostname:%{public}s failed, "
        "ret:%{public}d",
        component.c_str(), ret);
    return false;
  }
  return is_cleartext_cfg;
}

}  // namespace OHOS::NWeb