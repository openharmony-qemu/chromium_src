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

#ifndef VIDEO_CAPTURE_INFO_ADAPTER_IMPL_H
#define VIDEO_CAPTURE_INFO_ADAPTER_IMPL_H

#include "screen_capture_adapter.h"

namespace OHOS::NWeb {

class VideoCaptureInfoAdapterImpl : public VideoCaptureInfoAdapter {
 public:
  VideoCaptureInfoAdapterImpl() = default;

  uint64_t GetDisplayId() override;

  std::list<int32_t> GetTaskIDs() override;

  int32_t GetVideoFrameWidth() override;

  int32_t GetVideoFrameHeight() override;

  VideoSourceTypeAdapter GetVideoSourceType() override;

  void SetDisplayId(uint64_t id);

  void SetTaskIDs(std::list<int32_t> ids);

  void SetVideoFrameWidth(int32_t width);

  void SetVideoFrameHeight(int32_t height);

  void SetVideoSourceType(VideoSourceTypeAdapter type);

 private:
  /* Display id, should be set while captureMode = CAPTURE_SPECIFIED_SCREEN */
  uint64_t display_id_ = 0;
  /* The ids of mission, should be set while captureMode =
   * CAPTURE_SPECIFIED_WINDOW */
  std::list<int32_t> task_ids_;
  /* Video frame width of avscreeencapture */
  int32_t video_frame_width_ = 0;
  /* Video frame height of avscreeencapture */
  int32_t video_frame_height_ = 0;
  /* Video source type of avscreeencapture */
  VideoSourceTypeAdapter video_source_ =
      VideoSourceTypeAdapter::VIDEO_SOURCE_SURFACE_RGBA;
};

}  // namespace OHOS::NWeb

#endif  // VIDEO_CAPTURE_INFO_ADAPTER_IMPL_H
