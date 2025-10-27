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

#include "ohos_adapter/bridge/ark_video_capture_info_adapter_impl.h"

namespace OHOS::ArkWeb {
ArkVideoCaptureInfoAdapterImpl::ArkVideoCaptureInfoAdapterImpl(std::shared_ptr<OHOS::NWeb::VideoCaptureInfoAdapter> ref)
    : real_(ref)
{}

uint64_t ArkVideoCaptureInfoAdapterImpl::GetDisplayId()
{
    return real_->GetDisplayId();
}

ArkWebInt32List ArkVideoCaptureInfoAdapterImpl::GetTaskIDs()
{
    std::list<int32_t> ids = real_->GetTaskIDs();
    return ArkWebBasicListClassToStruct<int32_t, ArkWebInt32List>(ids);
}

int32_t ArkVideoCaptureInfoAdapterImpl::GetVideoFrameWidth()
{
    return real_->GetVideoFrameWidth();
}

int32_t ArkVideoCaptureInfoAdapterImpl::GetVideoFrameHeight()
{
    return real_->GetVideoFrameHeight();
}

int32_t ArkVideoCaptureInfoAdapterImpl::GetVideoSourceType()
{
    return (int32_t)real_->GetVideoSourceType();
}

} // namespace OHOS::ArkWeb
