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

#ifndef ARK_MEDIA_AVSESSION_ADAPTER_WRAPPER_H
#define ARK_MEDIA_AVSESSION_ADAPTER_WRAPPER_H
#pragma once

#include "media_avsession_adapter.h"
#include "ohos_adapter/include/ark_media_avsession_adapter.h"

namespace OHOS::ArkWeb {

class ArkMediaAVSessionAdapterWrapper : public OHOS::NWeb::MediaAVSessionAdapter {
public:
    explicit ArkMediaAVSessionAdapterWrapper(ArkWebRefPtr<ArkMediaAVSessionAdapter>);

    bool CreateAVSession(NWeb::MediaAVSessionType type) override;

    void DestroyAVSession() override;

    bool RegistCallback(std::shared_ptr<NWeb::MediaAVSessionCallbackAdapter> callbackAdapter) override;

    bool IsActivated() override;

    bool Activate() override;

    void DeActivate() override;

    void SetMetadata(const std::shared_ptr<NWeb::MediaAVSessionMetadataAdapter> metadata) override;

    void SetPlaybackState(NWeb::MediaAVSessionPlayState state) override;

    void SetPlaybackPosition(const std::shared_ptr<NWeb::MediaAVSessionPositionAdapter> position) override;

private:
    ArkWebRefPtr<ArkMediaAVSessionAdapter> ctocpp_;
};

} // namespace OHOS::ArkWeb

#endif // ARK_MEDIA_AVSESSION_ADAPTER_WRAPPER_H
