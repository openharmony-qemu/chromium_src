/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#ifndef NWEB_CONTENT_CHANGE_DETECTION_H
#define NWEB_CONTENT_CHANGE_DETECTION_H

#include "nweb_delegate.h"

namespace OHOS::NWeb {

class NWebContentChangeDetection {
 public:
  explicit NWebContentChangeDetection(base::WeakPtr<NWebDelegate> nweb_delegate);
  void SetContentChangeDetectionEnable(bool enable);
  void SetContentChangeDetectionConfig(int32_t min_report_time, float text_content_ratio);

 private:
  void StartDetection();
  void StopDetection();

  bool enable_ = false;
  int32_t min_report_time_ = 100;
  float text_content_ratio_ = 0.15f;

  base::WeakPtr<NWebDelegate> nweb_delegate_;
  base::WeakPtrFactory<NWebContentChangeDetection> weak_factory_{this};
};

} // namespace OHOS::NWeb

#endif //NWEB_CONTENT_CHANGE_DETECTION_H