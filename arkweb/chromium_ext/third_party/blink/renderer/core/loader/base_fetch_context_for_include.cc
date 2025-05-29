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

namespace blink {

class BaseFetchContextUtil {
 public:
  static std::optional<ResourceRequestBlockedReason>
  CanRequestBasedOnSubresourceFilterOnlyForInclude(
      raw_ptr<const BaseFetchContext> obj,
      ResourceType type,
      const ResourceRequest& resource_request,
      const KURL& url,
      const ResourceLoaderOptions& options,
      ReportingDisposition reporting_disposition) {
#if BUILDFLAG(ARKWEB_ADBLOCK)
    auto* user_subresource_filter = obj->GetUserSubresourceFilter();
    if (user_subresource_filter &&
        !user_subresource_filter->AllowLoad(
            url, resource_request.GetRequestDestination(),
            reporting_disposition)) {
      if (reporting_disposition == ReportingDisposition::kReport) {
        obj->DispatchDidBlockRequest(
            resource_request, options,
            ResourceRequestBlockedReason::kSubresourceFilter, type);
      }
      LOG(INFO) << "[User AdBlock] Subresource request blocked : ***";
#ifdef OHOS_LOGGER_REPORT
      LOG_FEEDBACK(INFO) << "[User AdBlock] Subresource request blocked : "
                         << url::LogUtils::ConvertUrlWithMask(
                                url.GetString().Utf8());
#endif
      return ResourceRequestBlockedReason::kSubresourceFilter;
    }
#endif
    return std::nullopt;
  }
};

}  // namespace blink
