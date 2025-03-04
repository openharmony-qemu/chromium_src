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

#ifndef RECORDER_INFO_ADAPTER_IMPL_H
#define RECORDER_INFO_ADAPTER_IMPL_H

#include "screen_capture_adapter.h"

namespace OHOS::NWeb {

class RecorderInfoAdapterImpl : public RecorderInfoAdapter {
 public:
  RecorderInfoAdapterImpl() = default;

  std::string GetUrl() override;

  ContainerFormatTypeAdapter GetFileFormat() override;

  void SetUrl(std::string url);

  void SetFileFormat(ContainerFormatTypeAdapter format);

 private:
  /* Recorder file url */
  std::string url_ = "";
  /* Recorder file format */
  ContainerFormatTypeAdapter file_format_ =
      ContainerFormatTypeAdapter::CFT_MPEG_4A_TYPE;
};

}  // namespace OHOS::NWeb

#endif  // RECORDER_INFO_ADAPTER_IMPL_H
