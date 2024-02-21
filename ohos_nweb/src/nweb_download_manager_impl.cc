/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "nweb_download_manager_impl.h"
#include "cef/libcef/browser/net_service/net_helpers.h"
#include "cef_delegate/nweb_application.h"
#include "content/public/browser/network_service_instance.h"
#include "nweb_hilog.h"
#include "services/network/network_service.h"

namespace OHOS::NWeb {

void NWebDownloadManagerImpl::SetHttpDns(
    std::shared_ptr<NWebDOHConfig> config) {
  if (!config) {
    return;
  }

#if defined(OHOS_HTTP_DNS)
  WVLOG_I("set http dns config mode:%{public}d config: %{public}s",
          config->GetMode(), config->GetConfig().c_str());

  net_service::NetHelpers::doh_mode = config->GetMode();
  net_service::NetHelpers::doh_config = config->GetConfig();

  CefApplyHttpDns();
#endif // defined(OHOS_HTTP_DNS)
}

void NWebDownloadManagerImpl::SetConnectionTimeout(const int &timeout) {
#if defined(OHOS_EX_NETWORK_CONNECTION)
  net_service::NetHelpers::connection_timeout = timeout;
  if (content::GetNetworkService() != nullptr) {
    content::GetNetworkService()->SetConnectTimeout(
        net_service::NetHelpers::connection_timeout);

    WVLOG_I("set connection timeout value in NetHelpers is: %{public}d",
            net_service::NetHelpers::connection_timeout);
  } else {
    WVLOG_E("net_work_service is nullptr");
  }
#endif // OHOS_EX_NETWORK_CONNECTION
}

} // namespace OHOS::NWeb
