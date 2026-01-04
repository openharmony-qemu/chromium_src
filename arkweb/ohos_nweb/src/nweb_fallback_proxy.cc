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
 
#include "nweb_fallback_proxy.h"
 
#include "nweb_hilog.h"
#include "cef/ohos_cef_ext/libcef/browser/fallback_proxy/fallback_proxy_service.h"
#include "cef/ohos_cef_ext/libcef/browser/arkweb_global_list_config.h"
#include "base/no_destructor.h"
 
namespace {
std::shared_ptr<NWebProxyClientCallback>& GetProxyClientCallback() {
  static base::NoDestructor<std::shared_ptr<NWebProxyClientCallback>> proxy_client_callback_(nullptr);
  return *proxy_client_callback_;
}
}
 
namespace OHOS::NWeb {
 
// static
void NwebFallbackProxy::PutProxyClientCallback(
    std::shared_ptr<NWebProxyClientCallback> proxy_callback) {
  LOG(DEBUG) << "PutProxyClientCallback callback";
  GetProxyClientCallback() = proxy_callback;
}
 
// static
void NwebFallbackProxy::OnUpdateProxyToken(const std::string& old_token) {
  LOG(DEBUG) << "NwebFallbackProxy::onUpdateProxyToken";
  if (!GetProxyClientCallback() || !GetProxyClientCallback()->onUpdateProxyToken) {
    LOG(ERROR) << "GetProxyClientCallback is null";
    return;
  }
  GetProxyClientCallback()->onUpdateProxyToken(old_token.c_str());
}
 
// static
void NwebFallbackProxy::UpdateProxyToken(const char* token, const char* token_info) {
  LOG(DEBUG) << "NwebFallbackProxy::UpdateProxyToken";
  if (token == nullptr || token_info == nullptr) {
    LOG(ERROR) << "UpdateProxyToken token or token_info is null";
    return;
  }
  fallback_proxy::FallbackProxyService::GetInstance()->UpdateProxyToken(
      token, token_info);
}
 
// static
void NwebFallbackProxy::SetGlobalListConfigPath(const char* file_path,
                                                const char* version) {
  LOG(DEBUG) << "NwebFallbackProxy::SetGlobalListConfigPath";
  if (file_path == nullptr || version == nullptr) {
    LOG(ERROR) << "SetGlobalListConfigPath file_path or version is null";
    return;
  }
  std::string file_path_str = file_path;
  std::string version_str = version;
  fallback_proxy::ArkwebGlobalListConfig::GetInstance()
      ->SetGlobalListConfigPath(base::FilePath(file_path_str), version_str);
}
 
}  // namespace OHOS::NWeb