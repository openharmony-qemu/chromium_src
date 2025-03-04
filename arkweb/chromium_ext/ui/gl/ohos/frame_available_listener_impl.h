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

#ifndef FRAME_AVAILABLE_LISTENER_IMPL_H
#define FRAME_AVAILABLE_LISTENER_IMPL_H

#include "third_party/ohos_ndk/includes/ohos_adapter/graphic_adapter.h"

namespace OHOS::NWeb {

class FrameAvailableListenerImpl : public FrameAvailableListener {
 public:
  FrameAvailableListenerImpl() = default;

  void* GetContext() override;

  OnFrameAvailableCb GetOnFrameAvailableCb() override;

  void SetContext(void*);

  void SetOnFrameAvailableCb(OnFrameAvailableCb);

 private:
  void* context_ = nullptr;
  OnFrameAvailableCb cb_ = nullptr;
};

}  // namespace OHOS::NWeb

#endif  // FRAME_AVAILABLE_LISTENER_IMPL_H
