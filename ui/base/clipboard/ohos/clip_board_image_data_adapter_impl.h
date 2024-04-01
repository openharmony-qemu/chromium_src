/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef CLIP_BOARD_IMAGE_DATA_ADAPTER_IMPL_H
#define CLIP_BOARD_IMAGE_DATA_ADAPTER_IMPL_H

#include "pasteboard_client_adapter.h"

namespace OHOS::NWeb {

class ClipBoardImageDataAdapterImpl : public ClipBoardImageDataAdapter {
 public:
  ClipBoardImageDataAdapterImpl() = default;

  ClipBoardImageColorType GetColorType() override;

  ClipBoardImageAlphaType GetAlphaType() override;

  uint32_t* GetData() override;

  size_t GetDataSize() override;

  size_t GetRowBytes() override;

  int32_t GetWidth() override;

  int32_t GetHeight() override;

  void SetColorType(ClipBoardImageColorType color) override;

  void SetAlphaType(ClipBoardImageAlphaType alpha) override;

  void SetData(uint32_t* data) override;

  void SetDataSize(size_t size) override;

  void SetRowBytes(size_t rowBytes) override;

  void SetWidth(int32_t width) override;

  void SetHeight(int32_t height) override;

 private:
  ClipBoardImageColorType color_type_ =
      ClipBoardImageColorType::COLOR_TYPE_UNKNOWN;
  ClipBoardImageAlphaType alpha_type_ =
      ClipBoardImageAlphaType::ALPHA_TYPE_UNKNOWN;
  uint32_t* data_ = nullptr;
  size_t data_size_ = 0;
  size_t row_bytes_ = 0;
  int32_t width_ = 0;
  int32_t height_ = 0;
};

}  // namespace OHOS::NWeb

#endif  // CLIP_BOARD_IMAGE_DATA_ADAPTER_IMPL_H
