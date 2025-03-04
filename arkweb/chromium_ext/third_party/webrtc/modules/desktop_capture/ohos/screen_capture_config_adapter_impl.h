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

#ifndef SCREEN_CAPTURE_CONFIG_ADAPTER_IMPL_H
#define SCREEN_CAPTURE_CONFIG_ADAPTER_IMPL_H

#include "screen_capture_adapter.h"

namespace OHOS::NWeb {

class ScreenCaptureConfigAdapterImpl : public ScreenCaptureConfigAdapter {
 public:
  ScreenCaptureConfigAdapterImpl() = default;

  CaptureModeAdapter GetCaptureMode() override;

  DataTypeAdapter GetDataType() override;

  std::shared_ptr<AudioInfoAdapter> GetAudioInfo() override;

  std::shared_ptr<VideoInfoAdapter> GetVideoInfo() override;

  std::shared_ptr<RecorderInfoAdapter> GetRecorderInfo() override;

  void SetCaptureMode(CaptureModeAdapter mode);

  void SetDataType(DataTypeAdapter type);

  void SetAudioInfo(std::shared_ptr<AudioInfoAdapter> info);

  void SetVideoInfo(std::shared_ptr<VideoInfoAdapter> info);

  void SetRecorderInfo(std::shared_ptr<RecorderInfoAdapter> info);

 private:
  CaptureModeAdapter capture_mode_ = CaptureModeAdapter::CAPTURE_INVAILD;
  DataTypeAdapter data_type_ = DataTypeAdapter::INVAILD_DATA_TYPE;
  std::shared_ptr<AudioInfoAdapter> audio_info_ = nullptr;
  std::shared_ptr<VideoInfoAdapter> video_info_ = nullptr;
  /* should be set, while dataType = CAPTURE_FILE */
  std::shared_ptr<RecorderInfoAdapter> recorder_info_ = nullptr;
};

}  // namespace OHOS::NWeb

#endif  // SCREEN_CAPTURE_CONFIG_ADAPTER_IMPL_H
