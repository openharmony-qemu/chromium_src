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

#include "media/gpu/ohos/ohos_image_decode_accelerator_worker.h"

#include <cstring>
#include <utility>

#include "base/containers/span.h"
#include "base/feature_list.h"
#include "base/functional/bind.h"
#include "base/location.h"
#include "base/logging.h"
#include "base/memory/ptr_util.h"
#include "base/metrics/histogram_macros.h"
#include "base/synchronization/waitable_event.h"
#include "base/task/sequenced_task_runner.h"
#include "base/task/thread_pool.h"
#include "base/trace_event/trace_event.h"
#include "gpu/config/gpu_finch_features.h"
#include "media/gpu/macros.h"
#include "media/gpu/ohos/ohos_image_decoder.h"
#include "media/parsers/webp_parser.h"
#include "mojo/public/cpp/bindings/callback_helpers.h"
#include "ui/gfx/geometry/size.h"
#include "ui/gfx/gpu_memory_buffer.h"
#include "ui/gfx/linux/native_pixmap_dmabuf.h"
#include "ui/gfx/native_pixmap_handle.h"

namespace media {

namespace {

void DecodeTask(
    OhosImageDecoder* decoder,
    std::vector<uint8_t> encoded_data,
    const gfx::Size& output_size,
    gpu::ImageDecodeAcceleratorWorker::CompletedDecodeCB decode_cb) {
  gpu::ImageDecodeAcceleratorWorker::CompletedDecodeCB scoped_decode_callback =
      mojo::WrapCallbackWithDefaultInvokeIfNotRun(std::move(decode_cb),
                                                  nullptr);
  if (!decoder) {
    LOG(ERROR)
        << __func__
        << "[OhosImageDecode] No decoder is available for supplied image";
    std::move(scoped_decode_callback).Run(nullptr);
    return;
  }
  OhosImageDecodeStatus status = decoder->Decode(encoded_data);
  if (status != OhosImageDecodeStatus::kSuccess) {
    LOG(ERROR) << __func__ << "[OhosImageDecode] Failed to decode - status = "
               << static_cast<uint32_t>(status);
    std::move(scoped_decode_callback).Run(nullptr);
    return;
  }
  std::unique_ptr<NativePixmapAndSizeInfo> exported_pixmap =
      decoder->ExportAsNativePixmapDmaBuf(&status);
  if (!exported_pixmap) {
    LOG(ERROR)
        << __func__
        << "[OhosImageDecode] ExportAsNativePixmapDmaBuf failed, status: "
        << static_cast<uint32_t>(status);
    return;
  }
  gfx::NativePixmapHandle pixmap_handle =
      exported_pixmap->pixmap->ExportHandle();
  if (pixmap_handle.planes.empty()) {
    LOG(ERROR) << __func__
               << "[OhosImageDecode] Could not export the NativePixmapHandle";
    return;
  }
  if (exported_pixmap->pixmap->GetBufferSize() != output_size) {
    LOG(ERROR) << __func__ << "[OhosImageDecode] Scaling is not supported"
               << "GetBufferSize "
               << exported_pixmap->pixmap->GetBufferSize().ToString()
               << ", output_size " << output_size.ToString();
    return;
  }
  auto result =
      std::make_unique<gpu::ImageDecodeAcceleratorWorker::DecodeResult>();
  result->handle.type = gfx::GpuMemoryBufferType::NATIVE_PIXMAP;
  result->handle.native_pixmap_handle = std::move(pixmap_handle);
  result->visible_size = exported_pixmap->pixmap->GetBufferSize();
  result->buffer_format = exported_pixmap->pixmap->GetBufferFormat();
  result->buffer_byte_size = exported_pixmap->byte_size;
  result->yuv_color_space = decoder->GetYUVColorSpace();
  result->window_buffer = exported_pixmap->pixmap->GetWindowBuffer();

  // Due to the limitation of the surface system. OhosImageDecoderAdapter
  // doesn't provide a concurrent decoding mechanism. We make blocking calls
  // from decoder thread to main thread to ensure current decoding data has been
  // processed before a new decoding task starts. io_thread main_thread
  // decode_thread
  //
  // ScheduleImageDecode --------------------------------> Decode
  //                                                   EnableSequence
  //                                                       Wait
  //                       ProcessCompletedDecode <------
  //                             Signal
  // ScheduleImageDecode --------------------------------> Decode
  //
  auto completion_event_ptr = std::make_unique<base::WaitableEvent>(
      base::WaitableEvent::ResetPolicy::AUTOMATIC,
      base::WaitableEvent::InitialState::NOT_SIGNALED);
  result->event = completion_event_ptr.get();
  std::move(scoped_decode_callback).Run(std::move(result));
  completion_event_ptr->Wait();
}

}  // namespace

std::unique_ptr<OhosImageDecodeAcceleratorWorker> OhosImageDecodeAcceleratorWorker::Create() {
  auto decoder = std::make_unique<OhosImageDecoder>();
  decoder->Initialize();
  return base::WrapUnique(
      new OhosImageDecodeAcceleratorWorker(std::move(decoder)));
}

OhosImageDecodeAcceleratorWorker::OhosImageDecodeAcceleratorWorker(
    std::unique_ptr<OhosImageDecoder> decoder)
    : decoder_(std::move(decoder)) {
  decoder_task_runner_ = base::ThreadPool::CreateSequencedTaskRunner({});
  CHECK(decoder_);
  auto profiles = decoder_->GetSupportedProfiles();
  for (auto& profile : profiles) {
    supported_profiles_.push_back(profile);
  }
}

OhosImageDecodeAcceleratorWorker::~OhosImageDecodeAcceleratorWorker() {
  DCHECK(decoder_task_runner_);
  decoder_task_runner_->DeleteSoon(FROM_HERE, std::move(decoder_));
}

gpu::ImageDecodeAcceleratorSupportedProfiles OhosImageDecodeAcceleratorWorker::GetSupportedProfiles() {
  return supported_profiles_;
}

void OhosImageDecodeAcceleratorWorker::Decode(std::vector<uint8_t> encoded_data,
                                              const gfx::Size& output_size,
                                              CompletedDecodeCB decode_cb) {
  if (!decoder_) {
    LOG(ERROR) << __func__ << "[OhosImageDecode] No decoder found for image";
    std::move(decode_cb).Run(nullptr);
    return;
  }
  decoder_task_runner_->PostTask(
      FROM_HERE,
      base::BindOnce(&DecodeTask, decoder_.get(), std::move(encoded_data),
                     output_size, std::move(decode_cb)));
}

void OhosImageDecodeAcceleratorWorker::ReleaseDecodedPixelMap() {
  decoder_->ReleaseDecodedPixelMap();
}

}  // namespace media
