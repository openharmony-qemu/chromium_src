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

#include "screen_capture_config_adapter_impl.h"

namespace OHOS::NWeb {

CaptureModeAdapter ScreenCaptureConfigAdapterImpl::GetCaptureMode() {
  return capture_mode_;
}

DataTypeAdapter ScreenCaptureConfigAdapterImpl::GetDataType() {
  return data_type_;
}

std::shared_ptr<AudioInfoAdapter>
ScreenCaptureConfigAdapterImpl::GetAudioInfo() {
  return audio_info_;
}

std::shared_ptr<VideoInfoAdapter>
ScreenCaptureConfigAdapterImpl::GetVideoInfo() {
  return video_info_;
}

std::shared_ptr<RecorderInfoAdapter>
ScreenCaptureConfigAdapterImpl::GetRecorderInfo() {
  return recorder_info_;
}

void ScreenCaptureConfigAdapterImpl::SetCaptureMode(CaptureModeAdapter mode) {
  capture_mode_ = mode;
}

void ScreenCaptureConfigAdapterImpl::SetDataType(DataTypeAdapter type) {
  data_type_ = type;
}

void ScreenCaptureConfigAdapterImpl::SetAudioInfo(
    std::shared_ptr<AudioInfoAdapter> info) {
  audio_info_ = info;
}

void ScreenCaptureConfigAdapterImpl::SetVideoInfo(
    std::shared_ptr<VideoInfoAdapter> info) {
  video_info_ = info;
}

void ScreenCaptureConfigAdapterImpl::SetRecorderInfo(
    std::shared_ptr<RecorderInfoAdapter> info) {
  recorder_info_ = info;
}

}  // namespace OHOS::NWeb
