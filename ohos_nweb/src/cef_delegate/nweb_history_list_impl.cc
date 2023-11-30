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

#include "nweb_history_list_impl.h"
#include "nweb.h"

#include "base/logging.h"
#include "include/core/SkBitmap.h"

using namespace OHOS::NWeb;
namespace {
ImageColorType TransformSkColorType(int color_type) {
  switch (color_type) {
    case kRGBA_8888_SkColorType:
      return ImageColorType::COLOR_TYPE_RGBA_8888;
    case kBGRA_8888_SkColorType:
      return ImageColorType::COLOR_TYPE_BGRA_8888;
    default:
      return ImageColorType::COLOR_TYPE_UNKNOWN;
  }
}

ImageAlphaType TransformSkAlphaType(int alpha_type) {
  switch (alpha_type) {
    case kOpaque_SkAlphaType:
      return ImageAlphaType::ALPHA_TYPE_OPAQUE;
    case kPremul_SkAlphaType:
      return ImageAlphaType::ALPHA_TYPE_PREMULTIPLIED;
    case kUnpremul_SkAlphaType:
      return ImageAlphaType::ALPHA_TYPE_POSTMULTIPLIED;
    default:
      return ImageAlphaType::ALPHA_TYPE_UNKNOWN;
  }
}
}  // namespace

namespace OHOS::NWeb {
NWebHistoryItemImpl::NWebHistoryItemImpl(CefRefPtr<CefNavigationEntry> entry)
    : entry_(entry) {}

std::string NWebHistoryItemImpl::GetHistoryRawUrl() {
  if (!entry_ || !entry_->IsValid()) {
    return std::string();
  }
  return entry_->GetOriginalURL().ToString().empty()
             ? entry_->GetDisplayURL().ToString()
             : entry_->GetOriginalURL().ToString();
}

std::string NWebHistoryItemImpl::GetHistoryTitle() {
  return (entry_ != nullptr && entry_->IsValid())
             ? entry_->GetTitle().ToString()
             : std::string();
}

std::string NWebHistoryItemImpl::GetHistoryUrl() {
  return (entry_ != nullptr && entry_->IsValid()) ? entry_->GetURL().ToString()
                                                  : std::string();
}

bool NWebHistoryItemImpl::GetFavicon(void** data,
                                     int& width,
                                     int& height,
                                     ImageColorType& colorType,
                                     ImageAlphaType& alphaType) {
  if (!entry_ || !entry_->IsValid()) {
    return false;
  }
  int color, alpha;
  bool result = entry_->GetFavicon(data, color, alpha, width, height);
  colorType = TransformSkColorType(color);
  alphaType = TransformSkAlphaType(alpha);
  return result;
}

int32_t NWebHistoryListImpl::GetCurrentIndex() {
  return current_index_;
}

std::shared_ptr<NWebHistoryItem> NWebHistoryListImpl::GetItem(int32_t index) {
  if (index < 0 || index >= item_list_.size()) {
    return nullptr;
  }
  return item_list_[index];
}

int32_t NWebHistoryListImpl::GetListSize() {
  return item_list_.size();
}

void NWebHistoryListImpl::AddHistoryItem(CefRefPtr<CefNavigationEntry> entry) {
  if (entry) {
    item_list_.push_back(std::make_shared<NWebHistoryItemImpl>(entry));
  }
}

void NWebHistoryListImpl::SetCurrentIndex(int32_t index) {
  current_index_ = index;
}
}  // namespace OHOS::NWeb
