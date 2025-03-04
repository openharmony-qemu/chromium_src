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

#include "video_info_adapter_impl.h"

namespace OHOS::NWeb {

std::shared_ptr<VideoCaptureInfoAdapter>
VideoInfoAdapterImpl::GetVideoCapInfo() {
  return cap_info_;
}

std::shared_ptr<VideoEncInfoAdapter> VideoInfoAdapterImpl::GetVideoEncInfo() {
  return enc_info_;
}

void VideoInfoAdapterImpl::SetVideoCapInfo(
    std::shared_ptr<VideoCaptureInfoAdapter> info) {
  cap_info_ = info;
}

void VideoInfoAdapterImpl::SetVideoEncInfo(
    std::shared_ptr<VideoEncInfoAdapter> info) {
  enc_info_ = info;
}
}  // namespace OHOS::NWeb
