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

#include "video_capture_info_adapter_impl.h"

namespace OHOS::NWeb {

uint64_t VideoCaptureInfoAdapterImpl::GetDisplayId() {
  return display_id_;
}

std::list<int32_t> VideoCaptureInfoAdapterImpl::GetTaskIDs() {
  return task_ids_;
}

int32_t VideoCaptureInfoAdapterImpl::GetVideoFrameWidth() {
  return video_frame_width_;
}

int32_t VideoCaptureInfoAdapterImpl::GetVideoFrameHeight() {
  return video_frame_height_;
}

VideoSourceTypeAdapter VideoCaptureInfoAdapterImpl::GetVideoSourceType() {
  return video_source_;
}

void VideoCaptureInfoAdapterImpl::SetDisplayId(uint64_t id) {
  display_id_ = id;
}

void VideoCaptureInfoAdapterImpl::SetTaskIDs(std::list<int32_t> ids) {
  task_ids_ = ids;
}

void VideoCaptureInfoAdapterImpl::SetVideoFrameWidth(int32_t width) {
  video_frame_width_ = width;
}

void VideoCaptureInfoAdapterImpl::SetVideoFrameHeight(int32_t height) {
  video_frame_height_ = height;
}

void VideoCaptureInfoAdapterImpl::SetVideoSourceType(
    VideoSourceTypeAdapter type) {
  video_source_ = type;
}
}  // namespace OHOS::NWeb
