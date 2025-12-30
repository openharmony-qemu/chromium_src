// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Based on codec_wrapper.cc originally written by
// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. 

#include "media/gpu/ohos/codec_wrapper.h"

#include <stddef.h>
#include <stdint.h>

#include <string>
#include <vector>

#include "base/debug/crash_logging.h"
#include "base/functional/bind.h"
#include "base/memory/ptr_util.h"
#include "base/metrics/histogram_macros.h"
#include "base/task/sequenced_task_runner.h"
#include "base/trace_event/trace_event.h"
#include "third_party/abseil-cpp/absl/types/optional.h"

namespace media {
class CodecWrapperImpl : public base::RefCountedThreadSafe<CodecWrapperImpl> {
 public:
  CodecWrapperImpl(
      CodecSurfacePair codec_surface_pair,
      CodecWrapper::OutputReleasedCB output_buffer_release_cb,
      scoped_refptr<base::SequencedTaskRunner> release_task_runner);

  CodecWrapperImpl(const CodecWrapperImpl&) = delete;
  CodecWrapperImpl& operator=(const CodecWrapperImpl&) = delete;

  using DequeueStatus = CodecWrapper::DequeueStatus;
  using QueueStatus = CodecWrapper::QueueStatus;

  CodecSurfacePair TakeCodecSurfacePair();
  bool HasUnreleasedOutputBuffers() const;
  void DiscardOutputBuffers();
  bool IsFlushed() const;
  bool IsDraining() const;
  bool IsDrained() const;
  bool Flush();
  bool SetSurface(scoped_refptr<CodecSurfaceBundle> surface_bundle);
#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
  void SetVideoSurface(int32_t widget_id, bool is_surface_pending);
  void SetPreciseSeekTarget(int64_t target_timestamp);
  void PendingSetVideoSurface();
  bool PreciseSeek(int64_t id);
  void OnBufferRendered();
#endif // ARKWEB_VIDEO_ASSISTANT
#if BUILDFLAG(ARKWEB_MEDIA_DMABUF)
  void RecycleDmaBuffer();
  void ResumeDmaBuffer();
#endif  // ARKWEB_MEDIA_DMABUF
  scoped_refptr<CodecSurfaceBundle> SurfaceBundle();
  QueueStatus QueueInputBuffer(const DecoderBuffer& buffer);
  DequeueStatus DequeueOutputBuffer(
      base::TimeDelta* presentation_time,
      bool* end_of_stream,
      std::unique_ptr<CodecOutputBuffer>* codec_buffer);
  bool ReleaseCodecOutputBuffer(int64_t id, bool render);
  bool SetDecryptionConfig(void *session, bool isSecure);

 private:
  enum class State {
    kError,
    kFlushed,
    kRunning,
    kDraining,
    kDrained,
  };

  friend base::RefCountedThreadSafe<CodecWrapperImpl>;
  ~CodecWrapperImpl();

  void DiscardOutputBuffers_Locked();

  mutable base::Lock lock_;

  State state_;

  scoped_refptr<CodecSurfaceBundle> surface_bundle_;

  std::unique_ptr<MediaCodecDecoderBridgeImpl> codec_;

  int64_t next_buffer_id_;

  base::flat_map<int64_t, int> buffer_ids_;

  absl::optional<int> owned_input_buffer_;

  gfx::Size size_;

  CodecWrapper::OutputReleasedCB output_buffer_release_cb_;

  bool elided_eos_pending_ = false;

  gfx::ColorSpace color_space_ = gfx::ColorSpace::CreateSRGB();

