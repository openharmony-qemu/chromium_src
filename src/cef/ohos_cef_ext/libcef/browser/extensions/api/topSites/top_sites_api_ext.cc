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

#include "chrome/browser/extensions/api/top_sites/top_sites_api.h"

#include "base/functional/bind.h"
#include "base/values.h"
#include "chrome/browser/history/top_sites_factory.h"
#include "chrome/browser/profiles/profile.h"
#include "components/history/core/browser/top_sites.h"
#include "components/profile_metrics/browser_profile_type.h"
#include "ohos_nweb/src/capi/web_extension_top_sites_items.h"
#include "ohos_nweb/src/cef_delegate/nweb_extension_top_sites_cef_delegate.h"

namespace extensions {

ExtensionFunction::ResponseAction TopSitesGetFunction::Run() {
  NWebExtensionTopSitesQueryOptions options;
  options.contextType = "REGULAR";
  if (browser_context()->IsOffTheRecord()) { // 无痕模式
    options.contextType = "INCOGNITO";
  }
  options.includeIncognitoInfo = include_incognito_information();
  LOG(INFO) << "TopSitesGetFunction contextType=" << options.contextType.value()
            << ", includeIncognitoInfo=" << options.includeIncognitoInfo.value();
  call_get_ = true;
  OHOS::NWeb::NWebExtensionTopSitesCefDelegate::Get(
      base::BindRepeating(&TopSitesGetFunction::OnGet, weak_ptr_factory_.GetWeakPtr()), options);
  call_get_ = false;
  if (did_respond()) {
    LOG(INFO) << "TopSitesGetFunction did_respond";
    return AlreadyResponded();
  }
  AddRef();
  LOG(INFO) << "TopSitesGetFunction AddRef";
  return RespondLater();
}

void TopSitesGetFunction::OnGet(const base::WeakPtr<TopSitesGetFunction>& function,
                                const std::vector<NWebExtensionTopSitesMostVisitedURL>& data,
                                const std::optional<std::string>& error) {
  DCHECK(function);
  if (!function) {
    LOG(ERROR) << "TopSitesGetFunction::OnGet function is null";
    return;
  }

  if (error) {
    std::string err_msg = error ? error.value() : "get failed";
    function->Respond(function->Error(err_msg));
  } else {
    base::Value::List pages_value;
    for (const auto& url : data) {
      base::Value::Dict page_value;
      page_value.Set("url", url.url);
      if (url.title.empty()) {
        page_value.Set("title", url.url);
      } else {
        page_value.Set("title", url.title);
      }
      pages_value.Append(std::move(page_value));
    }

    function->Respond(
      function->has_callback() ? function->WithArguments(std::move(pages_value))
                               : function->NoArguments());
  }

  if (!function->call_get_) {
    LOG(INFO) << "TopSitesGetFunction Release";
    function->Release();
  }
 }

}  // namespace extensions
