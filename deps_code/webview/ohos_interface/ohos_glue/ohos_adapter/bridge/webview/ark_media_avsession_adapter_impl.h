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

#ifndef ARK_MEDIA_AVSESSION_ADAPTER_IMPL_H
#define ARK_MEDIA_AVSESSION_ADAPTER_IMPL_H
#pragma once

#include "media_avsession_adapter.h"
#include "ohos_adapter/include/ark_media_avsession_adapter.h"

namespace OHOS::ArkWeb {

class ArkMediaAVSessionAdapterImpl : public ArkMediaAVSessionAdapter {
public:
    explicit ArkMediaAVSessionAdapterImpl(std::shared_ptr<OHOS::NWeb::MediaAVSessionAdapter>);

    bool CreateAVSession(int32_t type) override;

    void DestroyAVSession() override;

    bool RegistCallback(ArkWebRefPtr<ArkMediaAVSessionCallbackAdapter> callbackAdapter) override;

    bool IsActivated() override;

    bool Activate() override;

    void DeActivate() override;

    void SetMetadata(const ArkWebRefPtr<ArkMediaAVSessionMetadataAdapter> metadata) override;

    void SetPlaybackState(int32_t state) override;

    void SetPlaybackPosition(const ArkWebRefPtr<ArkMediaAVSessionPositionAdapter> position) override;

private:
    std::shared_ptr<OHOS::NWeb::MediaAVSessionAdapter> real_;

    IMPLEMENT_REFCOUNTING(ArkMediaAVSessionAdapterImpl);
};

} // namespace OHOS::ArkWeb

#endif // ARK_MEDIA_AVSESSION_ADAPTER_IMPL_H
