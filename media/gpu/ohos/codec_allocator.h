/*
 * Copyright (c) 2023-2025 Haitai FangYuan Co., Ltd.
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 *    conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 *    of conditions and the following disclaimer in the documentation and/or other materials
 *    provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 *    to endorse or promote products derived from this software without specific prior written
 *    permission.
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

#ifndef MEDIA_GPU_OHOS_CODEC_ALLOCATOR_H_
#define MEDIA_GPU_OHOS_CODEC_ALLOCATOR_H_

#include <cstddef>
#include <string>

#include <memory>

#include "base/functional/callback.h"
#include "base/containers/circular_deque.h"
#include "base/memory/raw_ptr.h"
#include "base/no_destructor.h"
#include "base/task/sequenced_task_runner.h"
#include "media/base/ohos/ohos_media_decoder_bridge_impl.h"
#include "media/gpu/media_gpu_export.h"

namespace base {
class TickClock;
}

namespace media {
class MEDIA_GPU_EXPORT CodecAllocator {
 public:
  static CodecAllocator* GetInstance(
      scoped_refptr<base::SequencedTaskRunner> task_runner);

  CodecAllocator(const CodecAllocator&) = delete;
  CodecAllocator& operator=(const CodecAllocator&) = delete;

  using CodecFactoryCB =
      base::RepeatingCallback<std::unique_ptr<MediaCodecDecoderBridgeImpl>(
          const VideoBridgeCodecConfig& config)>;

  using CodecCreatedCB =
      base::OnceCallback<void(std::unique_ptr<MediaCodecDecoderBridgeImpl>)>;
  virtual void CreateMediaCodecAsync(
      CodecCreatedCB codec_created_cb,
      std::unique_ptr<VideoBridgeCodecConfig> codec_config);

  virtual void ReleaseMediaCodec(
      std::unique_ptr<MediaCodecDecoderBridgeImpl> codec,
      base::OnceClosure codec_released_cb);

 protected:
  friend class base::NoDestructor<CodecAllocator>;

  CodecAllocator(CodecFactoryCB factory_cb,
                 scoped_refptr<base::SequencedTaskRunner> task_runner);
  virtual ~CodecAllocator();

 private:
  friend class CodecAllocatorTest;

  void OnCodecCreated(base::TimeTicks start_time,
                      CodecCreatedCB codec_created_cb,
                      std::unique_ptr<MediaCodecDecoderBridgeImpl> codec);

  void OnCodecReleased(base::TimeTicks start_time,
                       base::OnceClosure codec_released_cb);

  base::SequencedTaskRunner* SelectCodecTaskRunner();

  const scoped_refptr<base::SequencedTaskRunner> task_runner_;

  const CodecFactoryCB factory_cb_;

  raw_ptr<const base::TickClock> tick_clock_;

  scoped_refptr<base::SequencedTaskRunner> primary_task_runner_;
};

}  // namespace media

#endif  // MEDIA_GPU_OHOS_CODEC_ALLOCATOR_H_
