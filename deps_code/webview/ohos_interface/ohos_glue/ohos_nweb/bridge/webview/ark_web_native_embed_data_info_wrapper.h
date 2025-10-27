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

#ifndef ARK_WEB_NATIVE_EMBED_DATA_INFO_WRAPPER_H_
#define ARK_WEB_NATIVE_EMBED_DATA_INFO_WRAPPER_H_
#pragma once

#include "include/nweb_handler.h"
#include "ohos_nweb/include/ark_web_native_embed_data_info.h"

namespace OHOS::ArkWeb {

using ArkWebNativeEmbedStatus = OHOS::NWeb::NativeEmbedStatus;

class ArkWebNativeEmbedDataInfoWrapper : public OHOS::NWeb::NWebNativeEmbedDataInfo {
public:
    ArkWebNativeEmbedDataInfoWrapper(ArkWebRefPtr<ArkWebNativeEmbedDataInfo> ark_web_native_embed_data_info);
    ~ArkWebNativeEmbedDataInfoWrapper() = default;

    ArkWebNativeEmbedStatus GetStatus() override;

    std::string GetEmbedId() override;

    std::string GetSurfaceId() override;

    std::shared_ptr<OHOS::NWeb::NWebNativeEmbedInfo> GetNativeEmbedInfo() override;

private:
    ArkWebRefPtr<ArkWebNativeEmbedDataInfo> ark_web_native_embed_data_info_;
};

} // namespace OHOS::ArkWeb

#endif // ARK_WEB_NATIVE_EMBED_DATA_INFO_WRAPPER_H_
