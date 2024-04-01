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

#include "buffer_request_config_adapter_impl.h"

namespace OHOS::NWeb {

int32_t BufferRequestConfigAdapterImpl::GetWidth() {
  return width_;
}

int32_t BufferRequestConfigAdapterImpl::GetHeight() {
  return height_;
}

int32_t BufferRequestConfigAdapterImpl::GetStrideAlignment() {
  return stride_alignment_;
}

int32_t BufferRequestConfigAdapterImpl::GetFormat() {
  return format_;
}

uint64_t BufferRequestConfigAdapterImpl::GetUsage() {
  return usage_;
}

int32_t BufferRequestConfigAdapterImpl::GetTimeout() {
  return timeout_;
}

ColorGamutAdapter BufferRequestConfigAdapterImpl::GetColorGamut() {
  return color_gamut_;
}

TransformTypeAdapter BufferRequestConfigAdapterImpl::GetTransformType() {
  return transform_type_;
}

void BufferRequestConfigAdapterImpl::SetWidth(int32_t width) {
  width_ = width;
}

void BufferRequestConfigAdapterImpl::SetHeight(int32_t height) {
  height_ = height;
}

void BufferRequestConfigAdapterImpl::SetStrideAlignment(int32_t alignment) {
  stride_alignment_ = alignment;
}

void BufferRequestConfigAdapterImpl::SetFormat(int32_t format) {
  format_ = format;
}

void BufferRequestConfigAdapterImpl::SetUsage(uint64_t usage) {
  usage_ = usage;
}

void BufferRequestConfigAdapterImpl::SetTimeout(int32_t timeout) {
  timeout_ = timeout;
}

void BufferRequestConfigAdapterImpl::SetColorGamut(ColorGamutAdapter gamut) {
  color_gamut_ = gamut;
}

void BufferRequestConfigAdapterImpl::SetTransformType(
    TransformTypeAdapter type) {
  transform_type_ = type;
}

}  // namespace OHOS::NWeb
