// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Based on vaapi_image_decode_accelerator_worker.cc originally written by
// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. 

#include "media/gpu/ohos/ohos_image_decode_accelerator_worker.h"

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
#include "media/gpu/ohos/ohos_heif_image_decoder.h"
#include "media/parsers/webp_parser.h"
#include "mojo/public/cpp/bindings/callback_helpers.h"
#include "string.h"
#include "ui/gfx/geometry/size.h"
#include "ui/gfx/gpu_memory_buffer.h"
#include "ui/gfx/linux/native_pixmap_dmabuf.h"
#include "ui/gfx/native_pixmap_handle.h"

namespace media {

namespace {
// Uses |decoder| to decode the image corresponding to |encoded_data|.
// |decode_cb| is called when finished or when an error is encountered. We don't
// support decoding to scale, so |output_size| is only used for tracing.
void DecodeTask(
    OhosImageDecoder* decoder,
    std::vector<uint8_t> encoded_data,
    const gfx::Size& output_size,
    gpu::ImageDecodeAcceleratorWorker::CompletedDecodeCB decode_cb) {
  // TRACE_EVENT2("heif", "OhosImageDecodeAcceleratorWorker::DecodeTask",
  //              "encoded_bytes", encoded_data.size(), "output_size",
  //              output_size.ToString()); fixme
  gpu::ImageDecodeAcceleratorWorker::CompletedDecodeCB scoped_decode_callback =
      mojo::WrapCallbackWithDefaultInvokeIfNotRun(std::move(decode_cb),
                                                  nullptr);
  if (!decoder) {
    LOG(ERROR) << "[HeifSupport] DecodeTask: No decoder is available for "
                  "supplied image";
    return;
  }

  OhosImageDecodeStatus status = decoder->Decode(encoded_data);
  if (status != OhosImageDecodeStatus::kSuccess) {
    LOG(ERROR) << "[HeifSupport]  Failed to decode - status = "
               << static_cast<uint32_t>(status);
    return;
  }
  // Export the decode result as a NativePixmap.
  std::unique_ptr<NativePixmapAndSizeInfo> exported_pixmap =
      decoder->ExportAsNativePixmapDmaBuf(&status);
  if (status != OhosImageDecodeStatus::kSuccess) {
    LOG(ERROR) << "[HeifSupport] Failed to export surface - status = "
               << static_cast<uint32_t>(status);
    return;
  }
  DCHECK(exported_pixmap);
  DCHECK(exported_pixmap->pixmap);
  if (exported_pixmap->pixmap->GetBufferSize() != output_size) {
    LOG(ERROR) << "[HeifSupport] Scaling is not supported" << "GetBufferSize "
               << exported_pixmap->pixmap->GetBufferSize().ToString()
               << ", output_size " << output_size.ToString();
    return;
  }

  // Output the decoded data.
  gfx::NativePixmapHandle pixmap_handle =
      exported_pixmap->pixmap->ExportHandle();
  // If a dup() failed while exporting the handle, we would get no planes.
  if (pixmap_handle.planes.empty()) {
    LOG(ERROR) << "[HeifSupport] Could not export the NativePixmapHandle";
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

// static
std::unique_ptr<OhosImageDecodeAcceleratorWorker>
OhosImageDecodeAcceleratorWorker::Create() {
  OhosImageDecoderVector decoders;

  auto heif_image_decoder = std::make_unique<OhosHeifImageDecoder>();

  if (heif_image_decoder->Initialize()) {
    decoders.push_back(std::move(heif_image_decoder));
  }

  // If there are no decoders due to disabled flags or initialization failure,
  // return nullptr.
  if (decoders.empty()) {
    return nullptr;
  }

  return base::WrapUnique(
      new OhosImageDecodeAcceleratorWorker(std::move(decoders)));
}

OhosImageDecodeAcceleratorWorker::OhosImageDecodeAcceleratorWorker(
    OhosImageDecoderVector decoders) {
  DETACH_FROM_SEQUENCE(io_sequence_checker_);
  decoder_task_runner_ = base::ThreadPool::CreateSequencedTaskRunner({});
  DCHECK(decoder_task_runner_);

  DCHECK(!decoders.empty());
  for (auto& decoder : decoders) {
    supported_profiles_.push_back(decoder->GetSupportedProfile());
    const gpu::ImageDecodeAcceleratorType type = decoder->GetType();
    decoders_[type] = std::move(decoder);
  }
}

OhosImageDecodeAcceleratorWorker::~OhosImageDecodeAcceleratorWorker() {
  DCHECK_CALLED_ON_VALID_SEQUENCE(main_sequence_checker_);
  DCHECK(decoder_task_runner_);
  for (auto& decoder : decoders_) {
    decoder_task_runner_->DeleteSoon(FROM_HERE, std::move(decoder.second));
  }
}

gpu::ImageDecodeAcceleratorSupportedProfiles
OhosImageDecodeAcceleratorWorker::GetSupportedProfiles() {
  return supported_profiles_;
}

OhosImageDecoder* OhosImageDecodeAcceleratorWorker::GetDecoderForImage() {
  DCHECK_CALLED_ON_VALID_SEQUENCE(io_sequence_checker_);
  auto result = decoders_.end();
  result = decoders_.find(gpu::ImageDecodeAcceleratorType::kHeif);

  return result == decoders_.end() ? nullptr : result->second.get();
}

void OhosImageDecodeAcceleratorWorker::Decode(std::vector<uint8_t> encoded_data,
                                              const gfx::Size& output_size,
                                              CompletedDecodeCB decode_cb) {
  DCHECK_CALLED_ON_VALID_SEQUENCE(io_sequence_checker_);
  DCHECK(decoder_task_runner_);
  // We defer checking for a null |decoder| until DecodeTask() because the
  // gpu::ImageDecodeAcceleratorWorker interface mandates that the callback be
  // called asynchronously.
  OhosImageDecoder* decoder = GetDecoderForImage();
  decoder_task_runner_->PostTask(
      FROM_HERE, base::BindOnce(&DecodeTask, decoder, std::move(encoded_data),
                                output_size, std::move(decode_cb)));
}

void OhosImageDecodeAcceleratorWorker::ReleaseDecodedPixelMap() {
  if (auto* decoder = GetDecoderForImage()) {
    decoder->ReleaseDecodedPixelMap();
  }
}

}  // namespace media
                     