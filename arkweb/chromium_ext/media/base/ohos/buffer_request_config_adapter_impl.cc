// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

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
