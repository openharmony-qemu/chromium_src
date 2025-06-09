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

#ifndef MEDIA_GPU_OHOS_VIDEO_DECODER_H_
#define MEDIA_GPU_OHOS_VIDEO_DECODER_H_

#include <vector>

#include "base/containers/circular_deque.h"
#include "base/memory/raw_ptr.h"
#include "base/threading/thread_checker.h"
#include "base/timer/elapsed_timer.h"
#include "base/timer/timer.h"
#include "gpu/command_buffer/service/ref_counted_lock.h"
#include "gpu/config/gpu_feature_info.h"
#include "gpu/config/gpu_preferences.h"
#include "media/base/callback_registry.h"
#include "media/base/cdm_context.h"
#include "media/base/scoped_async_trace.h"
#include "media/base/video_decoder.h"
#include "media/base/video_decoder_config.h"
#include "media/gpu/media_gpu_export.h"
#include "media/gpu/ohos/codec_allocator.h"
#include "media/gpu/ohos/codec_wrapper.h"
#include "media/gpu/ohos/video_frame_factory.h"
#include "third_party/abseil-cpp/absl/types/optional.h"

namespace media {

class MediaLog;
struct SupportedVideoDecoderConfig;

struct PendingDecode {
  static PendingDecode CreateEos();
  PendingDecode(scoped_refptr<DecoderBuffer> buffer,
                VideoDecoder::DecodeCB decode_cb);

  PendingDecode(const PendingDecode&) = delete;
  PendingDecode& operator=(const PendingDecode&) = delete;

  PendingDecode(PendingDecode&& other);

  ~PendingDecode();

  scoped_refptr<DecoderBuffer> buffer;
  VideoDecoder::DecodeCB decode_cb;
};

class MEDIA_GPU_EXPORT OhosVideoDecoder final
    : public VideoDecoder,
      public gpu::RefCountedLockHelperDrDc {
 public:
  static std::vector<SupportedVideoDecoderConfig> GetSupportedConfigs();

  OhosVideoDecoder(const OhosVideoDecoder&) = delete;
  OhosVideoDecoder& operator=(const OhosVideoDecoder&) = delete;

  ~OhosVideoDecoder() override;
  static void DestroyAsync(std::unique_ptr<OhosVideoDecoder>);

  static std::unique_ptr<VideoDecoder> Create(
      const gpu::GpuPreferences& gpu_preferences,
      const gpu::GpuFeatureInfo& gpu_feature_info,
      std::unique_ptr<MediaLog> media_log,
      CodecAllocator* codec_allocator,
      std::unique_ptr<VideoFrameFactory> video_frame_factory,
      scoped_refptr<gpu::RefCountedLock> drdc_lock);

  // VideoDecoder implementation:
  VideoDecoderType GetDecoderType() const override;
  void Initialize(const VideoDecoderConfig& config,
                  bool low_delay,
                  CdmContext* cdm_context,
                  InitCB init_cb,
                  const OutputCB& output_cb,
                  const WaitingCB& waiting_cb) override;
  void Decode(scoped_refptr<DecoderBuffer> buffer, DecodeCB decode_cb) override;
  void Reset(base::OnceClosure closure) override;
  bool NeedsBitstreamConversion() const override;
  bool CanReadWithoutStalling() const override;
  int GetMaxDecodeRequests() const override;

 private:
  OhosVideoDecoder(const gpu::GpuPreferences& gpu_preferences,
                   const gpu::GpuFeatureInfo& gpu_feature_info,
                   std::unique_ptr<MediaLog> media_log,
                   CodecAllocator* codec_allocator,
                   std::unique_ptr<VideoFrameFactory> video_frame_factory,
                   scoped_refptr<gpu::RefCountedLock> drdc_lock);

  enum class State { kInitializing, kRunning, kError, kSurfaceDestroyed };

  enum class DrainType { kForReset, kForDestroy };

  void StartLazyInit();
  void OnVideoFrameFactoryInitialized(
      scoped_refptr<gpu::NativeImageTextureOwner> texture_owner);

  void OnSurfaceChosen();

  bool SurfaceTransitionPending();

  void TransitionToTargetSurface();

  void CreateCodec();

  static void OnCodecConfiguredInternal(
      base::WeakPtr<OhosVideoDecoder> weak_this,
      CodecAllocator* codec_allocator,
      scoped_refptr<CodecSurfaceBundle> surface_bundle,
      std::unique_ptr<MediaCodecDecoderBridgeImpl> codec);
  void OnCodecConfigured(
      scoped_refptr<CodecSurfaceBundle> surface_bundle,
      std::unique_ptr<MediaCodecDecoderBridgeImpl> media_codec);

  void FlushCodec();

  void PumpCodec();
  bool QueueInput();
  bool DequeueOutput();

  void RunEosDecodeCb(int reset_generation);

  void ForwardVideoFrame(int reset_generation,
                         std::unique_ptr<ScopedAsyncTrace> async_trace,
                         base::TimeTicks started_at,
                         scoped_refptr<VideoFrame> frame);

  void StartDrainingCodec(DrainType drain_type);
  void OnCodecDrained();
  void CancelPendingDecodes(DecoderStatus status);
  void EnterTerminalState(State state, const char* reason);
  bool InTerminalState();

  void ReleaseCodec();

  std::unique_ptr<MediaLog> media_log_;

  State state_ = State::kInitializing;

  bool lazy_init_pending_ = true;

  base::circular_deque<PendingDecode> pending_decodes_;

  absl::optional<DrainType> drain_type_;

  base::OnceClosure reset_cb_;

  int reset_generation_ = 0;

  DecodeCB eos_decode_cb_;
  OutputCB output_cb_;
  WaitingCB waiting_cb_;
  VideoDecoderConfig decoder_config_;

  std::unique_ptr<CodecWrapper> codec_;
  base::ElapsedTimer idle_timer_;
  base::RepeatingTimer pump_codec_timer_;
  raw_ptr<CodecAllocator> codec_allocator_;

  scoped_refptr<CodecSurfaceBundle> target_surface_bundle_;

  scoped_refptr<CodecSurfaceBundle> texture_owner_bundle_;

  const bool is_surface_control_enabled_;

  std::unique_ptr<VideoFrameFactory> video_frame_factory_;

  bool deferred_flush_pending_ = false;

  bool deferred_reallocation_pending_ = false;

  int last_width_ = 0;

  base::WeakPtrFactory<OhosVideoDecoder> weak_factory_{this};
  base::WeakPtrFactory<OhosVideoDecoder> codec_allocator_weak_factory_{this};
};

}  // namespace media

namespace std {

template <>
struct MEDIA_GPU_EXPORT default_delete<media::OhosVideoDecoder>
    : public default_delete<media::VideoDecoder> {};

}  // namespace std

#endif  // MEDIA_GPU_OHOS_MEDIA_CODEC_VIDEO_DECODER_H_
