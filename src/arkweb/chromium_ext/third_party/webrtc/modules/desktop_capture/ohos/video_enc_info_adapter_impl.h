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

#ifndef VIDEO_ENC_INFO_ADAPTER_IMPL_H
#define VIDEO_ENC_INFO_ADAPTER_IMPL_H

#include "screen_capture_adapter.h"

namespace OHOS::NWeb {

class VideoEncInfoAdapterImpl : public VideoEncInfoAdapter {
 public:
  VideoEncInfoAdapterImpl() = default;

  VideoCodecFormatAdapter GetVideoCodecFormat() override;

  int32_t GetVideoBitrate() override;

  int32_t GetVideoFrameRate() override;

  void SetVideoCodecFormat(VideoCodecFormatAdapter format);

  void SetVideoBitrate(int32_t bitrate);

  void SetVideoFrameRate(int32_t frameRate);

 private:
  /* Video encoder format */
  VideoCodecFormatAdapter codec_ = VideoCodecFormatAdapter::VIDEO_DEFAULT;
  /* Video encoder bitrate */
  int32_t bitrate_ = 0;
  /* Video encoder frame rate */
  int32_t frame_rate_ = 0;
};

}  // namespace OHOS::NWeb

#endif  // VIDEO_ENC_INFO_ADAPTER_IMPL_H
