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

#ifndef NWEB_RESIZE_HELPER_H
#define NWEB_RESIZE_HELPER_H

#include <chrono>
#include <ctime>
#include "base/logging.h"
#include <string>

namespace OHOS::NWeb {
class NWebResizeHelper {
 public:
  static NWebResizeHelper& GetInstance();
  void SetResizeStartTimeStamp();
  void CaculateResizeTime();
  int64_t GetResizeStartTimeStamp();
  int64_t GetResizeTime();
  void RefreshStartTimeStamp();
  void RefreshParam();
  int64_t GetResizeAdjustValue(uint32_t target_length,
                              int64_t resize_pre_length,
                              bool isHeight);
  void SetResizeHeightAndWidth(int64_t height, int64_t width);
  void SetDragResizeStart(bool is_start);
  bool IsDragResizeStart();

private:
  int64_t resize_start_timestamp_ = 0;
  int64_t resize_time_ = 0L;
  int64_t resize_last_height_ = 0L;
  int64_t resize_last_width_ = 0L;
  bool is_drag_resize_start_ = false;

private:
  NWebResizeHelper() = default;
  ~NWebResizeHelper() = default;
};
}  // namespace OHOS::NWeb

#endif // NWEB_RESIZE_HELPER_H_
