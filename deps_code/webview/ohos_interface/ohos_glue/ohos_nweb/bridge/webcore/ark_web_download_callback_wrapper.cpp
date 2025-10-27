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

#include "ohos_nweb/bridge/ark_web_download_callback_wrapper.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkWebDownloadCallbackWrapper::ArkWebDownloadCallbackWrapper(
    ArkWebRefPtr<ArkWebDownloadCallback> ark_web_download_callback)
    : ark_web_download_callback_(ark_web_download_callback)
{}

void ArkWebDownloadCallbackWrapper::OnDownloadStart(const std::string& url, const std::string& user_agent,
    const std::string& content_disposition, const std::string& mime_type, long content_length)
{
    ArkWebString stUrl = ArkWebStringClassToStruct(url);
    ArkWebString stMimeType = ArkWebStringClassToStruct(mime_type);
    ArkWebString stUserAgent = ArkWebStringClassToStruct(user_agent);
    ArkWebString stContentDisposition = ArkWebStringClassToStruct(content_disposition);

    ark_web_download_callback_->OnDownloadStart(stUrl, stUserAgent, stContentDisposition, stMimeType, content_length);

    ArkWebStringStructRelease(stUrl);
    ArkWebStringStructRelease(stMimeType);
    ArkWebStringStructRelease(stUserAgent);
    ArkWebStringStructRelease(stContentDisposition);
}

} // namespace OHOS::ArkWeb
