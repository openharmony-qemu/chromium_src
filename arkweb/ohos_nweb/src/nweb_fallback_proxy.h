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
 
#ifndef NWEB_FALLBACK_PROXY_H
#define NWEB_FALLBACK_PROXY_H
 
#include <string>
 
#include "cef/include/cef_base.h"
#include "cef/include/cef_values.h"
#include "ohos_nweb/src/capi/nweb_proxy_client_callback.h"
 
namespace OHOS::NWeb {
 
class NwebFallbackProxy {
 public:
  NwebFallbackProxy() = default;
  ~NwebFallbackProxy() = default;
 
  static void PutProxyClientCallback(
      std::shared_ptr<NWebProxyClientCallback> proxy_callback);
  static void OnUpdateProxyToken(std::string old_token);
  static void UpdateProxyToken(const char* token, const char* token_info);
  static void SetGlobalListConfigPath(const char* file_path,
                                      const char* version);
};
 
}  // namespace OHOS::NWeb
 
#endif  // NWEB_FALLBACK_PROXY_H