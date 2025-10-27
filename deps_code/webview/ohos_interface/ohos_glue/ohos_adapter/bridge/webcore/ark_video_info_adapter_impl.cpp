/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "ohos_adapter/bridge/ark_video_info_adapter_impl.h"

#include "ohos_adapter/bridge/ark_video_capture_info_adapter_impl.h"
#include "ohos_adapter/bridge/ark_video_enc_info_adapter_impl.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {
ArkVideoInfoAdapterImpl::ArkVideoInfoAdapterImpl(std::shared_ptr<OHOS::NWeb::VideoInfoAdapter> ref) : real_(ref) {}

ArkWebRefPtr<ArkVideoCaptureInfoAdapter> ArkVideoInfoAdapterImpl::GetVideoCapInfo()
{
    std::shared_ptr<OHOS::NWeb::VideoCaptureInfoAdapter> info = real_->GetVideoCapInfo();
    if (CHECK_SHARED_PTR_IS_NULL(info)) {
        return nullptr;
    }
    return new ArkVideoCaptureInfoAdapterImpl(info);
}

ArkWebRefPtr<ArkVideoEncInfoAdapter> ArkVideoInfoAdapterImpl::GetVideoEncInfo()
{
    std::shared_ptr<OHOS::NWeb::VideoEncInfoAdapter> info = real_->GetVideoEncInfo();
    if (CHECK_SHARED_PTR_IS_NULL(info)) {
        return nullptr;
    }
    return new ArkVideoEncInfoAdapterImpl(info);
}

} // namespace OHOS::ArkWeb
