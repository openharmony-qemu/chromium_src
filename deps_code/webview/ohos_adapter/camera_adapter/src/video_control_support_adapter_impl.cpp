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

#include "video_control_support_adapter_impl.h"

namespace OHOS::NWeb {

bool VideoControlSupportAdapterImpl::GetPan()
{
    return pan_;
}

bool VideoControlSupportAdapterImpl::GetTilt()
{
    return tilt_;
}

bool VideoControlSupportAdapterImpl::GetZoom()
{
    return zoom_;
}

void VideoControlSupportAdapterImpl::SetPan(bool value)
{
    pan_ = value;
}

void VideoControlSupportAdapterImpl::SetTilt(bool value)
{
    tilt_ = value;
}

void VideoControlSupportAdapterImpl::SetZoom(bool value)
{
    zoom_ = value;
}

} // namespace OHOS::NWeb
