/*
 * Copyright (c) 2023-2026 Huawei Device Co., Ltd.
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors
 * may be used to endorse or promote products derived from this software without
 * specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef MEDIA_GPU_OHOS_OHOS_IMAGE_DECODE_ACCELERATOR_WORKER_H_
#define MEDIA_GPU_OHOS_OHOS_IMAGE_DECODE_ACCELERATOR_WORKER_H_

#include <stdint.h>

#include <memory>
#include <unordered_map>
#include <vector>

#include "base/containers/small_map.h"
#include "base/memory/scoped_refptr.h"
#include "gpu/config/gpu_info.h"
#include "gpu/ipc/service/image_decode_accelerator_worker.h"
#include "media/gpu/ohos/ohos_image_decoder.h"

namespace base {
class SequencedTaskRunner;
}

namespace gfx {
class Size;
}

namespace media {

class OhosImageDecoder;

using OhosImageDecoderVector = std::vector<std::unique_ptr<OhosImageDecoder>>;
using OhosImageDecoderMap =
    base::small_map<std::unordered_map<gpu::ImageDecodeAcceleratorType,
                                       std::unique_ptr<OhosImageDecoder>>>;

class OhosImageDecodeAcceleratorWorker
    : public gpu::ImageDecodeAcceleratorWorker {
 public:
  static std::unique_ptr<OhosImageDecodeAcceleratorWorker> Create();

  OhosImageDecodeAcceleratorWorker(const OhosImageDecodeAcceleratorWorker&) =
      delete;
  OhosImageDecodeAcceleratorWorker& operator=(
      const OhosImageDecodeAcceleratorWorker&) = delete;
  ~OhosImageDecodeAcceleratorWorker() override;

  gpu::ImageDecodeAcceleratorSupportedProfiles GetSupportedProfiles() override;
  void Decode(std::vector<uint8_t> encoded_data,
              const gfx::Size& output_size,
              CompletedDecodeCB decode_cb) override;
  void ReleaseDecodedPixelMap() override;

 private:
  explicit OhosImageDecodeAcceleratorWorker(
      std::unique_ptr<OhosImageDecoder> decoder);

  std::unique_ptr<OhosImageDecoder> decoder_;
  gpu::ImageDecodeAcceleratorSupportedProfiles supported_profiles_;
  scoped_refptr<base::SequencedTaskRunner> decoder_task_runner_;
};
}  // namespace media

#endif  // MEDIA_GPU_OHOS_OHOS_IMAGE_DECODE_ACCELERATOR_WORKER_H_
