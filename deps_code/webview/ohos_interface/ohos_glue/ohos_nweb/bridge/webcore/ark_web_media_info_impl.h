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

#ifndef ARK_WEB_MEDIA_INFO_IMPL_H_
#define ARK_WEB_MEDIA_INFO_IMPL_H_
#pragma once

#include "include/nweb_native_media_player.h"
#include "ohos_nweb/include/ark_web_media_info.h"

namespace OHOS::ArkWeb {

class ArkWebMediaInfoImpl : public ArkWebMediaInfo {
    IMPLEMENT_REFCOUNTING(ArkWebMediaInfoImpl);

public:
    ArkWebMediaInfoImpl(std::shared_ptr<OHOS::NWeb::NWebMediaInfo> nweb_media_info);
    ~ArkWebMediaInfoImpl() = default;

    int GetPreload() override;

    bool GetIsMuted() override;

    ArkWebString GetEmbedId() override;

    ArkWebString GetPosterUrl() override;

    int GetMediaType() override;

    bool GetIsControlsShown() override;

    ArkWebStringVector GetControls() override;

    ArkWebStringMap GetHeaders() override;

    ArkWebStringMap GetAttributes() override;

    ArkWebMediaSourceInfoVector GetSourceInfos() override;

    ArkWebRefPtr<ArkWebNativeMediaPlayerSurfaceInfo> GetSurfaceInfo() override;

private:
    std::shared_ptr<OHOS::NWeb::NWebMediaInfo> nweb_media_info_;
};

} // namespace OHOS::ArkWeb

#endif // ARK_WEB_MEDIA_INFO_IMPL_H_
