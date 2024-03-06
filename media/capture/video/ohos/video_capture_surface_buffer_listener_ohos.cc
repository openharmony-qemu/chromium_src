// Copyright (c) 2023 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "video_capture_surface_buffer_listener_ohos.h"
#include "base/logging.h"
#include "ohos_adapter_helper.h"
namespace media {

VideoCaptureSufaceBufferListenerOHOS::VideoCaptureSufaceBufferListenerOHOS(
    const scoped_refptr<base::SingleThreadTaskRunner>& task_runner,
    base::WeakPtr<OHOSCaptureDelegate> capture_delegate)
    : task_runner_(task_runner), capture_delegate_(capture_delegate) {
  DCHECK(task_runner_.get());
  DCHECK(capture_delegate_);
}

VideoCaptureSufaceBufferListenerOHOS::~VideoCaptureSufaceBufferListenerOHOS() {}

void VideoCaptureSufaceBufferListenerOHOS::OnBufferAvailable(
    std::shared_ptr<CameraSurfaceAdapter> surface,
    std::shared_ptr<OHOS::NWeb::CameraSurfaceBufferAdapter> buffer,
    std::shared_ptr<OHOS::NWeb::CameraRotationInfoAdapter> roration_info) {
  task_runner_->PostTask(
      FROM_HERE,
      base::BindOnce(&OHOSCaptureDelegate::OnBufferAvailable, capture_delegate_,
                     surface, std::move(buffer), roration_info));
}

}  // namespace media
