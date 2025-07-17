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

#include "nweb_extension_top_sites_cef_delegate.h"
#include "cef/libcef/browser/browser_host_base.h"
#include "nweb_common.h"

#if BUILDFLAG(ARKWEB_NWEB_EX)
#include "ohos_nweb_ex/core/extension/nweb_extension_top_sites_dispatcher.h"
#endif

namespace OHOS::NWeb {

namespace {
static TopSitesCallback g_top_sites_callback;
} // namespace

void NWebExtensionTopSitesCefDelegate::Get(TopSitesCallback callback) {
#if BUILDFLAG(ARKWEB_NWEB_EX)
  g_top_sites_callback = std::move(callback);
  NWebExtensionTopSitesDispatcher::Get();
#endif
}

void NWebExtensionTopSitesCefDelegate::GetCallback(const std::vector<NWebExtensionTopSitesMostVisitedURL>& data,
    const std::optional<std::string>& error) {
  std::move(g_top_sites_callback).Run(data, error);
}

}  // namespace OHOS::NWeb
