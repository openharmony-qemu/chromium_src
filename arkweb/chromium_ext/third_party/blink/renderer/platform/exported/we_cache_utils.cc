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
#include "arkweb/chromium_ext/third_party/blink/renderer/platform/exported/we_cache_utils.h"

#if BUILDFLAG(ARKWEB_INJECT_OFFLINE_RESOURCE)
#include <vector>
#include "third_party/blink/renderer/platform/loader/fetch/resource_response.h"
#include "base/trace_event/trace_event.h"
#include "net/http/http_status_code.h"
#include "third_party/blink/renderer/core/loader/resource/css_style_sheet_resource.h"
#include "third_party/blink/renderer/core/loader/resource/image_resource.h"
#include "third_party/blink/renderer/core/loader/resource/script_resource.h"
#include "third_party/blink/renderer/platform/network/mime/mime_type_registry.h"
#endif
#include "third_party/blink/renderer/platform/loader/fetch/memory_cache.h"
#include "arkweb/chromium_ext/third_party/blink/renderer/core/loader/resource/css_style_sheet_resource_utils.h"
#include "arkweb/chromium_ext/third_party/blink/renderer/core/loader/resource/image_resource_utils.h"
#include "arkweb/chromium_ext/third_party/blink/renderer/core/loader/resource/script_resource_utils.h"

namespace blink {

WebCacheUtils::WebCacheUtils(WebCache* web_cache)
{
  this->web_cache_ = web_cache;
}

#if BUILDFLAG(ARKWEB_INJECT_OFFLINE_RESOURCE)
void WebCacheUtils::AddResourceToCache(
    const std::string& url,
    const std::string& origin,
    const std::vector<uint8_t>& resource,
    const base::flat_map<std::string, std::string>& response_headers,
    const uint64_t type)
{
  TRACE_EVENT1("net", "WebCache::AddResourceToCache", "url", url.c_str());
  MemoryCache* cache = MemoryCache::Get();
  if (!cache) {
    LOG(ERROR) << "Cannot get MemoryCache.";
    return;
  }

  OfflineResourceType resource_type = static_cast<OfflineResourceType>(type);
  Resource* resource_obj;

  KURL kurl((String(url)));
  KURL origin_url((String(origin)));

  ResourceResponse response = GetResourceResponse(kurl, response_headers);

  AtomicString mime_type = response.MimeType();
  if (resource_type == OfflineResourceType::MODULE_JS &&
      !MIMETypeRegistry::IsSupportedJavaScriptMIMEType(mime_type)) {
    LOG(ERROR) << "Not supported MIMEType for module Javascript: "
               << mime_type.GetString();
    return;
  }

  switch (resource_type) {
    case OfflineResourceType::IMAGE:
      resource_obj = ImageResourceUtils::CreateForOfflineResource(kurl, origin_url);
      break;
    case OfflineResourceType::CSS:
      resource_obj =
          CSSStyleSheetResourceUtils::CreateForOfflineResource(kurl, origin_url);
      break;
    case OfflineResourceType::CLASSIC_JS:
    case OfflineResourceType::MODULE_JS:
      resource_obj = ScriptResourceUtils::CreateForOfflineResource(
          kurl, origin_url, response,
          resource_type == OfflineResourceType::MODULE_JS);
      break;
    default:
      LOG(ERROR) << "Type is not supported to be added into MemeryCache.";
      return;
  }

  std::vector<char> tempVector(resource.begin(), resource.end());
  base::span<const char> tempSpan(tempVector.begin(), tempVector.end());
  resource_obj->AppendData(tempSpan);
  resource_obj->ResponseReceived(response);
  resource_obj->Finish(base::TimeTicks(),
                       base::SingleThreadTaskRunner::GetCurrentDefault().get());
  resource_obj->SetKeepAliveOn();

  cache->Add(resource_obj);
  return;
}

ResourceResponse WebCacheUtils::GetResourceResponse(
    const KURL& kurl,
    const base::flat_map<std::string, std::string>& response_headers)
{
  ResourceResponse response(kurl);
  response.SetCurrentRequestUrl(kurl);

  for (auto header : response_headers) {
    response.SetHttpHeaderField(AtomicString(header.first.c_str()),
                                AtomicString(header.second.c_str()));
  }

  AtomicString cache_control =
      response.HttpHeaderField(AtomicString("Cache-Control"));
  AtomicString expires = response.HttpHeaderField(AtomicString("Expires"));
  if ((cache_control.empty() && expires.empty()) ||
      response.CacheControlContainsNoCache() ||
      response.CacheControlContainsNoStore()) {
    response.SetHttpHeaderField(AtomicString("Cache-Control"),
                                AtomicString("max-age=86400"));
  }

  AtomicString content_type = response.HttpContentType();
  if (!content_type.empty()) {
    Vector<String> result;
    content_type.GetString().Split(";", result);
    response.SetMimeType(AtomicString(result[0]));
  }

  response.SetHttpHeaderField(AtomicString("Age"), AtomicString("0"));
  response.SetHttpStatusCode(net::HTTP_OK);

  return response;
}
#endif
}  // namespace blink
