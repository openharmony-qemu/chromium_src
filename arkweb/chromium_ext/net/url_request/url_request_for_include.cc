/*
 * Copyright (c) 2025-2025 Huawei Device Co., Ltd.
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

#if BUILDFLAG(ARKWEB_EX_FALLBACK_PROXY)
#include "base/base_switches.h"
#include "base/command_line.h"
#include "arkweb/chromium_ext/content/public/common/content_switches_ext.h"
#endif

namespace net {

#if BUILDFLAG(ARKWEB_EX_FALLBACK_PROXY)
ProxyDelegate* URLRequest::proxy_delegate() const {
  return context_->proxy_delegate();
}

void URLRequest::HandleFallbackProxyResult() {
  if (!response_info_.used_fallback_proxy) {
    return;
  }

  if (!proxy_delegate()) {
    return;
  }

  LOG(DEBUG) << "Handle fallback proxy result, error_code " << status_
             << ", fallback_proxy_error_code " << fallback_proxy_error_code_
             << ", url " << url::LogUtils::ConvertUrlWithMask(url().spec());
  if (fallback_proxy_error_code_ == OK &&
      isolation_info().request_type() ==
          IsolationInfo::RequestType::kMainFrame) {
    proxy_delegate()->AddSuccessMainFrameHosts(url().host());
  }
}
#endif  // BUILDFLAG(ARKWEB_EX_FALLBACK_PROXY)

}  // namespace net
