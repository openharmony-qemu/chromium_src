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

#include <dlfcn.h>
#include <netdb.h>
#include <network/netstack/net_ssl/net_ssl_c.h>

#include <cstring>
#include <string>
#include <vector>

#include "arkweb/ohos_nweb/src/nweb_hilog.h"
#include "base/native_library.h"	
#include "net/base/network_handle.h"

using namespace OHOS::NWeb;

namespace OHOS::NWeb {

namespace {

using OHIsCleartextCfgByComponent = int32_t(*)(const char* component,
                                              bool* componentCfg);

OHIsCleartextCfgByComponent GetOHIsCleartextCfgByComponent() {
#if defined(ARCH_CPU_ARM64)
  base::FilePath file("system/lib64/ndk/libnet_ssl.so");
#else
  base::FilePath file("system/lib/ndk/libnet_ssl.so");
#endif
  void* dl = dlopen(file.value().c_str(), RTLD_NOW);
  return dl == nullptr ? nullptr
                       : reinterpret_cast<OHIsCleartextCfgByComponent>(dlsym(
                             dl, "OH_Netstack_IsCleartextCfgByComponent"));
}

}  // namespace

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
  static OHIsCleartextCfgByComponent get_iscleartextcfg_by_component =	
      GetOHIsCleartextCfgByComponent();	
  if (!get_iscleartextcfg_by_component) {
    return false;
  }
  int32_t ret =
      get_iscleartextcfg_by_component(component.c_str(), &is_cleartext_cfg);
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