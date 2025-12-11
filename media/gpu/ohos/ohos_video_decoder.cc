// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "media/gpu/ohos/ohos_video_decoder.h"

#include <memory>

#include "base/command_line.h"
#include "base/functional/bind.h"
#include "base/functional/callback.h"
#include "base/functional/callback_helpers.h"
#include "base/logging.h"
#include "base/memory/ptr_util.h"
#include "base/memory/weak_ptr.h"
#include "base/metrics/histogram_macros.h"
#include "base/task/bind_post_task.h"
#include "base/task/sequenced_task_runner.h"
#include "base/threading/platform_thread.h"
#include "base/time/time.h"
#include "base/trace_event/trace_event.h"
#include "media/base/async_destroy_video_decoder.h"
#include "media/base/decoder_buffer.h"
#include "media/base/media_log.h"
#include "media/base/media_switches.h"
#include "media/base/ohos/ohos_media_codec_util.h"
#include "media/base/ohos/ohos_media_decoder_bridge_impl.h"
#include "media/base/scoped_async_trace.h"
#include "media/base/status.h"
#include "media/base/supported_video_decoder_config.h"
#include "media/base/video_aspect_ratio.h"
#include "media/base/video_codecs.h"
#include "media/base/video_decoder_config.h"
#include "media/base/video_frame.h"
#include "media/gpu/ohos/codec_allocator.h"
#include "media/media_buildflags.h"

using media::OhosMediaCodecUtil;

