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

#include "frame_available_listener_impl.h"

namespace OHOS::NWeb {

FrameAvailableListenerImpl::FrameAvailableListenerImpl(
  raw_ptr<OhosNativeImageAdapter> adapter) : ohos_native_image_adapter_(adapter) {}

void FrameAvailableListenerImpl::OnFrameAvailableListener() {
  if (!ohos_native_image_adapter_) {
    return;
  }
  ohos_native_image_adapter_->OnFrameAvailableListener();
}

}  // namespace OHOS::NWeb
