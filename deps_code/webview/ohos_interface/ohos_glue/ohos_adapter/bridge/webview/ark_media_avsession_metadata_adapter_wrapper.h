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

#ifndef ARK_MEDIA_AVSESSION_METADATA_ADAPTER_WRAPPER_H
#define ARK_MEDIA_AVSESSION_METADATA_ADAPTER_WRAPPER_H
#pragma once

#include "media_avsession_adapter.h"
#include "ohos_adapter/include/ark_media_avsession_adapter.h"

namespace OHOS::ArkWeb {

class ArkMediaAVSessionMetadataAdapterWrapper : public NWeb::MediaAVSessionMetadataAdapter {
public:
    explicit ArkMediaAVSessionMetadataAdapterWrapper(ArkWebRefPtr<ArkMediaAVSessionMetadataAdapter>);

    void SetTitle(const std::string& title) override;

    std::string GetTitle() override;

    void SetArtist(const std::string& artist) override;

    std::string GetArtist() override;

    void SetAlbum(const std::string& album) override;

    std::string GetAlbum() override;

    void SetImageUrl(const std::string& imageUrl) override;

    std::string GetImageUrl() override;
private:
    ArkWebRefPtr<ArkMediaAVSessionMetadataAdapter> ctocpp_;
};

} // namespace OHOS::ArkWeb

#endif // ARK_MEDIA_AVSESSION_METADATA_ADAPTER_WRAPPER_H
