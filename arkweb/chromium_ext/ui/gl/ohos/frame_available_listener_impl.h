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

#include "arkweb/ohos_adapter_ndk/interfaces/graphic_adapter.h"
#include "arkweb/ohos_adapter_ndk/interfaces/ohos_adapter_helper.h"
#include "base/memory/scoped_refptr.h"
#include "base/memory/weak_ptr.h"
#include "base/task/single_thread_task_runner.h"

namespace gl {
  class OhosNativeImage;
}

namespace OHOS::NWeb {

class FrameAvailableListenerImpl : public FrameAvailableListener {
  public:
    FrameAvailableListenerImpl(base::RepeatingClosure frame_available_cb);

    void OnFrameAvailableListener() override;

  private:
    base::RepeatingClosure frame_available_cb_;  // For immediate signal on callback thread
};

}  // namespace OHOS::NWeb

#endif  // FRAME_AVAILABLE_LISTENER_IMPL_H
