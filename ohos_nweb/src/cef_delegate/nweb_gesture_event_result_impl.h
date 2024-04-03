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

#ifndef NWEB_GESTURE_EVENT_RESULT_IMPL_H
#define NWEB_GESTURE_EVENT_RESULT_IMPL_H

#include "cef/include/cef_browser.h"
#include "nweb_gesture_event_result.h"

namespace OHOS::NWeb {
class NWebGestureEventResultImpl : public NWebGestureEventResult {
 public:
  NWebGestureEventResultImpl() = default;
  explicit NWebGestureEventResultImpl(CefRefPtr<CefGestureEventCallback> callback);
  ~NWebGestureEventResultImpl() = default;
  void SetGestureEventResult(bool result) override;

 private:
  CefRefPtr<CefGestureEventCallback> callback_;
  bool has_put_ = false;
};
}  // namespace OHOS::NWeb

#endif
