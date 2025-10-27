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

#ifndef ARK_WEB_MEDIA_INFO_WRAPPER_H_
#define ARK_WEB_MEDIA_INFO_WRAPPER_H_
#pragma once

#include "include/nweb_native_media_player.h"
#include "ohos_nweb/include/ark_web_media_info.h"

namespace OHOS::ArkWeb {

using ArkWebPreload = OHOS::NWeb::Preload;
using ArkWebMediaType = OHOS::NWeb::MediaType;

class ArkWebMediaInfoWrapper : public OHOS::NWeb::NWebMediaInfo {
public:
    ArkWebMediaInfoWrapper(ArkWebRefPtr<ArkWebMediaInfo> ark_web_media_info);
    ~ArkWebMediaInfoWrapper() = default;

    ArkWebPreload GetPreload() override;

    bool GetIsMuted() override;

    std::string GetEmbedId() override;

    std::string GetPosterUrl() override;

    ArkWebMediaType GetMediaType() override;

    bool GetIsControlsShown() override;

    std::vector<std::string> GetControls() override;

    std::map<std::string, std::string> GetHeaders() override;

    std::map<std::string, std::string> GetAttributes() override;

    std::vector<std::shared_ptr<OHOS::NWeb::NWebMediaSourceInfo>> GetSourceInfos() override;

    std::shared_ptr<OHOS::NWeb::NWebNativeMediaPlayerSurfaceInfo> GetSurfaceInfo() override;

private:
    ArkWebRefPtr<ArkWebMediaInfo> ark_web_media_info_;
};

} // namespace OHOS::ArkWeb

#endif // ARK_WEB_MEDIA_INFO_WRAPPER_H_
