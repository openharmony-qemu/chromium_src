// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Based on vaapi_image_decode_accelerator_worker.h originally written by
// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. 

#ifndef MEDIA_GPU_OHOS_OHOS_IMAGE_DECODE_ACCELERATOR_WORKER_H_
#define MEDIA_GPU_OHOS_OHOS_IMAGE_DECODE_ACCELERATOR_WORKER_H_

#include <stdint.h>

#include <memory>
#include <unordered_map>
#include <vector>

#include "arkweb/build/features/features.h"
#include "base/containers/small_map.h"
#include "base/memory/scoped_refptr.h"
#include "base/sequence_checker.h"
#include "gpu/config/gpu_info.h"
#include "gpu/ipc/service/image_decode_accelerator_worker.h"

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
  // Creates a OhosImageDecodeAcceleratorWorker and attempts to initialize the
  // internal state. Returns nullptr if initialization fails.
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
  explicit OhosImageDecodeAcceleratorWorker(OhosImageDecoderVector decoders);

  OhosImageDecoder* GetDecoderForImage();

  // We delegate the decoding to the appropriate decoder in |decoders_| which
  // are used and destroyed on |decoder_task_runner_|.
  OhosImageDecoderMap decoders_;
  gpu::ImageDecodeAcceleratorSupportedProfiles supported_profiles_;
  scoped_refptr<base::SequencedTaskRunner> decoder_task_runner_;

  SEQUENCE_CHECKER(main_sequence_checker_);
  SEQUENCE_CHECKER(io_sequence_checker_);
};

}  // namespace media

#endif  // MEDIA_GPU_OHOS_OHOS_IMAGE_DECODE_ACCELERATOR_WORKER_H_
