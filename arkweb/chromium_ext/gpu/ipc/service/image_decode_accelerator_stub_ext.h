// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef GPU_IPC_SERVICE_IMAGE_DECODE_ACCELERATOR_STUB_EXT_H_
#define GPU_IPC_SERVICE_IMAGE_DECODE_ACCELERATOR_STUB_EXT_H_

#include "gpu/ipc/service/image_decode_accelerator_worker.h"
#include "gpu/ipc/service/gpu_channel.h"
#include "gpu/ipc/service/image_decode_accelerator_stub.h"

namespace gpu {
class ImageDecodeAcceleratorStub;

class ImageDecodeAcceleratorStubExt : public ImageDecodeAcceleratorStub {
public:
  ImageDecodeAcceleratorStubExt(ImageDecodeAcceleratorWorker* worker,
                             GpuChannel* channel,
                             int32_t route_id);
                             
  ~ImageDecodeAcceleratorStubExt() override;
#if BUILDFLAG(ARKWEB_HEIF_SUPPORT)
  bool ProcessCompletedDecodeExt(mojom::ScheduleImageDecodeParams& params,
                                 scoped_refptr<SharedContextState> shared_context_state,
                                 std::vector<sk_sp<SkImage>> plane_sk_images,
                                 std::optional<base::ScopedClosureRunner>& notify_gl_state_changed,
                                 std::unique_ptr<ImageDecodeAcceleratorWorker::DecodeResult>& completed_decode)
                                EXCLUSIVE_LOCKS_REQUIRED(lock_) override;
  
  void ReleasePixmapData(base::WaitableEvent* finish_event) override;
#endif
};

}
#endif  // GPU_IPC_SERVICE_IMAGE_DECODE_ACCELERATOR_STUB_EXT_H_