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

class DistillabilityServiceImplExt : public DistillabilityServiceImpl {
 public:
  explicit DistillabilityServiceImplExt(
      base::WeakPtr<DistillabilityDriver> distillability_driver)
      : DistillabilityServiceImpl(distillability_driver) {}

  ~DistillabilityServiceImplExt() override = default;

#if BUILDFLAG(ARKWEB_READER_MODE)
  void NotifyIsPageDistillable(mojom::PageDistillableInfoPtr page_info,
                               bool is_last_update,
                               bool is_mobile_friendly) override {
    if (!distillability_driver_)
      return;
    if (!page_info)
      return;
    DistillabilityResult result;
    result.page_info = *page_info;
    result.is_distillable = page_info->pageDistillable;
    result.is_last = is_last_update;
    result.is_mobile_friendly = is_mobile_friendly;
    distillability_driver_->OnDistillability(result);
  }

  void GetHostDistillerInfo(const std::string& host,
                            GetHostDistillerInfoCallback callback) override {
    blink::mojom::UrlHostDistillerInfoPtr distiller_info =
        blink::mojom::UrlHostDistillerInfo::New();
    distiller_info->is_distillable = false;

    const nweb_ex::BrowserReaderModeWhitelistConfig* whitelist_config =
        nweb_ex::AlloyBrowserReaderModeConfig::GetInstance()->QueryWhitelistConfig(host);
    if (whitelist_config) {
      distiller_info->is_distillable = true;
      distiller_info->templates_for_host = whitelist_config->template_ids;
      distiller_info->detail_regexes =
          whitelist_config->page_recognition_config.detail_regexes;
      distiller_info->content_regexes =
          whitelist_config->page_recognition_config.content_regexes;
      distiller_info->blacklist_regexes =
          whitelist_config->page_recognition_config.blacklist_regexes;
    }

    std::move(callback).Run(std::move(distiller_info));
  }
#endif
};

#if BUILDFLAG(ARKWEB_READER_MODE)
void DistillabilityDriver::DidStartNavigation(
    content::NavigationHandle* navigation_handle) {
  if (navigation_handle->IsInMainFrame()) {
    std::vector<GURL> result{navigation_handle->GetReferrer().url,
                             navigation_handle->GetURL()};
    nweb_ex::AlloyBrowserReaderModeConfig::GetInstance()->UpdateRedirectChain(result);
  }
}

void DistillabilityDriver::DidRedirectNavigation(
    content::NavigationHandle* navigation_handle) {
  nweb_ex::AlloyBrowserReaderModeConfig::GetInstance()->UpdateRedirectChain(
      navigation_handle->GetRedirectChain());
}
#endif  // ARKWEB_READER_MODE
