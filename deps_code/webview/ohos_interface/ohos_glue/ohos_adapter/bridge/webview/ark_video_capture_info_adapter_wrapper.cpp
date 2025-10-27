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

#include "ohos_adapter/bridge/ark_video_capture_info_adapter_wrapper.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkVideoCaptureInfoAdapterWrapper::ArkVideoCaptureInfoAdapterWrapper(ArkWebRefPtr<ArkVideoCaptureInfoAdapter> ref)
    : ctocpp_(ref)
{}

uint64_t ArkVideoCaptureInfoAdapterWrapper::GetDisplayId()
{
    return ctocpp_->GetDisplayId();
}

std::list<int32_t> ArkVideoCaptureInfoAdapterWrapper::GetTaskIDs()
{
    ArkWebInt32List ids = ctocpp_->GetTaskIDs();
    std::list<int32_t> result = ArkWebBasicListStructToClass<int32_t, ArkWebInt32List>(ids);
    ArkWebBasicListStructRelease<ArkWebInt32List>(ids);
    return result;
}

int32_t ArkVideoCaptureInfoAdapterWrapper::GetVideoFrameWidth()
{
    return ctocpp_->GetVideoFrameWidth();
}

int32_t ArkVideoCaptureInfoAdapterWrapper::GetVideoFrameHeight()
{
    return ctocpp_->GetVideoFrameHeight();
}

NWeb::VideoSourceTypeAdapter ArkVideoCaptureInfoAdapterWrapper::GetVideoSourceType()
{
    return (NWeb::VideoSourceTypeAdapter)ctocpp_->GetVideoSourceType();
}

} // namespace OHOS::ArkWeb
