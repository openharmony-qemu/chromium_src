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

#include "arkweb/chromium_ext/ui/gl/ohos/ohos_native_image.h"

namespace OHOS::NWeb {

FrameAvailableListenerImpl::FrameAvailableListenerImpl(
        base::RepeatingClosure frame_available_cb)
        : frame_available_cb_(std::move(frame_available_cb)) {}

void FrameAvailableListenerImpl::OnFrameAvailableListener() {
  // Immediately signal the wait coordinator on the callback thread to avoid timeout.
  // This is critical for timely wakeup of WaitForFrameAvailable() which has a
  // short timeout window (5-20ms). Posting to another thread introduces scheduling
  // delays that can cause timeouts.
  //
  // base::WaitableEvent::Signal() is thread-safe and can be called from any thread.
  if (frame_available_cb_) {
    frame_available_cb_.Run();
  }
}
}  // namespace OHOS::NWeb
