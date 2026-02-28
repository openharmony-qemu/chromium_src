// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "media/audio/ohos/ohos_audio_output_stream.h"

#include "base/hash/hash.h"
#include "content/public/browser/browser_task_traits.h"
#include "content/public/browser/browser_thread.h"
#include "media/base/audio_timestamp_helper.h"
#include "content/browser/media/session/media_session_impl.h"
#include "content/public/browser/web_contents.h"
#include "arkweb/chromium_ext/base/ohos/sys_info_utils_ext.h"
#include "media/audio/ohos/audio_dump.h"
#include "media/audio/ohos/ohos_audio_focus_controller.h"

namespace media {
std::vector<AudioParameters> OHOSAudioOutputStream::audioParameterSet_ = {};
CallbackSharedWrapper<OHOSAudioOutputCallback> OHOSAudioOutputStream::callback_wrapper_;

OHOSAudioOutputStream::OHOSAudioOutputStream(OHOSAudioManager* manager,
                                             const AudioParameters& parameters,
                                             bool isCommunication)
    : manager_(manager),
      parameters_(parameters),
      ns_per_frame_(base::Time::kNanosecondsPerSecond /
                    static_cast<double>(parameters.sample_rate())),
      audio_bus_(AudioBus::Create(parameters)),
      isCommunication_(isCommunication) {
  OH_AudioStreamBuilder* tempAudioStreamBuilder = audio_stream_builder_.get();
  OH_AudioStream_Result ret = OH_AudioStreamBuilder_Create(
      &tempAudioStreamBuilder, AUDIOSTREAM_TYPE_RENDERER);
  audio_stream_builder_ = tempAudioStreamBuilder;
  if (ret != AUDIOSTREAM_SUCCESS) {
    LOG(ERROR) << "AudioStreamBuilder create failed.";
  }
  if (base::ohos::IsPcDevice() && parameters.latency_tag() != AudioLatency::Type::kRtc) {
    isCommunication_ = false;
  }

  sample_format_ = kSampleFormatS16;
  bytes_per_frame_ = parameters.GetBytesPerFrame(sample_format_);
  buffer_size_bytes_ = parameters.GetBytesPerBuffer(sample_format_);
  time_per_buffer_ = AudioTimestampHelper::FramesToTime(parameters_.frames_per_buffer(),
                                                        parameters_.sample_rate());
  main_task_runner_ = content::GetUIThreadTaskRunner({});
  audio_task_runner_ = base::SingleThreadTaskRunner::GetCurrentDefault();
  audioResumeInterval_ = OHOSAudioFocusController::GetAudioResumeInterval(parameters_);
}

// LCOV_EXCL_START
OHOSAudioOutputStream::~OHOSAudioOutputStream() {
  LOG(INFO) << "OHOSAudioOutputStream::~OHOSAudioOutputStream";
  std::unique_lock<std::shared_mutex> lock_audio_callback(OHOSAudioOutputCallback::GetAudioCallbackMutex());
  if (callback_index_ > 0) {
    callback_wrapper_.Clear(callback_index_);
    callback_index_ = 0;
  }
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
  DumpFileUtil::CloseDumpScopedFile(&dumpFile_);
}

bool OHOSAudioOutputStream::Open() {
  LOG(INFO) << "OHOSAudioOutputStream::Open. [hash: "
            << std::hex << base::FastHash(base::byte_span_from_ref(this)) << "]";
  base::AutoLock lock(lock_);
  if (!InitRender()) {
    return false;
  }
  time_t now = time(nullptr);
  std::string dumpFileName = std::to_string(now) + "_" +
      std::to_string(parameters_.sample_rate()) + "_" +
      std::to_string(parameters_.channels()) + "_" +
      std::to_string(1) + "_output_write.pcm";
  DumpFileUtil::OpenDumpScopedFile(dumpFileName, &dumpFile_);
  return true;
}

void OHOSAudioOutputStream::Close() {
  LOG(INFO) << "OHOSAudioOutputStream::Close. [hash: "
            << std::hex << base::FastHash(base::byte_span_from_ref(this)) << "]";
  Stop();
  DumpFileUtil::CloseDumpScopedFile(&dumpFile_);
  manager_->ReleaseOutputStream(this);
}
// LCOV_EXCL_STOP

int32_t OHOSAudioOutputStream::AudioRendererOnWriteData(OH_AudioRenderer* renderer,
                                        void* userData,
                                        void* buffer,
                                        int32_t length) {
  if (userData && buffer) {
    size_t callback_index = reinterpret_cast<size_t>(userData);
    std::shared_ptr<OHOSAudioOutputCallback> callback = callback_wrapper_.GetCallback(callback_index);
    if (!callback) {
      LOG(ERROR) << "AudioRendererOnWriteData callback is nullptr";
      return 0;
    }
    callback->AudioRendererOnWriteData(buffer, length);
  }
  return 0;
}

int32_t OHOSAudioOutputStream::AudioRendererOnError(OH_AudioRenderer* renderer,
                                                    void* userData,
                                                    OH_AudioStream_Result error) {
  if (userData) {
    size_t callback_index = reinterpret_cast<size_t>(userData);
    std::shared_ptr<OHOSAudioOutputCallback> callback = callback_wrapper_.GetCallback(callback_index);
    if (!callback) {
      LOG(ERROR) << "AudioRendererOnError callback is nullptr";
      return 0;
    }
    callback->AudioRendererOnError(error);
  }
  return 0;
}

int32_t OHOSAudioOutputStream::AudioRendererOnInterruptEvent(OH_AudioRenderer* renderer,
                                             void* userData,
                                             OH_AudioInterrupt_ForceType type,
                                             OH_AudioInterrupt_Hint hint) {
  LOG(INFO) << "AudioRenderer on interrupt type:" << type << "hint:" << hint;

  if (userData) {
    size_t callback_index = reinterpret_cast<size_t>(userData);
    std::shared_ptr<OHOSAudioOutputCallback> callback = callback_wrapper_.GetCallback(callback_index);
    if (!callback) {
      LOG(ERROR) << "AudioRendererOnInterruptEvent callback is nullptr";
      return 0;
    }
    callback->AudioRendererOnInterruptEvent(hint);
  }
  return 0;
}

void OHOSAudioOutputStream::AudioRendererOutputDeviceChangeCallback(
    OH_AudioRenderer* renderer, void* userData, OH_AudioStream_DeviceChangeReason reason) {
  LOG(INFO) << "AudioRenderer on device change reason:" << static_cast<int32_t>(reason);
  if (userData) {
    size_t callback_index = reinterpret_cast<size_t>(userData);
    std::shared_ptr<OHOSAudioOutputCallback> callback = callback_wrapper_.GetCallback(callback_index);
    if (!callback) {
      LOG(ERROR) << "AudioRendererOutputDeviceChangeCallback callback is nullptr";
      return;
    }
    callback->AudioRendererOutputDeviceChangeCallback(reason);
  }
}

// LCOV_EXCL_START
void OHOSAudioOutputStream::OnSuspend() {
  if (isDestroyed_.load()) {
    LOG(INFO) << "OHOSAudioOutputStream::OnSuspend during destroyed";
    return;
  }
  LOG(INFO) << "OHOSAudioOutputStream::OnSuspend. [hash: "
            << std::hex << base::FastHash(base::byte_span_from_ref(this)) << "]";
  if (!parameters_.IsValid()) {
    LOG(ERROR) << "OHOSAudioOutputStream::OnSuspend parameters_ is not valid.";
    return;
  }
  AudioParameters parameters = parameters_;
  if (!running_) {
    LOG(ERROR) << "The playback is stopped. Exit OnSuspend.";
    return;
  }
  if (OHOSAudioFocusController::IsActive(parameters) || !isSuspended_) {
    if (audioResumeInterval_ != 0) {
      intervalSinceLastSuspend_ = std::time(nullptr);
    }
    if (!main_task_runner_) {
      LOG(INFO) << "main task runner is nullptr";
      return;
    }
    main_task_runner_->PostTask(
        FROM_HERE, base::BindOnce(&OHOSAudioFocusController::OnSuspend, parameters));
    isSuspended_ = true;
    // After stopping playback, it is necessary to continue obtaining audio
    // data, which will trigger the pause action of the render process.
    if (audio_task_runner_) {
      audio_task_runner_->PostTask(
        FROM_HERE, base::BindOnce(&OHOSAudioOutputStream::PumpSamples,
                                  weak_factory_.GetWeakPtr()));
    }
  } else {
    LOG(INFO) << "media session is not active. [hash: "
              << std::hex << base::FastHash(base::byte_span_from_ref(this)) << "]";
#if BUILDFLAG(ARKWEB_PERFORMANCE_PERSISTENT_TASK)
    if (OHOSAudioFocusController::HasOnlyOneShotPlayersPublic(parameters)) {
      OneShotMediaPlayerStopped(parameters);
    }
#endif
  }
}

void OHOSAudioOutputStream::OneShotMediaPlayerStopped(AudioParameters parameters) {
  LOG(INFO) << __func__ << "[hash: "
            << std::hex << base::FastHash(base::byte_span_from_ref(this)) << "]";
  if (!content::BrowserThread::CurrentlyOn(content::BrowserThread::UI)) {
    if (!main_task_runner_) {
      LOG(ERROR) << "main_task_runner is nullptr";
      return;
    }
    main_task_runner_->PostTask(
        FROM_HERE, base::BindOnce(&OHOSAudioFocusController::OneShotMediaPlayerStopped, parameters));
  } else {
    OHOSAudioFocusController::OneShotMediaPlayerStopped(parameters);
  }
}

void OHOSAudioOutputStream::OnResume() {
  if (isDestroyed_.load()) {
    LOG(INFO) << "OHOSAudioOutputStream::OnResume during destroyed";
    return;
  }
  LOG(INFO) << "OHOSAudioOutputStream::OnResume audioResumeInterval is: "
            << std::time(nullptr) - intervalSinceLastSuspend_;
  if (!parameters_.IsValid()) {
    LOG(ERROR) << "OHOSAudioOutputStream::OnResume parameters_ is not valid.";
    return;
  }
  AudioParameters parameters = parameters_;
  if (OHOSAudioFocusController::IsSuspended(parameters_)) {
    if(isNeedResume(audioResumeInterval_)) {
      if (!main_task_runner_) {
        LOG(ERROR) << "OHOSAudioOutputStream::OnResume main task runner is nullptr";
        return;
      }
      main_task_runner_->PostTask(
        FROM_HERE, base::BindOnce(&OHOSAudioFocusController::OnResume, parameters));
    }
    return;
  }
  if (callback_ && audio_task_runner_) {
    LOG(INFO) << "[Oneshot] try to restart stream";
    audio_task_runner_->PostTask(
      FROM_HERE, base::BindOnce(&OHOSAudioOutputStream::Start,
                                weak_factory_.GetWeakPtr(), callback_));
  }
}
// LCOV_EXCL_STOP

void OHOSAudioOutputStream::OldDeviceUnavailable() {
  if (isCommunication_) {
    LOG(INFO) << "OldDeviceUnavailable communication";
    return;
  }
 
  LOG(INFO) << "AudioRendererOutputDeviceChangeCallback need stop session";
  auto OutputDeviceChangeFunc =
    [] (AudioParameters params) {
    content::RenderFrameHost* renderFrameHost =
    content::RenderFrameHost::FromID(params.render_process_id(),
                                      params.render_frame_id());
    if (!renderFrameHost) {
      LOG(ERROR) << "OldDeviceUnavailable get renderFrameHost failed.";
      return;
    }
    auto webContent =
        content::WebContents::FromRenderFrameHost(renderFrameHost);
    if (!webContent) {
      LOG(ERROR) << "AudioOutputStream get webContent failed.";
      return;
    }
    content::MediaSessionImpl* mediaSession =
      content::MediaSessionImpl::Get(webContent);
    if (!mediaSession) {
      LOG(ERROR) << "AudioOutputStream get mediaSession failed.";
      return;
    }
    auto weakMediaSession = mediaSession->weakMediaSessionFactory_.GetWeakPtr();
    if (!weakMediaSession) {
      LOG(ERROR) << "OHOSAudioOutputStream::OHOSAudioOutputStream "
                    "weakMediaSession get failed";
      return;
    }
 
    if (weakMediaSession.get()->IsActive()) {
      LOG(INFO) << "MediaSession is suspending the audio";
      weakMediaSession.get()->Suspend(
          content::MediaSession::SuspendType::kSystem);
    } else {
      LOG(INFO) << "MediaSession is suspended";
    }
  };
 
  if (!content::BrowserThread::CurrentlyOn(content::BrowserThread::UI)) {
    if (!main_task_runner_) {
      LOG(INFO) << "main_task_runner is nullptr";
      return;
    }
    main_task_runner_->PostTask(
      FROM_HERE,
      base::BindOnce(OutputDeviceChangeFunc, parameters_));
  } else {
    OutputDeviceChangeFunc(parameters_);
  }
}

bool OHOSAudioOutputStream::isNeedResume(int32_t resumeInterval) {
  return resumeInterval < 0 ||
      (resumeInterval > 0 &&
      std::time(nullptr) - intervalSinceLastSuspend_ <=
      static_cast<double>(resumeInterval));
}

// LCOV_EXCL_START
void OHOSAudioOutputStream::SuspendOtherMediaSession() {
  auto it = OHOSAudioOutputStream::audioParameterSet_.begin();
  while (it != OHOSAudioOutputStream::audioParameterSet_.end()) {
    if (!(*it).IsValid() || ((*it).render_process_id() <= 0 || (*it).render_frame_id() <= 0)) {
      LOG(ERROR) << "OHOSAudioOutputStream::Start parameter is not valid.";
      it = OHOSAudioOutputStream::audioParameterSet_.erase(it);
      continue;
    }
    if ((*it).render_process_id() == parameters_.render_process_id() &&
        (*it).render_frame_id() == parameters_.render_frame_id()) {
      LOG(INFO) << "skip mediaSession control because of same audioparameters.";
      it++;
      continue;
    }
    if (GetInterruptMode() && !IsPreloadOrMutedMediaMode()) {
      LOG(INFO) << "MediaSession is suspending the audio in other web.";
      main_task_runner_->PostTask(
          FROM_HERE,
          base::BindOnce(OHOSAudioFocusController::OnSuspend, (*it)));
    }
    it++;
  }

  OHOSAudioOutputStream::audioParameterSet_.emplace_back(parameters_);
}
// LCOV_EXCL_STOP

void OHOSAudioOutputStream::Start(AudioSourceCallback* callback) {
  LOG(INFO) << "OHOSAudioOutputStream::Start [hash: "
            << std::hex << base::FastHash(base::byte_span_from_ref(this)) << "]";
  base::AutoLock lock(lock_);
  DCHECK(reference_time_.is_null());
  isSuspended_ = false;

  // set interrupt mode
  Prepare(parameters_);

  callback_ = callback;
  if (!StartRender()) {
    callback_ = nullptr;
    LOG(ERROR) << "OHOSAudioOutputStream::StartRender failed";
    return;
  }
  SuspendOtherMediaSession();
  running_ = true;
}

// LCOV_EXCL_START
void OHOSAudioOutputStream::Stop() {
  LOG(INFO) << "OHOSAudioOutputStream::Stop. [hash: "
            << std::hex << base::FastHash(base::byte_span_from_ref(this)) << "]";
  base::AutoLock lock(lock_);
  StopTimer();
  running_ = false;
  auto it = std::find(OHOSAudioOutputStream::audioParameterSet_.begin(),
                      OHOSAudioOutputStream::audioParameterSet_.end(), parameters_);
  if (it != OHOSAudioOutputStream::audioParameterSet_.end()) {
    OHOSAudioOutputStream::audioParameterSet_.erase(it);
  }
  if (!audio_renderer_) {
    LOG(ERROR) << "OHOSAudioOutputStream::Stop. audio_renderer_ is nullptr";
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

void OHOSAudioOutputStream::StopTimer() {
  if (timer_.IsRunning()) {
    timer_.Stop();
  }
}
// LCOV_EXCL_STOP

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

// LCOV_EXCL_START
bool OHOSAudioOutputStream::InitRender() {
  if (!parameters_.IsValid() || !audio_stream_builder_) {
    LOG(ERROR) << "OHOSAudioOutputStream::InitRender, parameters_ is not valid "
                  "or audio_stream_builder_ is nullptr.";
    return false;
  }
  // set params
  OH_AudioStreamBuilder_SetSamplingRate(audio_stream_builder_,
                                        parameters_.sample_rate());
  OH_AudioStreamBuilder_SetChannelCount(audio_stream_builder_,
                                        parameters_.channels());
  if (parameters_.latency_tag() == AudioLatency::Type::kPlayback) {
    OH_AudioStreamBuilder_SetLatencyMode(audio_stream_builder_,
                                      AUDIOSTREAM_LATENCY_MODE_NORMAL);
  } else {
    LOG(INFO) << "OHOSAudioOutputStream InitRender low latency stream";
    OH_AudioStreamBuilder_SetLatencyMode(audio_stream_builder_,
                                        AUDIOSTREAM_LATENCY_MODE_FAST);
  }
  OH_AudioStreamBuilder_SetFrameSizeInCallback(audio_stream_builder_,
                                               parameters_.frames_per_buffer());
  OH_AudioStreamBuilder_SetEncodingType(audio_stream_builder_,
                                        AUDIOSTREAM_ENCODING_TYPE_RAW);
  OH_AudioStreamBuilder_SetSampleFormat(audio_stream_builder_,
                                        AUDIOSTREAM_SAMPLE_S16LE);
  OH_AudioStreamBuilder_SetRendererInterruptMode(
      audio_stream_builder_, (OH_AudioInterrupt_Mode) false);
  SetStreamUsage();

  // set callback
  OH_AudioRenderer_Callbacks callbacks;
  callbacks.OH_AudioRenderer_OnWriteData = AudioRendererOnWriteData;
  callbacks.OH_AudioRenderer_OnError = AudioRendererOnError;
  callbacks.OH_AudioRenderer_OnInterruptEvent = AudioRendererOnInterruptEvent;

  std::shared_ptr<OHOSAudioOutputCallback> audioOutputCallback =
      std::make_shared<OHOSAudioOutputCallback>(weak_factory_.GetWeakPtr());
  if (!audioOutputCallback) {
    LOG(ERROR) << "InitRender audioOutputCallback is nullptr";
    return false;
  }

  if (callback_index_ > 0) {
    callback_wrapper_.Clear(callback_index_);
    callback_index_ = 0;
  }
  callback_index_ = callback_wrapper_.AddCallback(audioOutputCallback);
  OH_AudioStreamBuilder_SetRendererCallback(audio_stream_builder_, callbacks,
                                            reinterpret_cast<void*>(callback_index_));
  OH_AudioStream_Result res = OH_AudioStreamBuilder_SetRendererOutputDeviceChangeCallback(
                              audio_stream_builder_,
                              &AudioRendererOutputDeviceChangeCallback, reinterpret_cast<void*>(callback_index_));
  if (res != AUDIOSTREAM_SUCCESS) {
    return false;
  }
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

void OHOSAudioOutputStream::SetStreamUsage() {
  if (isCommunication_) {
    OH_AudioStreamBuilder_SetRendererInfo(audio_stream_builder_, AUDIOSTREAM_USAGE_VIDEO_COMMUNICATION);
    return;
  }
 
  OH_AudioStreamBuilder_SetRendererInfo(audio_stream_builder_, AUDIOSTREAM_USAGE_UNKNOWN);
  if (parameters_.IsValid()) {
    auto audioSessionTypeFocus = OHOSAudioFocusController::GetAudioSessionType(parameters_);
    LOG(INFO) << "OHOSAudioOutputStream AudioSessionType: " << audioSessionTypeFocus;
    AudioSessionType audioSessionType = static_cast<AudioSessionType>(audioSessionTypeFocus);
    if (audioSessionType == AudioSessionType::AMBIENT) {
      OH_AudioStreamBuilder_SetRendererInfo(audio_stream_builder_, AUDIOSTREAM_USAGE_GAME);
    } else if (audioSessionType == AudioSessionType::AUTO) {
      media::MediaContentType contentType = OHOSAudioFocusController::GetMediaContentType(parameters_);
      if (contentType == media::MediaContentType::kTransient) {
        OH_AudioStreamBuilder_SetRendererInfo(audio_stream_builder_, AUDIOSTREAM_USAGE_NAVIGATION);
      }
    }
  }
}
// LCOV_EXCL_STOP

void OHOSAudioOutputStream::Prepare(const AudioParameters& parameters) {
  LOG(INFO) << "OHOSAudioOutputStream::Prepare";
  if (!parameters_.IsValid()) {
    LOG(ERROR) << "OHOSAudioOutputStream::Prepare parameters_ is not valid.";
    return;
  }
  audioExclusive_ = OHOSAudioFocusController::GetAudioExclusive(parameters);
  LOG(INFO) << "OHOSAudioOutputStream::SetRendererInterruptMode audioExclusive: " << audioExclusive_;
}

// LCOV_EXCL_START
bool OHOSAudioOutputStream::StartRender() {
  if (IsPreloadOrMutedMediaMode()) {
    OH_AudioRenderer_SetSilentModeAndMixWithOthers(audio_renderer_, true);
    LOG(INFO) << "OHOSAudioOutputStream SetAudioSilentMode true";
    isSilentMode_ = true;
    write_data_counts_ = 0;
  }
  OH_AudioStream_Result ret = OH_AudioRenderer_Start(audio_renderer_);
  if (ret != AUDIOSTREAM_SUCCESS) {
    if (OH_AudioRenderer_Release(audio_renderer_) != AUDIOSTREAM_SUCCESS) {
      LOG(ERROR) << "ohos audio render release failed";
    } else {
      if (callback_index_ > 0) {
        callback_wrapper_.Clear(callback_index_);
        callback_index_ = 0;
      }
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
// LCOV_EXCL_STOP

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

// LCOV_EXCL_START
void OHOSAudioOutputStream::PumpSamples() {
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
    if (!audio_renderer_) {
      LOG(INFO) << "AudioRender is nullptr";
      return;
    }
    {
      base::AutoLock lock(lock_);
      // Obtains data. The data does not need to be processed and may be empty.
      (void)callback_->OnMoreData(base::TimeDelta(), base::TimeTicks::Now(), {}, audio_bus_.get());
    }
    SchedulePumpSamples();
}
// LCOV_EXCL_STOP

void OHOSAudioOutputStream::OnWriteData(void* buffer, int32_t length) {
  base::AutoLock lock(lock_);
  if (!running_) {
      return;
  }
  if (!buffer) {
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
  frames_filled = std::min(frames_filled, length / bytes_per_frame_);
  audio_bus_->ToInterleaved<SignedInt16SampleTypeTraits>(
      frames_filled, reinterpret_cast<int16_t*>(buffer));
  DumpFileUtil::WriteDumpScopedFile(dumpFile_, buffer, length);
  if (reference_time_.is_null()) {
    reference_time_ = now;
  }
}

// LCOV_EXCL_START
void OHOSAudioOutputStream::SetUpAudioSilentState() {
  if (!isSilentMode_) {
    return;
  }

  if (!parameters_.IsValid() || !audio_renderer_) {
    LOG(ERROR) << "OHOSAudioOutputStream: Try to set audio silent but get "
                  "parameters or audioRender failed!";
    return;
  }

  if (parameters_.latency_tag() == AudioLatency::Type::kInteractive) {
    if (OHOSAudioFocusController::IsWebContentCurrentlyAudible(parameters_)) {
      OH_AudioRenderer_SetSilentModeAndMixWithOthers(audio_renderer_, false);
      LOG(INFO) << "OHOSAudioOutputStream AudioContext SetAudioSilentMode false!";
      isSilentMode_ = false;
    } else {
      LOG(DEBUG) << "OHOSAudioOutputStream IsWebContentCurrentlyAudible is false";
    }
    return;
  }

  bool is_playing = OHOSAudioFocusController::IsActive(parameters_) ||
                    OHOSAudioFocusController::GetPlayingState(parameters_) ||
                    write_data_counts_ >= 1;
  bool is_muted = OHOSAudioFocusController::GetMediaPlayerMuteState(parameters_) ||
                  !OHOSAudioFocusController::IsWebContentCurrentlyAudible(parameters_);
  if (is_playing && !is_muted) {
    OH_AudioRenderer_SetSilentModeAndMixWithOthers(audio_renderer_, false);
    LOG(INFO) << "OHOSAudioOutputStream SetAudioSilentMode false!";
    isSilentMode_ = false;
    write_data_counts_ = 0;
  } else {
    ++write_data_counts_;
  }
}

bool OHOSAudioOutputStream::IsPreloadOrMutedMediaMode() {
  if (base::ohos::IsPcDevice()) {
    LOG(INFO)
        << "OHOSAudioOutputStream::IsPreloadOrMutedMediaMode device is pc";
    return false;
  }

  if (!parameters_.IsValid()) {
    LOG(ERROR) << "OHOSAudioOutputStream::IsPreloadOrMutedMediaMode,"
                  "parameters_ is not valid";
    return false;
  }

  if (parameters_.latency_tag() == AudioLatency::Type::kInteractive) {
    LOG(INFO) << "OHOSAudioOutputStream::IsPreloadOrMutedMediaMode AudioContext";
    if (audio_renderer_) {
      LOG(INFO) << "OHOSAudioOutputStream AudioContext SetAudioSilentMode true";
      OH_AudioRenderer_SetSilentModeAndMixWithOthers(audio_renderer_, true);
      isSilentMode_ = true;
    }
    return false;
  }
  
  content::MediaSessionImpl::NWebMediaSessionState sessionState =
      OHOSAudioFocusController::GetSessionState(parameters_);
  bool is_active = OHOSAudioFocusController::IsActive(parameters_);
  bool is_playingState = OHOSAudioFocusController::GetPlayingState(parameters_);
  bool is_muted = OHOSAudioFocusController::GetMediaPlayerMuteState(parameters_) ||
                  !OHOSAudioFocusController::IsWebContentCurrentlyAudible(parameters_);
  LOG(INFO) << "OHOSAudioOutputStream sessionState:"
            << static_cast<uint32_t>(sessionState) << ", mutedMode:" << is_muted
            << ", activeMode:" << is_active
            << ", playingState: " << is_playingState;

  bool is_preload = is_active || is_playingState;
  if (sessionState == content::MediaSessionImpl::NWebMediaSessionState::NOINITIAL ||
      OHOSAudioFocusController::HasOneShotPlayersWhenSetMetadataPublic(parameters_)) {
    return false;
  }

  return !is_preload || is_muted;
}

void OHOSAudioOutputStream::SchedulePumpSamples() {
  if (timer_.IsRunning()) {
    LOG(INFO) << __func__ << ", timer is running";
    return;
  }
  if (!running_) {
    LOG(INFO) << __func__ << ", audio output not running";
    return;
  }
  timer_.Start(FROM_HERE, time_per_buffer_,
               base::BindOnce(&OHOSAudioOutputStream::PumpSamples,
                              weak_factory_.GetWeakPtr()));
}
// LCOV_EXCL_STOP

}  // namespace media
