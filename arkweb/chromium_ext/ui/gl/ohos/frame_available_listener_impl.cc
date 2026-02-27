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
        base::WeakPtr<gl::OhosNativeImage> adapter,
        scoped_refptr<base::SingleThreadTaskRunner> task_runner)
        :oni_wptr(std::move(adapter)), task_runner_(task_runner) {}

void FrameAvailableListenerImpl::OnFrameAvailableListener() {
  task_runner_->PostTask(
              FROM_HERE, base::BindOnce(&gl::OhosNativeImage::OnFrameAvailableListener, oni_wptr));
}
}  // namespace OHOS::NWeb
