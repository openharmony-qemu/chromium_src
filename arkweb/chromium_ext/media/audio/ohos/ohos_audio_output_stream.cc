// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "media/audio/ohos/ohos_audio_output_stream.h"

#include "content/public/browser/browser_task_traits.h"
#include "content/public/browser/browser_thread.h"
#include "media/base/audio_timestamp_helper.h"
#include "content/browser/media/session/media_session_impl.h"
#include "content/public/browser/web_contents.h"
#include "arkweb/chromium_ext/base/ohos/sys_info_utils_ext.h"

namespace media {

std::set<content::WebContents*> OHOSAudioOutputStream::webContentSet_ = {};

OHOSAudioOutputStream::OHOSAudioOutputStream(OHOSAudioManager* manager,
                                             const AudioParameters& parameters,
                                             bool isCommunication)
    : manager_(manager),
      parameters_(parameters),
      ns_per_frame_(base::Time::kNanosecondsPerSecond /
                    static_cast<double>(parameters.sample_rate())),
      audio_bus_(AudioBus::Create(parameters)),
      isCommunication_(isCommunication) {
  content::RenderFrameHost* renderFrameHost = content::RenderFrameHost::FromID(
      parameters_.render_process_id(), parameters_.render_frame_id());
  webContent_ = content::WebContents::FromRenderFrameHost(renderFrameHost);
  if (!webContent_) {
    LOG(ERROR) << "AudioOutputStream get webContent failed.";
  } else {
    content::MediaSessionImpl* mediaSession =
        content::MediaSessionImpl::Get(webContent_);
    if (!mediaSession) {
      LOG(ERROR) << "AudioOutputStream get mediaSession failed.";
    } else {
      weakMediaSession_ = mediaSession->weakMediaSessionFactory_.GetWeakPtr();
      if (!weakMediaSession_) {
        LOG(ERROR) << "OHOSAudioOutputStream::OHOSAudioOutputStream weakMediaSession get failed";
      }
    }
  }
  OH_AudioStreamBuilder* tempAudioStreamBuilder = audio_stream_builder_.get();
  OH_AudioStream_Result ret = OH_AudioStreamBuilder_Create(
      &tempAudioStreamBuilder, AUDIOSTREAM_TYPE_RENDERER);
  audio_stream_builder_ = tempAudioStreamBuilder;
  if (ret != AUDIOSTREAM_SUCCESS) {
    LOG(ERROR) << "AudioStreamBuilder create failed.";
  }

  sample_format_ = kSampleFormatS16;
  bytes_per_frame_ = parameters.GetBytesPerFrame(sample_format_);
  buffer_size_bytes_ = parameters.GetBytesPerBuffer(sample_format_);
  time_per_buffer_ = AudioTimestampHelper::FramesToTime(parameters_.frames_per_buffer(),
                                                        parameters_.sample_rate());
  main_task_runner_ = content::GetUIThreadTaskRunner({});
}

OHOSAudioOutputStream::~OHOSAudioOutputStream() {
  LOG(INFO) << "OHOSAudioOutputStream::~OHOSAudioOutputStream";
  isDestroyed_.store(true);
  {
    // Ensure that OnWriteData can exit quickly and does not block the destructor.
    base::AutoLock lock(lock_);
    running_ = false;
  }
  // Close() must be called first.
  if (audio_renderer_ != nullptr) {
    OH_AudioRenderer_Release(audio_renderer_);
    audio_renderer_ = nullptr;
  }
  if (audio_stream_builder_ != nullptr) {
    OH_AudioStreamBuilder_Destroy(audio_stream_builder_);
    audio_stream_builder_ = nullptr;
  }
}

bool OHOSAudioOutputStream::Open() {
  LOG(INFO) << "OHOSAudioOutputStream::Open";
  base::AutoLock lock(lock_);
  if (!InitRender()) {
    return false;
  }
  return true;
}

void OHOSAudioOutputStream::Close() {
  Stop();
  manager_->ReleaseOutputStream(this);
}

static int32_t AudioRendererOnWriteData(OH_AudioRenderer* renderer,
                                        void* userData,
                                        void* buffer,
                                        int32_t length) {
  if (userData && buffer) {
    ((OHOSAudioOutputStream*)(userData))->OnWriteData(buffer, length);
    ((OHOSAudioOutputStream*)(userData))->SetUpAudioSilentState();
  }
  return 0;
}

static int32_t AudioRendererOnError(OH_AudioRenderer* renderer,
                                    void* userData,
                                    OH_AudioStream_Result error) {
  if (userData) {
    ((OHOSAudioOutputStream*)(userData))->ReportError();
  }
  return 0;
}

static int32_t AudioRendererOnInterruptEvent(OH_AudioRenderer* renderer,
                                             void* userData,
                                             OH_AudioInterrupt_ForceType type,
                                             OH_AudioInterrupt_Hint hint) {
  LOG(INFO) << "AudioRenderer on interrupt type:" << type << "hint:" << hint;

  if (userData) {
    switch (hint) {
        case OH_AudioInterrupt_Hint::AUDIOSTREAM_INTERRUPT_HINT_PAUSE:
            ((OHOSAudioOutputStream*)(userData))->OnSuspend();
            break;
        case OH_AudioInterrupt_Hint::AUDIOSTREAM_INTERRUPT_HINT_STOP:
            ((OHOSAudioOutputStream*)(userData))->OnSuspend();
            break;
        case OH_AudioInterrupt_Hint::AUDIOSTREAM_INTERRUPT_HINT_RESUME:
            ((OHOSAudioOutputStream*)(userData))->OnResume();
            break;
        default:
            LOG(ERROR) << "audio renderer interrupt hint not foud, code:" << hint;
            break;
    }
  }
  return 0;
}

void OHOSAudioOutputStream::OnSuspend() {
    LOG(INFO) << "AudioRendererCallback::OnSuspend";

    if (!weakMediaSession_) {
        LOG(ERROR) << "AudioRendererCallback::OnSuspend media_session_ is null.";
        return;
    }
    if (weakMediaSession_.get()->IsActive()) {
        if (weakMediaSession_->audioResumeInterval_ != 0) {
            intervalSinceLastSuspend_ = std::time(nullptr);
        }
        if (!main_task_runner_) {
            LOG(INFO) << "main task runner is nullptr";
            return;
        }
        main_task_runner_->PostTask(
            FROM_HERE,
            base::BindOnce(&content::MediaSessionImpl::Suspend,
                           weakMediaSession_,
                           content::MediaSession::SuspendType::kSystem));
        isSuspended_ = true;
        // After stopping playback, it is necessary to continue obtaining audio data,
        // which will trigger the pause action of the render process.
        main_task_runner_->PostTask(FROM_HERE,
                                    base::BindOnce(&OHOSAudioOutputStream::PumpSamples,
                                                   base::Unretained(this)));
    } else {
        LOG(INFO) << "media session is not active.";
#if BUILDFLAG(ARKWEB_PERFORMANCE_PERSISTENT_TASK)
        if (weakMediaSession_.get()->HasOnlyOneShotPlayersPublic() ||
            weakMediaSession_.get()->GetSessionState() == content::MediaSessionImpl::NWebMediaSessionState::NOINITIAL) {

          if (!webContent_) {
            LOG(ERROR) << "AudioOutputStream get webContent failed.";
            return;
          }
          webContent_->OneShotMediaPlayerStopped();
        }
#endif
    }
}

void OHOSAudioOutputStream::OnResume() {
    LOG(DEBUG) << "AudioRendererCallback::OnResume audioResumeInterval is: "
               << std::time(nullptr) - intervalSinceLastSuspend_;
    if (!weakMediaSession_) {
        LOG(ERROR) << "AudioRendererCallback::OnResume media_session_ is null.";
        return;
    }
    if (isNeedResume(weakMediaSession_->audioResumeInterval_) &&
        weakMediaSession_->IsSuspended()) {
        if (!main_task_runner_) {
            return;
        }
        main_task_runner_->PostTask(
            FROM_HERE,
            base::BindOnce(&content::MediaSessionImpl::Resume,
                           weakMediaSession_,
                           content::MediaSession::SuspendType::kSystem));
    }
}

bool OHOSAudioOutputStream::isNeedResume(int32_t resumeInterval) {
  return resumeInterval < 0 ||
      (resumeInterval > 0 &&
      std::time(nullptr) - intervalSinceLastSuspend_ <=
      static_cast<double>(resumeInterval));
}

void OHOSAudioOutputStream::SuspendOtherMediaSession(
    base::WeakPtr<content::MediaSessionImpl> weakMediaSession) {
  auto it = OHOSAudioOutputStream::webContentSet_.begin();
  while (it != OHOSAudioOutputStream::webContentSet_.end()) {
    auto otherMediaSession = content::MediaSessionImpl::FromWebContents(*it);
    if (!otherMediaSession) {
      it = OHOSAudioOutputStream::webContentSet_.erase(it);
      continue;
    }
    if (otherMediaSession == weakMediaSession_.get()) {
      LOG(INFO) << "skip mediaSession control because of same mediaSession.";
      it++;
      continue;
    }
    if (GetInterruptMode() && otherMediaSession->IsActive() && !IsPreloadOrMutedMediaMode()) {
      LOG(INFO) << "MediaSession is suspending the audio in other web.";
      main_task_runner_->PostTask(
          FROM_HERE,
          base::BindOnce(&content::MediaSessionImpl::Suspend, otherMediaSession->weakMediaSessionFactory_.GetWeakPtr(),
                         content::MediaSession::SuspendType::kSystem));
    }
    it++;
  }

  OHOSAudioOutputStream::webContentSet_.insert(webContent_);
}

void OHOSAudioOutputStream::Start(AudioSourceCallback* callback) {
  LOG(INFO) << "OHOSAudioOutputStream::Start";
  base::AutoLock lock(lock_);
  DCHECK(!callback_);
  DCHECK(reference_time_.is_null());
  isSuspended_ = false;

  callback_ = callback;
  if (!StartRender()) {
    callback_ = nullptr;
    LOG(ERROR) << "OHOSAudioOutputStream::StartRender failed";
    return;
  }
  SuspendOtherMediaSession(weakMediaSession_);
  running_ = true;
}

void OHOSAudioOutputStream::Stop() {
  LOG(INFO) << "OHOSAudioOutputStream::Stop";
  base::AutoLock lock(lock_);
  timer_.Stop();
  running_ = false;
  OHOSAudioOutputStream::webContentSet_.erase(webContent_);
  if (!audio_renderer_) {
    return;
  }
  callback_ = nullptr;
  if (!reference_time_.is_null()) {
    reference_time_ = base::TimeTicks();
  }
  if (isSuspended_) {
    LOG(INFO) << "OHOSAudioOutputStream::Stop stream is suspended, cannot stop";
    return;
  }
  OH_AudioStream_Result ret = OH_AudioRenderer_Stop(audio_renderer_);
  if (ret != AUDIOSTREAM_SUCCESS) {
    ReportError();
  }
}

bool OHOSAudioOutputStream::GetInterruptMode() {
  return audioExclusive_;
}

void OHOSAudioOutputStream::Refresh() {
  if (!audio_renderer_) {
    LOG(ERROR) << "OHOSAudioOutputStream::Refresh audio_renderer_ is null.";
    return;
  }
  OH_AudioRenderer_Stop(audio_renderer_);
  OH_AudioRenderer_Start(audio_renderer_);
}

void OHOSAudioOutputStream::SetInterruptMode(bool audioExclusive) {
  LOG(INFO) << "OHOSAudioOutputStream::SetInterruptMode audioExclusive: "
            << audioExclusive;
  if (!audio_renderer_) {
    LOG(ERROR)
        << "OHOSAudioOutputStream::SetInterruptMode audio_renderer_ is null.";
    return;
  }
}

// This stream is always used with sub second buffer sizes, where it's
// sufficient to simply always flush upon Start().
void OHOSAudioOutputStream::Flush() {}

void OHOSAudioOutputStream::SetVolume(double volume) {
  if (volume < 0.0 || volume > 1.0) {
    return;
  }
  volume_ = volume;
}

void OHOSAudioOutputStream::GetVolume(double* volume) {
  *volume = volume_;
}

bool OHOSAudioOutputStream::InitRender() {
  // set params
  OH_AudioStreamBuilder_SetSamplingRate(audio_stream_builder_,
                                        parameters_.sample_rate());
  OH_AudioStreamBuilder_SetChannelCount(audio_stream_builder_,
                                        parameters_.channels());
  OH_AudioStreamBuilder_SetLatencyMode(audio_stream_builder_,
                                       AUDIOSTREAM_LATENCY_MODE_NORMAL);
  OH_AudioStreamBuilder_SetFrameSizeInCallback(audio_stream_builder_,
                                               parameters_.frames_per_buffer());
  OH_AudioStreamBuilder_SetEncodingType(audio_stream_builder_, AUDIOSTREAM_ENCODING_TYPE_RAW);
  if (isCommunication_) {
    OH_AudioStreamBuilder_SetRendererInfo(audio_stream_builder_, AUDIOSTREAM_USAGE_VOICE_COMMUNICATION);
  } else {
    OH_AudioStreamBuilder_SetRendererInfo(audio_stream_builder_, AUDIOSTREAM_USAGE_UNKNOWN);
  }
  if (weakMediaSession_) {
    media::MediaContentType contentType = weakMediaSession_.get()->getMediaContentType();
    if (contentType == media::MediaContentType::kTransient) {
      OH_AudioStreamBuilder_SetRendererInfo(audio_stream_builder_, AUDIOSTREAM_USAGE_NAVIGATION);
    }
  }
  // set callback
  OH_AudioRenderer_Callbacks callbacks;
  callbacks.OH_AudioRenderer_OnWriteData = AudioRendererOnWriteData;
  callbacks.OH_AudioRenderer_OnError = AudioRendererOnError;
  callbacks.OH_AudioRenderer_OnInterruptEvent = AudioRendererOnInterruptEvent;
  OH_AudioStreamBuilder_SetRendererCallback(audio_stream_builder_, callbacks,
                                            this);
  // set interrupt mode
  Prepare(weakMediaSession_);

  OH_AudioStream_Result ret;
  // create audio render
  OH_AudioRenderer* tempAudioRenderer = audio_renderer_.get();
  ret = OH_AudioStreamBuilder_GenerateRenderer(audio_stream_builder_,
                                               &tempAudioRenderer);
  audio_renderer_ = tempAudioRenderer;
  if (ret != AUDIOSTREAM_SUCCESS) {
    LOG(ERROR) << "AudioStreamBuilder GenerateRenderer failed.";
    return false;
  }
  return true;
}

void OHOSAudioOutputStream::Prepare(
      base::WeakPtr<content::MediaSessionImpl> weakMediaSession) {
  LOG(INFO) << "OHOSAudioOutputStream::Prepare";
  if (!weakMediaSession) {
    LOG(ERROR) << "OHOSAudioOutputStream::Prepare weakMediaSession is null";
    return;
  }
  content::MediaSessionImpl* mediaSession = weakMediaSession.get();
  if (!mediaSession) {
    LOG(ERROR) << "OHOSAudioOutputStream::Prepare mediaSession is null";
    return;
  }
  audioExclusive_ = mediaSession->audioExclusive_;
  OH_AudioStreamBuilder_SetRendererInterruptMode(audio_stream_builder_, (OH_AudioInterrupt_Mode)false);
  LOG(INFO) << "OHOSAudioOutputStream::SetRendererInterruptMode audioExclusive: " << audioExclusive_;
}

bool OHOSAudioOutputStream::StartRender() {
  if (IsPreloadOrMutedMediaMode()) {
    OH_AudioRenderer_SetSilentModeAndMixWithOthers(audio_renderer_, true);
    LOG(INFO) << "OHOSAudioOutputStream SetAudioSilentMode true";
    isSilentMode_ = true;
  }
  OH_AudioStream_Result ret = OH_AudioRenderer_Start(audio_renderer_);
  if (ret != AUDIOSTREAM_SUCCESS) {
    if (OH_AudioRenderer_Release(audio_renderer_) != AUDIOSTREAM_SUCCESS) {
      LOG(ERROR) << "ohos audio render release failed";
    } else {
      audio_renderer_ = nullptr;
    }
    ReportError();
    return false;
  }
  isSuspended_ = false;
  LOG(INFO) << "OHOSAudioOutputStream::StartRender done";
  return true;
}

void OHOSAudioOutputStream::ReportError() {
  LOG(ERROR) << "ohos audio render error happened";
  if (isDestroyed_.load()) {
    LOG(INFO) << "OHOSAudioOutputStream::ReportError during destroyed";
    return;
  }
  reference_time_ = base::TimeTicks();
  running_ = false;
  timer_.Stop();
  if (callback_) {
    callback_->OnError(AudioSourceCallback::ErrorType::kUnknown);
  }
}

base::TimeDelta OHOSAudioOutputStream::GetDelay(
    base::TimeTicks delay_timestamp) {
  // Get the time that a known audio frame was presented for playing.
  int64_t existing_frame_index;
  int64_t existing_frame_pts;
  OH_AudioStream_Result result =
      OH_AudioRenderer_GetTimestamp(audio_renderer_, CLOCK_MONOTONIC,
                                    &existing_frame_index, &existing_frame_pts);
  if (result != OH_AudioStream_Result::AUDIOSTREAM_SUCCESS) {
    LOG(ERROR) << "Failed to get audio latency, result: " << result;
    return base::TimeDelta();
  }

  // Calculate the number of frames between our known frame and the write index.
  int64_t frames;
  result = OH_AudioRenderer_GetFramesWritten(audio_renderer_, &frames);
  if (result != OH_AudioStream_Result::AUDIOSTREAM_SUCCESS) {
    LOG(ERROR) << "Failed to OH_AudioRenderer_GetFramesWritten, result: "
               << result;
    return base::TimeDelta();
  }
  const int64_t frame_index_delta = frames - existing_frame_index;

  // Calculate the time which the next frame will be presented.
  const base::TimeDelta next_frame_pts =
      base::Nanoseconds(existing_frame_pts + frame_index_delta * ns_per_frame_);

  // Calculate the latency between write time and presentation time. At startup
  // we may end up with negative values here.
  return std::max(base::TimeDelta(),
                  next_frame_pts - (delay_timestamp - base::TimeTicks()));
}

void OHOSAudioOutputStream::PumpSamples() {
    base::AutoLock lock(lock_);
    if (!running_) {
        LOG(INFO) << "The playback is stopped. Exit PumpSamples.";
        return;
    }

    if (!isSuspended_) {
        LOG(INFO) << "Not a focus preemption scenario. Exit PumpSamples.";
        return;
    }
    // Request more samples from |callback_|.
    if (!callback_) {
      LOG(INFO) << "PumpSample failed, callback_ is nullptr";
      ReportError();
      return;
    }

    if (weakMediaSession_.get()->IsActive()) {
        LOG(INFO) << "AudioStream should be restarted";
        if (OH_AudioRenderer_Start(audio_renderer_) == AUDIOSTREAM_SUCCESS) {
            isSuspended_ = false;
            return;
        } else {
            LOG(ERROR) << "Restarted audioStream but failed";
        }
    }

    // Obtains data. The data does not need to be processed and may be empty.
    (void)callback_->OnMoreData(base::TimeDelta(), base::TimeTicks::Now(), {}, audio_bus_.get());
    SchedulePumpSamples();
}

void OHOSAudioOutputStream::OnWriteData(void* buffer, int32_t length) {
  base::AutoLock lock(lock_);
  if (!running_) {
      return;
  }
  base::TimeTicks now = base::TimeTicks::Now();
  base::TimeDelta delay = GetDelay(now);
  if (!callback_) {
    LOG(INFO) << "OnWriteData failed, callback_ is nullptr";
    ReportError();
    return;
  }
  // Request more samples from |callback_|.
  int frames_filled =
      callback_->OnMoreData(delay, now, {}, audio_bus_.get(), false);
  DCHECK_EQ(frames_filled, audio_bus_->frames());
  audio_bus_->Scale(volume_);
  audio_bus_->ToInterleaved<SignedInt16SampleTypeTraits>(
      frames_filled, reinterpret_cast<int16_t*>(buffer));
  if (reference_time_.is_null()) {
    reference_time_ = now;
  }
}

void OHOSAudioOutputStream::SetUpAudioSilentState()
{
  if (!isSilentMode_) {
    return;
  }

  if (!weakMediaSession_ || !audio_renderer_) {
    LOG(ERROR) << "OHOSAudioOutputStream: Try to set audio silent but get mediaSession or audioRender failed!";
    return;
  }

  bool is_playing = weakMediaSession_.get()->IsActive() || weakMediaSession_.get()->GetPlayingState();
  bool is_muted = weakMediaSession_.get()->GetMuteState();
  if (is_playing && !is_muted) {
    OH_AudioRenderer_SetSilentModeAndMixWithOthers(audio_renderer_, false);
    LOG(INFO) << "OHOSAudioOutputStream SetAudioSilentMode false!";
    isSilentMode_ = false;
  }
}

bool OHOSAudioOutputStream::IsPreloadOrMutedMediaMode()
{
  if (base::ohos::IsPcDevice()) {
    LOG(INFO) << "OHOSAudioOutputStream::IsPreloadOrMutedMediaMode device is pc";
    return false;
  }

  if (!weakMediaSession_) {
    return false;
  }

  content::MediaSessionImpl::NWebMediaSessionState sessionState = weakMediaSession_.get()->GetSessionState();
  bool is_active = weakMediaSession_.get()->IsActive();
  bool is_playingState = weakMediaSession_.get()->GetPlayingState();
  bool is_muted = weakMediaSession_.get()->GetMuteState();
  LOG(INFO) << "OHOSAudioOutputStream sessionState:" << static_cast<uint32_t>(sessionState)
      << ", mutedMode:" << is_muted << ", activeMode:" << is_active << ", playingState: " << is_playingState;

  bool is_preload = weakMediaSession_.get()->IsActive() || weakMediaSession_.get()->GetPlayingState();
  if (sessionState == content::MediaSessionImpl::NWebMediaSessionState::NOINITIAL) {
    return false;
  }

  return !is_preload || is_muted;
}

void OHOSAudioOutputStream::SchedulePumpSamples() {
    timer_.Start(FROM_HERE, time_per_buffer_,
                 base::BindOnce(&OHOSAudioOutputStream::PumpSamples,
                                base::Unretained(this)));
}

}  // namespace media