namespace media {
namespace {

void OutputBufferReleased(base::RepeatingClosure pump_cb, bool has_work) {
  if (!has_work) {
    return;
  }
  pump_cb.Run();
}

bool IsSurfaceControlEnabled(const gpu::GpuFeatureInfo& info) {
  return true;
}

std::vector<SupportedVideoDecoderConfig> GetSupportedConfigsInternal() {
  std::vector<SupportedVideoDecoderConfig> supported_configs;

  std::optional<SupportedVideoDecoderConfig> avc_supported_video_decoder_config =
      OhosMediaCodecUtil::GetAVCSupportedConfig();
  if (avc_supported_video_decoder_config.has_value()) {
    supported_configs.emplace_back(avc_supported_video_decoder_config.value());
  }

  std::optional<SupportedVideoDecoderConfig> hevc_supported_video_decoder_config =
      OhosMediaCodecUtil::GetHEVCSupportedConfig();
  if (hevc_supported_video_decoder_config.has_value()) {
    supported_configs.emplace_back(hevc_supported_video_decoder_config.value());
  }

  std::optional<SupportedVideoDecoderConfig> vvc_supported_video_decoder_config =
      OhosMediaCodecUtil::GetVVCSupportedConfig();
  if (vvc_supported_video_decoder_config.has_value()) {
    supported_configs.emplace_back(vvc_supported_video_decoder_config.value());
  }

  return supported_configs;
}

}  // namespace

// static
PendingDecode PendingDecode::CreateEos() {
  return {DecoderBuffer::CreateEOSBuffer(), base::DoNothing()};
}

PendingDecode::PendingDecode(scoped_refptr<DecoderBuffer> buffer,
                             VideoDecoder::DecodeCB decode_cb)
    : buffer(std::move(buffer)), decode_cb(std::move(decode_cb)) {}
PendingDecode::PendingDecode(PendingDecode&& other) = default;
PendingDecode::~PendingDecode() = default;

// static
std::vector<SupportedVideoDecoderConfig>
OhosVideoDecoder::GetSupportedConfigs() {
  static const auto configs = GetSupportedConfigsInternal();
  return configs;
}

OhosVideoDecoder::OhosVideoDecoder(
    const gpu::GpuPreferences& gpu_preferences,
    const gpu::GpuFeatureInfo& gpu_feature_info,
    std::unique_ptr<MediaLog> media_log,
    CodecAllocator* codec_allocator,
    std::unique_ptr<VideoFrameFactory> video_frame_factory,
    scoped_refptr<gpu::RefCountedLock> drdc_lock)
    : gpu::RefCountedLockHelperDrDc(std::move(drdc_lock)),
      media_log_(std::move(media_log)),
      codec_allocator_(codec_allocator),
      is_surface_control_enabled_(IsSurfaceControlEnabled(gpu_feature_info)),
      video_frame_factory_(std::move(video_frame_factory)) {}

std::unique_ptr<VideoDecoder> OhosVideoDecoder::Create(
    const gpu::GpuPreferences& gpu_preferences,
    const gpu::GpuFeatureInfo& gpu_feature_info,
    std::unique_ptr<MediaLog> media_log,
    CodecAllocator* codec_allocator,
    std::unique_ptr<VideoFrameFactory> video_frame_factory,
    scoped_refptr<gpu::RefCountedLock> drdc_lock) {
  auto* decoder = new OhosVideoDecoder(
      gpu_preferences, gpu_feature_info, std::move(media_log), codec_allocator,
      std::move(video_frame_factory), std::move(drdc_lock));
  return std::make_unique<AsyncDestroyVideoDecoder<OhosVideoDecoder>>(
      base::WrapUnique(decoder));
}

OhosVideoDecoder::~OhosVideoDecoder() {
  TRACE_EVENT0("media", "OhosVideoDecoder::~OhosVideoDecoder");
  ReleaseCodec();
}

void OhosVideoDecoder::DestroyAsync(std::unique_ptr<OhosVideoDecoder> decoder) {
  TRACE_EVENT0("media", "OhosVideoDecoder::DestroyAsync");
  DCHECK(decoder);
  auto* self = decoder.release();

  if (self == nullptr) {
    LOG(ERROR) << __func__ << " [WiseplayDrm] decoder is nullptr";
    return;
  }

  self->weak_factory_.InvalidateWeakPtrs();

#if BUILDFLAG(ENABLE_WISEPLAY)
  if (self->ohos_crypto_context_) {
    // Cancel previously registered callback (if any).
    self->event_cb_registration_.reset();
    self->ohos_crypto_context_->SetOHOSMediaCryptoReadyCB(base::NullCallback());
    self->ohos_crypto_context_ = nullptr;
  }
#endif  // BUILDFLAG(ENABLE_WISEPLAY)

  if (self->reset_cb_) {
    std::move(self->reset_cb_).Run();
  }

  self->codec_allocator_weak_factory_.InvalidateWeakPtrs();
  self->CancelPendingDecodes(DecoderStatus::Codes::kAborted);
  self->StartDrainingCodec(DrainType::kForDestroy);
}

void OhosVideoDecoder::Initialize(const VideoDecoderConfig& config,
                                  bool low_delay,
                                  CdmContext* cdm_context,
                                  InitCB init_cb,
                                  const OutputCB& output_cb,
                                  const WaitingCB& waiting_cb) {
  DCHECK(output_cb);
  DCHECK(waiting_cb);
  const bool first_init = !decoder_config_.IsValidConfig();
  DVLOG(1) << (first_init ? "Initializing" : "Reinitializing")
             << " config: " << config.AsHumanReadableString();

  if (!config.IsValidConfig()) {
    LOG(WARNING) << __FUNCTION__ << " [WiseplayDRM] config invalid.";
    base::BindPostTaskToCurrentDefault(std::move(init_cb))
        .Run(DecoderStatus::Codes::kUnsupportedConfig);
    return;
  }

  if (!first_init && decoder_config_.codec() != config.codec()) {
    LOG(WARNING) << __FUNCTION__ << " [WiseplayDRM] duplicate init.";
    base::BindPostTaskToCurrentDefault(std::move(init_cb))
        .Run(DecoderStatus::Codes::kCantChangeCodec);
    return;
  }

  decoder_config_ = config;
  output_cb_ = output_cb;
  waiting_cb_ = waiting_cb;

  const int width = decoder_config_.coded_size().width();
#if BUILDFLAG(ENABLE_WISEPLAY)
  if (first_init && cdm_context && cdm_context->GetOhosMediaCryptoContext()) {
    last_width_ = width;
    SetCdm(cdm_context, std::move(init_cb));
    return;
  }
  if (config.is_encrypted() && media_key_session_ == nullptr) {
    base::BindPostTaskToCurrentDefault(std::move(init_cb))
        .Run(DecoderStatus::Codes::kUnsupportedEncryptionMode);
    return;
  }
#else
  if (first_init) {
    last_width_ = width;
  }
#endif  // BUILDFLAG(ENABLE_WISEPLAY)
  base::BindPostTaskToCurrentDefault(std::move(init_cb))
      .Run(DecoderStatus::Codes::kOk);
}

#if BUILDFLAG(ENABLE_WISEPLAY)
void OhosVideoDecoder::SetCdm(CdmContext* cdm_context, InitCB init_cb) {
  TRACE_EVENT0("media", "OhosVideoDecoder::SetCdm");
  if (!cdm_context) {
    LOG(WARNING) << __FUNCTION__ << " [WiseplayDRM] OhosVideoDecoder::SetCdm No CDM provided";
    base::BindPostTaskToCurrentDefault(std::move(init_cb)).Run(DecoderStatus::Codes::kFailed);
    return;
  }

  ohos_crypto_context_ = cdm_context->GetOhosMediaCryptoContext();

  event_cb_registration_ = cdm_context->RegisterEventCB(base::BindRepeating(
      &OhosVideoDecoder::OnCdmContextEvent, weak_factory_.GetWeakPtr()));

  ohos_crypto_context_->SetOHOSMediaCryptoReadyCB(
      base::BindPostTaskToCurrentDefault(
          base::BindOnce(&OhosVideoDecoder::OnMediaCryptoReady,
                         weak_factory_.GetWeakPtr(), std::move(init_cb))));
}

void OhosVideoDecoder::OnMediaCryptoReady(
    InitCB init_cb,
    void* media_key_session,
    bool requires_secure_video_codec) {
  DCHECK_CALLED_ON_VALID_SEQUENCE(sequence_checker_);
  DVLOG(1) << __func__
           << ": requires_secure_video_codec = " << requires_secure_video_codec;

  DCHECK(state_ == State::kInitializing);
  DCHECK(media_key_session);

  if (media_key_session == nullptr) {
    ohos_crypto_context_->SetOHOSMediaCryptoReadyCB(base::NullCallback());
    ohos_crypto_context_ = nullptr;
    media_key_session_ = nullptr;
    requires_secure_codec_ = requires_secure_video_codec;
    if (codec_ &&
        !codec_->SetDecryptionConfig(nullptr, requires_secure_video_codec)) {
      LOG(ERROR)
          << "OhosVideoDecoder::OnMediaCryptoReady set decryt nullptr fail";
    }
    if (decoder_config_.is_encrypted()) {
      LOG(ERROR)
          << "OhosVideoDecoder::OnMediaCryptoReady can't play encrypted stream";
      EnterTerminalState(State::kError, "MediaCrypto is not available");
      std::move(init_cb).Run(DecoderStatus::Codes::kUnsupportedEncryptionMode);
      return;
    }

    // MediaCrypto is not available, but the stream is clear. So we can still
    // play the current stream. But if we switch to an encrypted stream playback
    // will fail.
    std::move(init_cb).Run(DecoderStatus::Codes::kOk);
    return;
  }

  media_key_session_ = std::move(media_key_session);
  requires_secure_codec_ = requires_secure_video_codec;

  // Signal success, and create the codec lazily on the first decode.
  if (!init_cb.is_null()) {
    std::move(init_cb).Run(DecoderStatus::Codes::kOk);
  }
}

void OhosVideoDecoder::OnCdmContextEvent(CdmContext::Event event) {
  LOG(INFO) << "OhosVideoDecoder::OnCdmContextEvent enter";
  if (event != CdmContext::Event::kHasAdditionalUsableKey) {
    return;
  }

  waiting_for_key_ = false;
  PumpCodec();
}
#endif  // BUILDFLAG(ENABLE_WISEPLAY)

void OhosVideoDecoder::StartLazyInit() {
  TRACE_EVENT0("media", "OhosVideoDecoder::StartLazyInit");
  lazy_init_pending_ = false;
  video_frame_factory_->Initialize(
      base::BindRepeating(&OhosVideoDecoder::OnVideoFrameFactoryInitialized,
                          weak_factory_.GetWeakPtr()));
}

void OhosVideoDecoder::OnVideoFrameFactoryInitialized(
    scoped_refptr<gpu::NativeImageTextureOwner> texture_owner) {
  TRACE_EVENT0("media", "OhosVideoDecoder::OnVideoFrameFactoryInitialized");
  if (!texture_owner) {
    LOG(WARNING) << __FUNCTION__ << " [WiseplayDRM] no texture owner: " << texture_owner;
    EnterTerminalState(State::kError, "Could not allocated TextureOwner");
    return;
  }
  texture_owner_bundle_ =
      new CodecSurfaceBundle(std::move(texture_owner), GetDrDcLock());

  OnSurfaceChosen();
}

void OhosVideoDecoder::OnSurfaceChosen() {
  DCHECK(state_ == State::kInitializing);
  TRACE_EVENT0("media", "OhosVideoDecoder::OnSurfaceChosen");
  target_surface_bundle_ = texture_owner_bundle_;

  if (state_ == State::kInitializing) {
    state_ = State::kRunning;
    CreateCodec();
  }
}

bool OhosVideoDecoder::SurfaceTransitionPending() {
  return codec_ && codec_->SurfaceBundle() != target_surface_bundle_;
}

void OhosVideoDecoder::TransitionToTargetSurface() {
  LOG(INFO) << "OhosVideoDecoder::TransitionToTargetSurface";
  DCHECK(SurfaceTransitionPending());

  if (!codec_->SetSurface(target_surface_bundle_)) {
    video_frame_factory_->SetSurfaceBundle(nullptr);
    EnterTerminalState(State::kError, "Could not switch codec output surface");
    return;
  }

  video_frame_factory_->SetSurfaceBundle(target_surface_bundle_);
}

void OhosVideoDecoder::CreateCodec() {
  DCHECK(!codec_);
  DCHECK(target_surface_bundle_);
  DCHECK_EQ(state_, State::kRunning);

  auto config = std::make_unique<VideoBridgeCodecConfig>();
  if (!config) {
    LOG(ERROR) << __FUNCTION__ << " [WiseplayDRM] config is null";
    return;
  }

  config->codec = decoder_config_.codec();
  config->on_buffers_available_cb =
      base::BindPostTaskToCurrentDefault(base::BindRepeating(
          &OhosVideoDecoder::PumpCodec, weak_factory_.GetWeakPtr()));
#if BUILDFLAG(ENABLE_WISEPLAY)
  config->media_key_session = std::move(media_key_session_);
#endif  // BUILDFLAG(ENABLE_WISEPLAY)
  video_frame_factory_->SetSurfaceBundle(target_surface_bundle_);
  codec_allocator_->CreateMediaCodecAsync(
      base::BindOnce(&OhosVideoDecoder::OnCodecConfiguredInternal,
                     codec_allocator_weak_factory_.GetWeakPtr(),
                     codec_allocator_, target_surface_bundle_),
      std::move(config));
}

// static
void OhosVideoDecoder::OnCodecConfiguredInternal(
    base::WeakPtr<OhosVideoDecoder> weak_this,
    CodecAllocator* codec_allocator,
    scoped_refptr<CodecSurfaceBundle> surface_bundle,
    std::unique_ptr<MediaCodecDecoderBridgeImpl> codec) {
  if (!weak_this) {
    if (codec) {
      codec_allocator->ReleaseMediaCodec(
          std::move(codec),
          base::BindOnce(
              &base::SequencedTaskRunner::ReleaseSoon<CodecSurfaceBundle>,
              base::SequencedTaskRunner::GetCurrentDefault(), FROM_HERE,
              std::move(surface_bundle)));
    }
    return;
  }
  weak_this->OnCodecConfigured(std::move(surface_bundle), std::move(codec));
}

void OhosVideoDecoder::OnCodecConfigured(
    scoped_refptr<CodecSurfaceBundle> surface_bundle,
    std::unique_ptr<MediaCodecDecoderBridgeImpl> codec) {
  DCHECK(!codec_);
  DCHECK_EQ(state_, State::kRunning);

  if (!codec) {
    LOG(ERROR) << __FUNCTION__ << " [WiseplayDRM] codec is null";
    EnterTerminalState(State::kError, "Unable to allocate codec");
    return;
  }

  DecoderFormat decoderFormat;
  decoderFormat.width = decoder_config_.coded_size().width();
  decoderFormat.height = decoder_config_.coded_size().height();
  codec->ConfigureBridgeDecoder(decoderFormat,
                                base::SequencedTaskRunner::GetCurrentDefault());
  if (codec->SetBridgeOutputSurface(surface_bundle->GetOHOSNativeWindow()) ==
      DecoderAdapterCode::DECODER_ERROR) {
    LOG(ERROR) << __FUNCTION__ << " [WiseplayDRM] SetBridgeOutputSurface failed:Unable to initialize codec.";
    EnterTerminalState(State::kError, "Unable to initialize codec");
    return;
  }
  codec->PrepareBridgeDecoder();
  codec->StartBridgeDecoder();
  codec_ = std::make_unique<CodecWrapper>(
      CodecSurfacePair(std::move(codec), std::move(surface_bundle)),
      base::BindRepeating(
          &OutputBufferReleased,
          base::BindPostTaskToCurrentDefault(base::BindRepeating(
              &OhosVideoDecoder::PumpCodec, weak_factory_.GetWeakPtr()))),
      base::SequencedTaskRunner::GetCurrentDefault());

  PumpCodec();
}

void OhosVideoDecoder::Decode(scoped_refptr<DecoderBuffer> buffer,
                              DecodeCB decode_cb) {
  if (!buffer) {
    LOG(ERROR) << "[WiseplayDRM] OhosVideoDecoder::Decode buffer is null";
    std::move(decode_cb).Run(DecoderStatus::Codes::kFailed);
    return;
  }
  if (state_ == State::kError) {
    std::move(decode_cb).Run(DecoderStatus::Codes::kFailed);
    return;
  }
  pending_decodes_.emplace_back(std::move(buffer), std::move(decode_cb));

  if (state_ == State::kInitializing) {
    if (lazy_init_pending_) {
      StartLazyInit();
    }
    return;
  }
  PumpCodec();
}

void OhosVideoDecoder::FlushCodec() {
  DVLOG(2) << "OhosVideoDecoder::FlushCodec";
  deferred_flush_pending_ = false;

  if (deferred_reallocation_pending_) {
    deferred_reallocation_pending_ = false;
    ReleaseCodec();
    CreateCodec();
  }

  if (!codec_ || codec_->IsFlushed()) {
    return;
  }

  if (!codec_->Flush()) {
    EnterTerminalState(State::kError, "Codec flush failed");
  }
}

void OhosVideoDecoder::PumpCodec() {
  bool did_input = false;
  bool did_output = false;
  do {
    did_input = QueueInput();
    did_output = DequeueOutput();
  } while (did_input || did_output);
}

bool OhosVideoDecoder::QueueInput() {
  if (!codec_) {
    LOG(ERROR) << __func__ << "[WiseplayDRM] codec_ is null";
    return false;
  }

#if BUILDFLAG(ENABLE_WISEPLAY)
  if (waiting_for_key_) {
    LOG(INFO) << __func__ << "[WiseplayDRM]  wait for key";
    return false;
  }
#endif  // BUILDFLAG(ENABLE_WISEPLAY)

  if (codec_->IsDrained() || deferred_flush_pending_) {
    if (!codec_->HasUnreleasedOutputBuffers() && !pending_decodes_.empty()) {
      FlushCodec();
      return true;
    }
    return false;
  }

  if (pending_decodes_.empty()) {
    return false;
  }

  PendingDecode& pending_decode = pending_decodes_.front();

  auto status = codec_->QueueInputBuffer(*pending_decode.buffer);
  // fix
  switch (status) {
    case CodecWrapper::QueueStatus::kOk:
      break;
#if BUILDFLAG(ENABLE_WISEPLAY)
    case CodecWrapper::QueueStatus::kNoKey:
      // Retry when a key is added.
      waiting_for_key_ = true;
      waiting_cb_.Run(WaitingReason::kNoDecryptionKey);
      return false;
#endif  // BUILDFLAG(ENABLE_WISEPLAY)
    case CodecWrapper::QueueStatus::kTryAgainLater:
      return false;
    case CodecWrapper::QueueStatus::kError:
    default:
      EnterTerminalState(State::kError, "QueueInputBuffer failed");
      return false;
  }

  if (pending_decode.buffer->end_of_stream()) {
    DCHECK(!eos_decode_cb_);
    eos_decode_cb_ = std::move(pending_decode.decode_cb);
  } else {
    std::move(pending_decode.decode_cb).Run(DecoderStatus::Codes::kOk);
  }
  pending_decodes_.pop_front();
  return true;
}

bool OhosVideoDecoder::DequeueOutput() {
  if (!codec_ || codec_->IsDrained()) {
    LOG(ERROR) << "[WiseplayDRM] OhosVideoDecoder::DequeueOutput failed";
    return false;
  }

#if BUILDFLAG(ENABLE_WISEPLAY)
  if (waiting_for_key_) {
    LOG(ERROR) << "[WiseplayDRM] OhosVideoDecoder::DequeueOutput failed";
    return false;
  }
#endif  // BUILDFLAG(ENABLE_WISEPLAY)

  base::TimeDelta presentation_time;
  bool eos = false;
  std::unique_ptr<CodecOutputBuffer> output_buffer;
  auto status =
      codec_->DequeueOutputBuffer(&presentation_time, &eos, &output_buffer);
  switch (status) {
    case CodecWrapper::DequeueStatus::kOk:
      break;
    case CodecWrapper::DequeueStatus::kTryAgainLater:
      return false;
    case CodecWrapper::DequeueStatus::kError:
      EnterTerminalState(State::kError, "DequeueOutputBuffer failed");
      return false;
  }
  DVLOG(3) << "DequeueOutputBuffer(): pts="
           << (eos ? "EOS" : std::to_string(presentation_time.InMilliseconds()));

  if (eos) {
    if (eos_decode_cb_) {
      video_frame_factory_->RunAfterPendingVideoFrames(
          base::BindOnce(&OhosVideoDecoder::RunEosDecodeCb,
                         weak_factory_.GetWeakPtr(), reset_generation_));
    }
    if (drain_type_) {
      OnCodecDrained();
    }
    LOG(ERROR) << " [WiseplayDRM] OhosVideoDecoder::DequeueOutput is drained";
    return false;
  }

  if (drain_type_ || deferred_flush_pending_) {
    return true;
  }

  if (output_buffer->size().GetArea() >
      decoder_config_.coded_size().GetArea()) {
    decoder_config_.set_coded_size(output_buffer->size());
  }

  gfx::Rect visible_rect(output_buffer->size());
  std::unique_ptr<ScopedAsyncTrace> async_trace =
      ScopedAsyncTrace::CreateIfEnabled("OhosVideoDecoder::CreateVideoFrame");

  if (!is_surface_control_enabled_) {
    LOG(WARNING) << __FUNCTION__ << " [WiseplayDRM] is_surface_control_enabled_ not enabled."
                 << "Calling OhosVideoDecoder::PumpCodec() later.";
    output_buffer->set_render_cb(
        base::BindPostTaskToCurrentDefault(base::BindOnce(
            &OhosVideoDecoder::PumpCodec, weak_factory_.GetWeakPtr())));
  }
  video_frame_factory_->CreateVideoFrame(
      std::move(output_buffer), presentation_time,
      decoder_config_.aspect_ratio().GetNaturalSize(visible_rect),
      base::BindOnce(&OhosVideoDecoder::ForwardVideoFrame,
                     weak_factory_.GetWeakPtr(), reset_generation_,
                     std::move(async_trace), base::TimeTicks::Now()));
  return true;
}

void OhosVideoDecoder::RunEosDecodeCb(int reset_generation) {
  if (reset_generation == reset_generation_ && eos_decode_cb_) {
    std::move(eos_decode_cb_).Run(DecoderStatus::Codes::kOk);
  }
}

void OhosVideoDecoder::ForwardVideoFrame(
    int reset_generation,
    std::unique_ptr<ScopedAsyncTrace> async_trace,
    base::TimeTicks started_at,
    scoped_refptr<VideoFrame> frame) {
  if (!frame) {
    EnterTerminalState(State::kError, "Could not create VideoFrame");
    return;
  }
  if (reset_generation == reset_generation_) {
    frame->metadata().power_efficient = true;
    output_cb_.Run(std::move(frame));
  }
}

void OhosVideoDecoder::Reset(base::OnceClosure closure) {
  DCHECK(!reset_cb_);
  reset_generation_++;
  reset_cb_ = std::move(closure);
  CancelPendingDecodes(DecoderStatus::Codes::kAborted);
  StartDrainingCodec(DrainType::kForReset);
}

void OhosVideoDecoder::StartDrainingCodec(DrainType drain_type) {
  TRACE_EVENT0("media", "OhosVideoDecoder::StartDrainingCodec");
  DCHECK(pending_decodes_.empty());
  drain_type_ = drain_type;

  if (codec_) {
    codec_->DiscardOutputBuffers();
  }

  if (decoder_config_.codec() != VideoCodec::kVP8 || !codec_ ||
      codec_->IsFlushed() || codec_->IsDrained()) {
    deferred_flush_pending_ =
        codec_ && !codec_->IsDrained() && !codec_->IsFlushed();
    OnCodecDrained();
    return;
  }

  if (!codec_->IsDraining()) {
    pending_decodes_.push_back(PendingDecode::CreateEos());
  }

  PumpCodec();
}

void OhosVideoDecoder::OnCodecDrained() {
  TRACE_EVENT0("media", "OhosVideoDecoder::OnCodecDrained");
  DrainType drain_type = *drain_type_;
  drain_type_.reset();

  if (drain_type == DrainType::kForDestroy) {
    base::SequencedTaskRunner::GetCurrentDefault()->DeleteSoon(FROM_HERE, this);
    return;
  }

  std::move(reset_cb_).Run();

  if (codec_ && !codec_->IsFlushed() && !codec_->IsDrained() &&
      !deferred_flush_pending_) {
    FlushCodec();
  }
}

void OhosVideoDecoder::EnterTerminalState(State state, const char* reason) {
  state_ = state;
  DCHECK(InTerminalState());

  // Cancel pending codec creation.
  codec_allocator_weak_factory_.InvalidateWeakPtrs();
  pump_codec_timer_.Stop();
  ReleaseCodec();
  target_surface_bundle_ = nullptr;
  texture_owner_bundle_ = nullptr;
  if (state == State::kError) {
    if (reason != nullptr) {
      LOG(ERROR) << __func__ << "[OHOSVideoDecoder] reason: " << reason;
    } else {
      LOG(ERROR) << __func__ << "[OHOSVideoDecoder] unknown reason.";
    }
    CancelPendingDecodes(DecoderStatus::Codes::kFailed);
  }
  if (drain_type_) {
    OnCodecDrained();
  }
}

bool OhosVideoDecoder::InTerminalState() {
  return state_ == State::kSurfaceDestroyed || state_ == State::kError;
}

void OhosVideoDecoder::CancelPendingDecodes(DecoderStatus status) {
  for (auto& pending_decode : pending_decodes_) {
    std::move(pending_decode.decode_cb).Run(status);
  }
  pending_decodes_.clear();
  if (eos_decode_cb_) {
    std::move(eos_decode_cb_).Run(status);
  }
}

void OhosVideoDecoder::ReleaseCodec() {
  if (!codec_) {
    return;
  }
  auto pair = codec_->TakeCodecSurfacePair();
  codec_ = nullptr;
  codec_allocator_->ReleaseMediaCodec(
      std::move(pair.first),
      base::BindOnce(
          &base::SequencedTaskRunner::ReleaseSoon<CodecSurfaceBundle>,
          base::SequencedTaskRunner::GetCurrentDefault(), FROM_HERE,
          std::move(pair.second)));
}

VideoDecoderType OhosVideoDecoder::GetDecoderType() const {
  return VideoDecoderType::kMediaCodec;
}

bool OhosVideoDecoder::NeedsBitstreamConversion() const {
  return true;
}

bool OhosVideoDecoder::CanReadWithoutStalling() const {
  return false;
}

int OhosVideoDecoder::GetMaxDecodeRequests() const {
  return 2;
}

}  // namespace media
