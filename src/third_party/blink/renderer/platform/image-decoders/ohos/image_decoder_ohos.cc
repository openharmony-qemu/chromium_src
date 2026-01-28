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

#include "third_party/blink/renderer/platform/image-decoders/ohos/image_decoder_ohos.h"

#include "base/logging.h"

namespace blink {

ImageDecoderOhos::ImageDecoderOhos(AlphaOption alpha_option,
                                   HighBitDepthDecodingOption hbd_option,
                                   const ColorBehavior& color_behavior,
                                   wtf_size_t max_decoded_bytes,
                                   AnimationOption animation_option)
    : ImageDecoder(alpha_option,
                   hbd_option,
                   color_behavior,
                   cc::AuxImage::kDefault,
                   max_decoded_bytes) {
  decoder_adapter_ = std::make_unique<ImageDecoderAdapterOhos>();
}

ImageDecoderOhos::~ImageDecoderOhos() {
  decoder_adapter_.reset();
}

void ImageDecoderOhos::OnSetData(scoped_refptr<SegmentReader> data) {
  if (!data || data->size() == 0 || !IsAllDataReceived()) {
    return;
  }

  auto sk_data = data->GetAsSkData();
  if (!sk_data || sk_data->size() == 0) {
    LOG(ERROR) << "[OhosImageDecode] Failed to get contiguous memory from "
                  "SegmentReader.";
    SetFailed();
    return;
  }

  if (!decoder_adapter_) {
    LOG(ERROR) << "[OhosImageDecode] Fail to get decoder adapter.";
    SetFailed();
    return;
  }

  const uint8_t* bytes = reinterpret_cast<const uint8_t*>(sk_data->bytes());
  uint32_t length = static_cast<uint32_t>(sk_data->size());
  if (decoder_adapter_->ParseRawData(bytes, length)) {
    uint32_t width = decoder_adapter_->GetImageWidth();
    uint32_t height = decoder_adapter_->GetImageHeight();
    LOG(INFO) << "[OhosImageDecode] OHOS image parsed: " << width << " * "
              << height;
    SetSize(width, height);
    return;
  }
  SetFailed();
}

void ImageDecoderOhos::DecodeSize() {}

void ImageDecoderOhos::Decode(wtf_size_t index) {
  if (frame_buffer_cache_.empty()) {
    LOG(ERROR)
        << "[OhosImageDecode] ImageDecoderOhos::Decode frame_buffer_cache "
           "is empty.";
    SetFailed();
    return;
  }

  if (!decoder_adapter_ || !decoder_adapter_->Decode()) {
    LOG(ERROR) << "[OhosImageDecode] ImageDecoderOhos::Decode decode failed.";
    SetFailed();
    return;
  }

  void* decoded_ptr = decoder_adapter_->GetDecodeData();
  if (!decoded_ptr || !InitFrameBuffer(index)) {
    SetFailed();
    return;
  }

  ImageFrame& buffer = frame_buffer_cache_[index];
  int width = buffer.Bitmap().width();
  int height = buffer.Bitmap().height();
  size_t row_bytes = buffer.Bitmap().rowBytes();
  int stride = decoder_adapter_->GetStride();
  uint8_t* dst_ptr = reinterpret_cast<uint8_t*>(buffer.GetAddr(0, 0));

  if (row_bytes * height != decoder_adapter_->GetByteCount()) {
    LOG(ERROR) << "[OhosImageDecode] The sizes of the copied memory are "
                  "inconsistent, source size:"
               << decoder_adapter_->GetByteCount()
               << " target size:" << row_bytes * height;
    SetFailed();
    return;
  }

  // This is the point where the decoded data is copied. Due to the decoded
  // image data having a stride (bytes per row) that is 64-byte aligned, this
  // value often does not match the image's effective row_bytes. Consequently,
  // we are forced to perform row-by-row (strided) copying.
  if (stride == static_cast<int>(row_bytes)) {
    memcpy(dst_ptr, decoded_ptr, height * row_bytes);
  } else {
    for (int i = 0; i < height; i++) {
      memcpy(dst_ptr + i * row_bytes,
             reinterpret_cast<uint8_t*>(decoded_ptr) + i * stride,
             width * buffer.Bitmap().bytesPerPixel());
    }
  }

  buffer.SetPixelsChanged(true);
  buffer.SetStatus(ImageFrame::kFrameComplete);
  LOG(INFO) << "[OhosImageDecode] ImageDecoderOhos::Decode decode complete.";
}

}  // namespace blink
