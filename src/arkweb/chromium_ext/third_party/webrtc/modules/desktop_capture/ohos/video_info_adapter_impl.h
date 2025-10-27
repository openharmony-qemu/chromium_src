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

#ifndef VIDEO_INFO_ADAPTER_IMPL_H
#define VIDEO_INFO_ADAPTER_IMPL_H

#include "screen_capture_adapter.h"

namespace OHOS::NWeb {

class VideoInfoAdapterImpl : public VideoInfoAdapter {
 public:
  VideoInfoAdapterImpl() = default;

  std::shared_ptr<VideoCaptureInfoAdapter> GetVideoCapInfo() override;

  std::shared_ptr<VideoEncInfoAdapter> GetVideoEncInfo() override;

  void SetVideoCapInfo(std::shared_ptr<VideoCaptureInfoAdapter> info);

  void SetVideoEncInfo(std::shared_ptr<VideoEncInfoAdapter> info);

 private:
  /* Video capture info */
  std::shared_ptr<VideoCaptureInfoAdapter> cap_info_ = nullptr;
  /* Video encoder info */
  std::shared_ptr<VideoEncInfoAdapter> enc_info_ = nullptr;
};

}  // namespace OHOS::NWeb

#endif  // VIDEO_INFO_ADAPTER_IMPL_H
