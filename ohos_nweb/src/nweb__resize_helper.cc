/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "nweb_resize_helper.h"

namespace OHOS::NWeb {

NWebResizeHelper& NWebResizeHelper::GetInstance() {
    static NWebResizeHelper instance;
    return instance;
}

void NWebResizeHelper::SetResizeStartTimeStamp() {
    auto time = std::chrono::high_resolution_clock::now().time_since_epoch();
    resize_start_timestamp_ = std::chrono::duration_cast<std::chrono::milliseconds>(time).count();
}

void NWebResizeHelper::CaculateResizeTime() {
    auto time = std::chrono::high_resolution_clock::now().time_since_epoch();
    int64_t complete_time = std::chrono::duration_cast<std::chrono::milliseconds>(time).count();
    resize_time_ = complete_time - resize_start_timestamp_;
}

int64_t NWebResizeHelper::GetResizeStartTimeStamp() {
    return resize_start_timestamp_;
}

int64_t NWebResizeHelper::GetResizeTime() {
    return resize_time_;
}

void NWebResizeHelper::RefreshStartTimeStamp() {
    resize_start_timestamp_ = 0;
}

int64_t NWebResizeHelper::GetResizeAdjustValue(uint32_t target_length,
                                               int64_t resize_pre_length,
                                               bool isHeight) {
  int64_t result = 0;
  if (isHeight) {
    if (resize_last_height_ == 0) {
        resize_start_timestamp_ = 0;
        return target_length;
    }
    result = target_length + (target_length - resize_last_height_) + resize_pre_length;
    if (result < 0) {
        result = target_length;
    }
    if (result < resize_last_height_) {
        result = resize_last_height_;
    }
  }else {
    if (resize_last_width_ == 0) {
        resize_start_timestamp_ = 0;
        return target_length;
    }
    result = target_length + (target_length - resize_last_width_) + resize_pre_length;
    if (result < 0) {
        result = target_length;
    }
    if (result < resize_last_width_) {
        result = resize_last_width_;
    }
  }
  return result;
}

void NWebResizeHelper::RefreshParam() {
  resize_start_timestamp_ = 0;
  resize_time_ = 0L;
  resize_last_height_ = 0L;
  resize_last_width_ = 0L;
}

void NWebResizeHelper::SetDragResizeStart(bool is_start) {
    is_drag_resize_start_ = is_start;
}

bool NWebResizeHelper::IsDragResizeStart() {
    return is_drag_resize_start_;
}

void NWebResizeHelper::SetResizeHeightAndWidth(int64_t height, int64_t width) {
    LOG(DEBUG) << "===== set resize last size";
    resize_last_height_ = height;
    resize_last_width_ = width;
    LOG(DEBUG) << "====== resize_last_width_ ======= " << resize_last_width_ << "=== resize_last_height_ ===" << resize_last_height_;
}

}