  scoped_refptr<base::SequencedTaskRunner> release_task_runner_;

#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
  bool render_video_view_ = false;
  std::unordered_map<int64_t, int64_t> buffer_timestamp_map_;
  bool is_surface_pending_ = false;
  bool did_last_outputbuffer_rendered_ = false;
  int64_t target_timestamp_ = -1;
  int32_t pending_surface_id_ = -1;
#endif // ARKWEB_VIDEO_ASSISTANT
};

#ifndef ARKWEB_TEST_INCLUDE
CodecOutputBuffer::CodecOutputBuffer(scoped_refptr<CodecWrapperImpl> codec,
                                     int64_t id,
                                     const gfx::Size& size,
#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
                                     bool render_video_view,
#endif // ARKWEB_VIDEO_ASSISTANT
                                     const gfx::ColorSpace& color_space)
    : codec_(std::move(codec)),
      id_(id),
      size_(size),
#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
      render_video_view_(render_video_view),
#endif // ARKWEB_VIDEO_ASSISTANT
      color_space_(color_space) {}

CodecOutputBuffer::~CodecOutputBuffer() {
  if (!was_rendered_ && codec_) {
    codec_->ReleaseCodecOutputBuffer(id_, false);
  }
}

bool CodecOutputBuffer::ReleaseToSurface() {
  was_rendered_ = true;
  auto result = codec_->ReleaseCodecOutputBuffer(id_, true);
  return result;
}

#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
void CodecOutputBuffer::OnBufferRendered() {
  if (codec_) {
    codec_->OnBufferRendered();
  }
}
#endif // ARKWEB_VIDEO_ASSISTANT

CodecWrapperImpl::CodecWrapperImpl(
    CodecSurfacePair codec_surface_pair,
    CodecWrapper::OutputReleasedCB output_buffer_release_cb,
    scoped_refptr<base::SequencedTaskRunner> release_task_runner)
    : state_(State::kFlushed),
      surface_bundle_(std::move(codec_surface_pair.second)),
      codec_(std::move(codec_surface_pair.first)),
      next_buffer_id_(0),
      output_buffer_release_cb_(std::move(output_buffer_release_cb)),
      release_task_runner_(std::move(release_task_runner)) {
  size_ = gfx::Size(codec_->GetConfigWidth(), codec_->GetConfigHeight());
}

CodecWrapperImpl::~CodecWrapperImpl() = default;

CodecSurfacePair CodecWrapperImpl::TakeCodecSurfacePair() {
  base::AutoLock l(lock_);
  if (!codec_) {
    return {nullptr, nullptr};
  }
  DiscardOutputBuffers_Locked();
  return {std::move(codec_), std::move(surface_bundle_)};
}

bool CodecWrapperImpl::IsFlushed() const {
  base::AutoLock l(lock_);
  return state_ == State::kFlushed;
}

bool CodecWrapperImpl::IsDraining() const {
  base::AutoLock l(lock_);
  return state_ == State::kDraining;
}

bool CodecWrapperImpl::IsDrained() const {
  base::AutoLock l(lock_);
  return state_ == State::kDrained;
}

bool CodecWrapperImpl::HasUnreleasedOutputBuffers() const {
  base::AutoLock l(lock_);
  return !buffer_ids_.empty();
}

void CodecWrapperImpl::DiscardOutputBuffers() {
  DVLOG(2) << __func__;
  base::AutoLock l(lock_);
  DiscardOutputBuffers_Locked();
}

void CodecWrapperImpl::DiscardOutputBuffers_Locked() {
  // DVLOG(2) << __func__;
  lock_.AssertAcquired();
  for (auto& kv : buffer_ids_) {
    codec_->ReleaseOutputBuffer(kv.second, false);
  }
  buffer_ids_.clear();
#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
  target_timestamp_ = -1;
  did_last_outputbuffer_rendered_ = false;
  buffer_timestamp_map_.clear();
#endif // ARKWEB_VIDEO_ASSISTANT
}

bool CodecWrapperImpl::Flush() {
  // DVLOG(2) << __func__;
  base::AutoLock l(lock_);
  DCHECK(codec_ && state_ != State::kError);

  // Dequeued buffers are invalidated by flushing.
  buffer_ids_.clear();
#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
  target_timestamp_ = -1;
  did_last_outputbuffer_rendered_ = false;
  buffer_timestamp_map_.clear();
#endif // ARKWEB_VIDEO_ASSISTANT
  owned_input_buffer_.reset();
  auto status = codec_->FlushBridgeDecoder();
  if (status == DecoderAdapterCode::DECODER_ERROR) {
    state_ = State::kError;
    return false;
  }
  state_ = State::kFlushed;
  elided_eos_pending_ = false;
  return true;
}

CodecWrapperImpl::QueueStatus CodecWrapperImpl::QueueInputBuffer(
    const DecoderBuffer& buffer) {
  TRACE_EVENT0("media", "CodecWrapperImpl::QueueInputBuffer");
  LOG(DEBUG) << "CodecWrapperImpl::QueueInputBuffer";
  base::AutoLock l(lock_);
  DCHECK(codec_ && state_ != State::kError);
  LOG(DEBUG) << "QueueInputBuffer timedelta = "
             << base::TimeDelta().InMicroseconds();

  if (buffer.end_of_stream()) {
    LOG(DEBUG) << "CodecWrapperImpl::QueueInputBuffe EOS";
    if (state_ == State::kFlushed || state_ == State::kDrained) {
      elided_eos_pending_ = true;
    } else {
      auto res = codec_->QueueInputBufferEOS();
      TRACE_EVENT1("media", "QueueInputBufferEOS End", "result", res);
      if (res == DecoderAdapterCode::DECODER_RETRY) {
        return QueueStatus::kTryAgainLater;
      }
    }

    state_ = State::kDraining;
    return QueueStatus::kOk;
  }
  DecoderAdapterCode status;
  status = codec_->QueueInputBuffer(buffer.data(), buffer.size(),
                                    buffer.timestamp().ToInternalValue(),
                                    buffer.decrypt_config(),
                                    buffer.is_key_frame());
  TRACE_EVENT1("media", "CodecWrapperImpl::QueueInputBuffer End", "result",
               status);
  switch (status) {
    case DecoderAdapterCode::DECODER_OK:
      state_ = State::kRunning;
      return QueueStatus::kOk;
    case DecoderAdapterCode::DECODER_ERROR:
      state_ = State::kError;
      return QueueStatus::kError;
    case DecoderAdapterCode::DECODER_RETRY:
      return QueueStatus::kTryAgainLater;
    default:
      NOTREACHED();
  }
}

CodecWrapperImpl::DequeueStatus CodecWrapperImpl::DequeueOutputBuffer(
    base::TimeDelta* presentation_time,
    bool* end_of_stream,
    std::unique_ptr<CodecOutputBuffer>* codec_buffer) {
  TRACE_EVENT0("media", "CodecWrapperImpl::DequeueOutputBuffer");
  LOG(DEBUG) << "CodecWrapperImpl::DequeueOutputBuffer";
  base::AutoLock l(lock_);
  DCHECK(codec_ && state_ != State::kError);
  DCHECK(!*codec_buffer);

  if (elided_eos_pending_) {
    elided_eos_pending_ = false;
    state_ = State::kDrained;
    if (end_of_stream) {
      *end_of_stream = true;
    }
    return DequeueStatus::kOk;
  }

  for (int attempt = 0; attempt < 3; ++attempt) {
    uint32_t index = 0;
    bool eos = false;
    auto status = codec_->DequeueOutputBuffer(presentation_time, index, eos);
    TRACE_EVENT1("media", "CodecWrapperImpl::DequeueOutputBuffer End", "result",
                 status);
    switch (status) {
      case DecoderAdapterCode::DECODER_OK: {
        if (eos) {
          state_ = State::kDrained;
          auto result = codec_->ReleaseOutputBuffer(index, false);
          TRACE_EVENT1("media", "ReleaseOutputBuffer End", "result", result);
          if (end_of_stream) {
            *end_of_stream = true;
          }
          return DequeueStatus::kOk;
        }

        int64_t buffer_id = next_buffer_id_++;
        buffer_ids_[buffer_id] = static_cast<int>(index);
#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
        buffer_timestamp_map_[buffer_id] = (*presentation_time).InMicroseconds();
#endif // ARKWEB_VIDEO_ASSISTANT

        OHOS::NWeb::DecoderFormat format;
        auto result = codec_->GetOutputFormatBridgeDecoder(format);
        TRACE_EVENT1("media", "GetOutputFormatBridgeDecoder End", "result",
                     result);
        LOG(DEBUG) << "CodecWrapperImpl::DequeueOutputBuffer "
                      "des width: "
                   << format.width << ", height: " << format.height;
        LOG(DEBUG) << "CodecWrapperImpl::DequeueOutputBuffer "
                      "src width: "
                   << codec_->GetConfigWidth()
                   << ", height: " << codec_->GetConfigHeight();
        if (result == DecoderAdapterCode::DECODER_OK) {
          size_ = gfx::Size(format.width, format.height);
        } else {
          LOG(ERROR) << "CodecWrapperImpl::GetOutputFormatBridgeDecoder "
                        "failed.";
          size_ =
              gfx::Size(codec_->GetConfigWidth(), codec_->GetConfigHeight());
        }

        *codec_buffer = base::WrapUnique(new CodecOutputBuffer(this, buffer_id, size_,
#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
                        render_video_view_,
#endif // ARKWEB_VIDEO_ASSISTANT
                        color_space_));
        return DequeueStatus::kOk;
      }
      case DecoderAdapterCode::DECODER_RETRY: {
        return DequeueStatus::kTryAgainLater;
      }
      case DecoderAdapterCode::DECODER_ERROR: {
        state_ = State::kError;
        return DequeueStatus::kError;
      }
    }
  }

  state_ = State::kError;
  return DequeueStatus::kError;
}

bool CodecWrapperImpl::SetSurface(
    scoped_refptr<CodecSurfaceBundle> surface_bundle) {
  LOG(DEBUG) << "CodecWrapperImpl::SetSurface";
  base::AutoLock l(lock_);
  DCHECK(surface_bundle);
  DCHECK(codec_ && state_ != State::kError);

  if (!surface_bundle || codec_->SetBridgeOutputSurface(surface_bundle->GetOHOSNativeWindow()) ==
      DecoderAdapterCode::DECODER_ERROR) {
    state_ = State::kError;
    return false;
  }
  surface_bundle_ = std::move(surface_bundle);
#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
  render_video_view_ = false;
#endif // ARKWEB_VIDEO_ASSISTANT
  return true;
}

#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
void CodecWrapperImpl::SetVideoSurface(int32_t widget_id, bool is_surface_pending) {
  if (!is_surface_pending) {
    if (codec_) {
      codec_->SetVideoSurface(widget_id);
      render_video_view_ = widget_id > 0;
    }
  } else {
    pending_surface_id_ = widget_id;
    is_surface_pending_ = true;
  }
}

void CodecWrapperImpl::SetPreciseSeekTarget(int64_t target_timestamp) {
  LOG(DEBUG) << "CodecWrapperImpl::SetPreciseSeekTarget(" << target_timestamp << ")";
  target_timestamp_ = target_timestamp;
}

void CodecWrapperImpl::PendingSetVideoSurface() {
  if (codec_) {
    TRACE_EVENT0("media", "CodecWrapperImpl::PendingSetVideoSurface");
    codec_->SetVideoSurface(pending_surface_id_);
    render_video_view_ = pending_surface_id_ > 0;
    pending_surface_id_ = -1;
    is_surface_pending_ = false;
  }
}

bool CodecWrapperImpl::PreciseSeek(int64_t id) {
  auto it = buffer_timestamp_map_.find(id);
  if (it == buffer_timestamp_map_.end()) {
    LOG(WARNING) << "Buffer id not found in timestamp map.";
    return false;
  }

  if (it->second < target_timestamp_)
    return false;

  target_timestamp_ = -1;
  return true;
}

void CodecWrapperImpl::OnBufferRendered() {
  base::AutoLock l(lock_);
  if (is_surface_pending_ && did_last_outputbuffer_rendered_) {
    PendingSetVideoSurface();
  }
}
#endif // ARKWEB_VIDEO_ASSISTANT

#if BUILDFLAG(ARKWEB_MEDIA_DMABUF)
void CodecWrapperImpl::RecycleDmaBuffer() {
  if (codec_) {
    LOG(INFO) << "DMABUF::CodecWrapperImpl, RecycleDmaBuffer";
    codec_->RecycleDmaBuffer();
  }
}

void CodecWrapperImpl::ResumeDmaBuffer() {
  if (codec_) {
    LOG(INFO) << "DMABUF::CodecWrapperImpl, ResumeDmaBuffer";
    codec_->ResumeDmaBuffer();
  }
}
#endif  // ARKWEB_MEDIA_DMABUF

scoped_refptr<CodecSurfaceBundle> CodecWrapperImpl::SurfaceBundle() {
  base::AutoLock l(lock_);
  return surface_bundle_;
}

bool CodecWrapperImpl::ReleaseCodecOutputBuffer(int64_t id, bool render) {
  LOG(DEBUG) << "CodecWrapperImpl::ReleaseCodecOutputBuffer " << render;
  if (!render && release_task_runner_ &&
      !release_task_runner_->RunsTasksInCurrentSequence()) {
    release_task_runner_->PostTask(
        FROM_HERE,
        base::BindOnce(
            base::IgnoreResult(&CodecWrapperImpl::ReleaseCodecOutputBuffer),
            this, id, render));
    return true;
  }

  base::AutoLock l(lock_);
  if (!codec_ || state_ == State::kError) {
    return false;
  }

  auto buffer_it = buffer_ids_.find(id);
  bool valid = buffer_it != buffer_ids_.end();
  LOG(DEBUG) << __func__ << " id=" << id << " render=" << render
             << " valid=" << valid;
  if (!valid) {
    return false;
  }

  int index = buffer_it->second;
#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
  if (render && target_timestamp_ > -1) {
    render = PreciseSeek(id);
  }
#endif // ARKWEB_VIDEO_ASSISTANT
  codec_->ReleaseOutputBuffer(index, render);
  buffer_ids_.erase(buffer_it);

#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
  did_last_outputbuffer_rendered_ = render;
  buffer_timestamp_map_.erase(id);
#endif // ARKWEB_VIDEO_ASSISTANT
  return true;
}

bool CodecWrapperImpl::SetDecryptionConfig(void *session, bool isSecure)
{
  auto status = codec_->SetDecryptionConfig(session, isSecure);
  return status == DecoderAdapterCode::DECODER_OK;
}

CodecWrapper::CodecWrapper(
    CodecSurfacePair codec_surface_pair,
    OutputReleasedCB output_buffer_release_cb,
    scoped_refptr<base::SequencedTaskRunner> release_task_runner)
    : impl_(new CodecWrapperImpl(std::move(codec_surface_pair),
                                 std::move(output_buffer_release_cb),
                                 std::move(release_task_runner))) {}

CodecWrapper::~CodecWrapper() {
  DCHECK(!impl_->TakeCodecSurfacePair().first);
}

CodecSurfacePair CodecWrapper::TakeCodecSurfacePair() {
  return impl_->TakeCodecSurfacePair();
}

bool CodecWrapper::HasUnreleasedOutputBuffers() const {
  return impl_->HasUnreleasedOutputBuffers();
}

void CodecWrapper::DiscardOutputBuffers() {
  impl_->DiscardOutputBuffers();
}

bool CodecWrapper::IsFlushed() const {
  return impl_->IsFlushed();
}

bool CodecWrapper::IsDraining() const {
  return impl_->IsDraining();
}

bool CodecWrapper::IsDrained() const {
  return impl_->IsDrained();
}

bool CodecWrapper::Flush() {
  return impl_->Flush();
}

CodecWrapper::QueueStatus CodecWrapper::QueueInputBuffer(
    const DecoderBuffer& buffer) {
  return impl_->QueueInputBuffer(buffer);
}

CodecWrapper::DequeueStatus CodecWrapper::DequeueOutputBuffer(
    base::TimeDelta* presentation_time,
    bool* end_of_stream,
    std::unique_ptr<CodecOutputBuffer>* codec_buffer) {
  return impl_->DequeueOutputBuffer(presentation_time, end_of_stream,
                                    codec_buffer);
}

bool CodecWrapper::SetSurface(
    scoped_refptr<CodecSurfaceBundle> surface_bundle) {
  if (!impl_) {
    LOG(ERROR) << "CodecWrapper::SetSurface, impl_ is nullptr";
    return false;
  }
  return impl_->SetSurface(std::move(surface_bundle));
}

#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
void CodecWrapper::SetVideoSurface(int32_t widget_id, bool is_surface_pending) {
  impl_->SetVideoSurface(widget_id, is_surface_pending);
}

void CodecWrapper::SetPreciseSeekTarget(int64_t target_timestamp) {
  impl_->SetPreciseSeekTarget(target_timestamp);
}
#endif // ARKWEB_VIDEO_ASSISTANT

#if BUILDFLAG(ARKWEB_MEDIA_DMABUF)
void CodecWrapper::RecycleDmaBuffer() {
  if (impl_) {
    impl_->RecycleDmaBuffer();
  }
}

void CodecWrapper::ResumeDmaBuffer() {
  if (impl_) {
    impl_->ResumeDmaBuffer();
  }
}
#endif  // ARKWEB_MEDIA_DMABUF

scoped_refptr<CodecSurfaceBundle> CodecWrapper::SurfaceBundle() {
  return impl_->SurfaceBundle();
}

bool CodecWrapper::SetDecryptionConfig(void *session, bool isSecure)
{
  LOG(INFO) << __func__;
  return impl_->SetDecryptionConfig(session, isSecure);
}
#endif // ARKWEB_TEST_INCLUDE

}  // namespace media
