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

#include "ui/base/clipboard/ohos/clip_board_image_data_adapter_impl.h"

namespace OHOS::NWeb {

ClipBoardImageColorType ClipBoardImageDataAdapterImpl::GetColorType() {
  return color_type_;
}

ClipBoardImageAlphaType ClipBoardImageDataAdapterImpl::GetAlphaType() {
  return alpha_type_;
}

uint32_t* ClipBoardImageDataAdapterImpl::GetData() {
  return data_;
}

size_t ClipBoardImageDataAdapterImpl::GetDataSize() {
  return data_size_;
}

size_t ClipBoardImageDataAdapterImpl::GetRowBytes() {
  return row_bytes_;
}

int32_t ClipBoardImageDataAdapterImpl::GetWidth() {
  return width_;
}

int32_t ClipBoardImageDataAdapterImpl::GetHeight() {
  return height_;
}

void ClipBoardImageDataAdapterImpl::SetColorType(
    ClipBoardImageColorType color) {
  color_type_ = color;
}

void ClipBoardImageDataAdapterImpl::SetAlphaType(
    ClipBoardImageAlphaType alpha) {
  alpha_type_ = alpha;
}

void ClipBoardImageDataAdapterImpl::SetData(uint32_t* data) {
  data_ = data;
}

void ClipBoardImageDataAdapterImpl::SetDataSize(size_t size) {
  data_size_ = size;
}

void ClipBoardImageDataAdapterImpl::SetRowBytes(size_t rowBytes) {
  row_bytes_ = rowBytes;
}

void ClipBoardImageDataAdapterImpl::SetWidth(int32_t width) {
  width_ = width;
}

void ClipBoardImageDataAdapterImpl::SetHeight(int32_t height) {
  height_ = height;
}

}  // namespace OHOS::NWeb
