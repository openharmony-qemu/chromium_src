// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "media/audio/ohos/ohos_audio_output_stream.h"

#include "base/logging.h"
#include "base/task/single_thread_task_runner.h"
#include "content/public/browser/browser_task_traits.h"
#include "content/public/browser/browser_thread.h"
#include "media/base/audio_bus.h"
#include "media/base/audio_sample_types.h"
#include "media/base/audio_timestamp_helper.h"
#include "ohos/adapter/device_info/device_info.h"

extern "C" {
// since API 24
__attribute__((weak_import)) OH_AudioStream_Result OH_AudioRenderer_SetIndependentAudioSessionStrategy(
    OH_AudioRenderer* renderer,
    const OH_AudioSession_Strategy* strategy,
    uint32_t behavior);
}

namespace media {

// Used to circumvent issues where the AudioWriteCB thread callbacks continue
// after OH_AudioRenderer_Stop().
class LOCKABLE AudioRenderDestructionHelper {
 public:
  explicit AudioRenderDestructionHelper(OhosAudioOutputStream* stream)
      : output_stream_(stream) {}

  ~AudioRenderDestructionHelper() {
    DCHECK(is_closing_);
    if (audio_renderer_) {
      OH_AudioStream_Result ret = OH_AudioRenderer_Release(audio_renderer_);
      if (ret != AUDIOSTREAM_SUCCESS) {
        LOG(ERROR) << "AudioRenderer Release failed, result:" << ret;
      }
    }
  }

  OhosAudioOutputStream* GetAndLockStream() EXCLUSIVE_LOCK_FUNCTION() {
    lock_.Acquire();
    return is_closing_ ? nullptr : output_stream_.get();
  }

  void UnlockStream() UNLOCK_FUNCTION() { lock_.Release(); }

  void DeferStreamClosure(OH_AudioRenderer* stream) {
    base::AutoLock al(lock_);
    DCHECK(!is_closing_);

    is_closing_ = true;
    audio_renderer_ = stream;
  }

 private:
  base::Lock lock_;
  raw_ptr<OhosAudioOutputStream> output_stream_ GUARDED_BY(lock_) = nullptr;
  raw_ptr<OH_AudioRenderer> audio_renderer_ GUARDED_BY(lock_) = nullptr;
  bool is_closing_ GUARDED_BY(lock_) = false;
};

OhosAudioOutputStream::OhosAudioOutputStream(OHOSAudioManager* manager,
                                             const AudioParameters& parameters)
    : manager_(manager),
      parameters_(parameters),
      ns_per_frame_(base::Time::kNanosecondsPerSecond /
                    static_cast<double>(parameters.sample_rate())),
      audio_bus_(AudioBus::Create(parameters)),
      destruction_helper_(
          std::make_unique<AudioRenderDestructionHelper>(this)) {
  weak_media_session_ = content::MediaSession::Get(
      parameters_.render_process_id(), parameters_.render_frame_id());
  if (!weak_media_session_) {
    LOG(ERROR) << "OhosAudioOutputStream weak_media_session_ get failed";
  } else {
    is_session_controllable_ =
        !weak_media_session_->HasOnlyOneShotPlayersPublic();
  }

  OH_AudioStream_Result ret = OH_AudioStreamBuilder_Create(
      &audio_stream_builder_, AUDIOSTREAM_TYPE_RENDERER);
  if (ret != AUDIOSTREAM_SUCCESS) {
    LOG(ERROR) << "AudioStreamBuilder create failed.";
  }

  sample_format_ = kSampleFormatS16;
  bytes_per_frame_ = parameters.GetBytesPerFrame(sample_format_);
  buffer_size_bytes_ = parameters.GetBytesPerBuffer(sample_format_);
  time_per_buffer_ = AudioTimestampHelper::FramesToTime(
      parameters_.frames_per_buffer(), parameters_.sample_rate());
  main_task_runner_ = content::GetUIThreadTaskRunner({});
  audio_task_runner_ = base::SingleThreadTaskRunner::GetCurrentDefault();
}

OhosAudioOutputStream::~OhosAudioOutputStream() {
  if (audio_stream_builder_ != nullptr) {
    OH_AudioStreamBuilder_Destroy(audio_stream_builder_);
    audio_stream_builder_ = nullptr;
  }

  base::SequencedTaskRunner::GetCurrentDefault()->PostDelayedTask(
      FROM_HERE, base::DoNothingWithBoundArgs(std::move(destruction_helper_)),
      base::Seconds(1));
}

bool OhosAudioOutputStream::Open() {
  if (!InitRender()) {
    return false;
  }
  return true;
}

void OhosAudioOutputStream::Close() {
  Stop();

  // |destruction_helper_->GetStreamAndLock()| will return nullptr after this.
  destruction_helper_->DeferStreamClosure(audio_renderer_);

  // We shouldn't be acessing |audio_renderer_| after it's stopped.
  audio_renderer_ = nullptr;

  manager_->ReleaseOutputStream(this);
}

static int32_t AudioRendererOnWriteData(OH_AudioRenderer* renderer,
                                        void* user_data,
                                        void* buffer,
                                        int32_t length) {
  AudioRenderDestructionHelper* destruction_helper =
      reinterpret_cast<AudioRenderDestructionHelper*>(user_data);
  OhosAudioOutputStream* output_stream = destruction_helper->GetAndLockStream();
  if (output_stream && buffer) {
    output_stream->PumpSamples(buffer, length);
  }
  destruction_helper->UnlockStream();
  return 0;
}

static int32_t AudioRendererOnError(OH_AudioRenderer* renderer,
                                    void* user_data,
                                    OH_AudioStream_Result error) {
  AudioRenderDestructionHelper* destruction_helper =
      reinterpret_cast<AudioRenderDestructionHelper*>(user_data);
  OhosAudioOutputStream* output_stream = destruction_helper->GetAndLockStream();
  if (output_stream) {
    output_stream->ReportError();
  }
  destruction_helper->UnlockStream();
  return 0;
}

static int32_t AudioRendererOnInterruptEvent(OH_AudioRenderer* renderer,
                                             void* user_data,
                                             OH_AudioInterrupt_ForceType type,
                                             OH_AudioInterrupt_Hint hint) {
  LOG(INFO) << __func__ << " [AudioStream] hint: " << static_cast<int>(hint)
            << " type: " << static_cast<int>(type);
  AudioRenderDestructionHelper* destruction_helper =
      reinterpret_cast<AudioRenderDestructionHelper*>(user_data);
  OhosAudioOutputStream* output_stream = destruction_helper->GetAndLockStream();
  if (output_stream && hint == AUDIOSTREAM_INTERRUPT_HINT_RESUME) {
    output_stream->OnResume();
    destruction_helper->UnlockStream();
    return 0;
  }

  if (output_stream && type == AUDIOSTREAM_INTERRUPT_SHARE) {
    destruction_helper->UnlockStream();
    return 0;
  }

  if (output_stream && (hint == AUDIOSTREAM_INTERRUPT_HINT_PAUSE ||
                        hint == AUDIOSTREAM_INTERRUPT_HINT_STOP)) {
    output_stream->OnSuspend();
  }
  destruction_helper->UnlockStream();
  return 0;
}

static void AudioRendererOnOutputDeviceChange(
    OH_AudioRenderer* renderer,
    void* user_data,
    OH_AudioStream_DeviceChangeReason reason) {
  AudioRenderDestructionHelper* destruction_helper =
      reinterpret_cast<AudioRenderDestructionHelper*>(user_data);
  OhosAudioOutputStream* output_stream = destruction_helper->GetAndLockStream();
  if (output_stream && reason == REASON_OLD_DEVICE_UNAVAILABLE) {
    output_stream->SuspendPlayer();
  }
  destruction_helper->UnlockStream();
}

void OhosAudioOutputStream::Start(AudioSourceCallback* callback) {
  DCHECK(!callback_);
  DCHECK(reference_time_.is_null());
  {
    base::AutoLock al(lock_);
    callback_ = callback;
    if (!StartRender()) {
      LOG(ERROR) << "OhosAudioOutputStream::StartRender failed";
      callback_ = nullptr;
    }
  }
}

void OhosAudioOutputStream::Stop() {
  {
    base::AutoLock al(lock_);
    if (!audio_renderer_ || !callback_) {
      return;
    }
  }

  timer_.Stop();
  if (!reference_time_.is_null()) {
    reference_time_ = base::TimeTicks();
  }
  OH_AudioStream_Result ret = OH_AudioRenderer_Stop(audio_renderer_);
  {
    base::AutoLock al(lock_);
    if (ret != AUDIOSTREAM_SUCCESS) {
      ReportErrorInternal();
    }
    callback_ = nullptr;
  }
  Flush();
}

void OhosAudioOutputStream::Refresh() {
  if (!audio_renderer_) {
    LOG(ERROR) << "OhosAudioOutputStream::Refresh audio_renderer_ is null.";
    return;
  }
  OH_AudioRenderer_Stop(audio_renderer_);
  OH_AudioRenderer_Start(audio_renderer_);
}

void OhosAudioOutputStream::SetInterruptMode(bool audioExclusive) {
  LOG(INFO) << "OhosAudioOutputStream::SetInterruptMode audioExclusive: "
            << audioExclusive;
  if (!audio_renderer_) {
    LOG(ERROR)
        << "OhosAudioOutputStream::SetInterruptMode audio_renderer_ is null.";
    return;
  }
}

// This stream is always used with sub second buffer sizes, where it's
// sufficient to simply always flush upon Start().
void OhosAudioOutputStream::Flush() {
  if (!audio_renderer_) {
    LOG(ERROR) << "OhosAudioOutputStream::Flush audio_renderer_ is null.";
    return;
  }
  OH_AudioStream_Result ret = OH_AudioRenderer_Flush(audio_renderer_);
  if (ret != AUDIOSTREAM_SUCCESS) {
    LOG(ERROR) << "OhosAudioOutputStream::Flush failed, result:" << ret;
  }
}

void OhosAudioOutputStream::SetVolume(double volume) {
  if (volume < 0.0 || volume > 1.0) {
    return;
  }
  volume_ = volume;
}

void OhosAudioOutputStream::GetVolume(double* volume) {
  *volume = volume_;
}

bool OhosAudioOutputStream::InitRender() {
  if (audio_stream_builder_ == nullptr) {
    LOG(ERROR) << __func__
               << " [AudioStream] audio_stream_builder_ is not initialized";
    return false;
  }
  if (!parameters_.IsValid()) {
    LOG(ERROR) << __func__ << " [AudioStream] parameters_ is not initialized";
    return false;
  }

  // set params
  OH_AudioStream_Result audio_stream_result =
      OH_AudioStreamBuilder_SetSamplingRate(audio_stream_builder_,
                                            parameters_.sample_rate());
  if (audio_stream_result != AUDIOSTREAM_SUCCESS) {
    LOG(WARNING) << __func__
                 << " [AudioStream] failed to "
                    "OH_AudioStreamBuilder_SetSamplingRate, result: "
                 << static_cast<int>(audio_stream_result);
  }
  audio_stream_result = OH_AudioStreamBuilder_SetChannelCount(
      audio_stream_builder_, parameters_.channels());
  if (audio_stream_result != AUDIOSTREAM_SUCCESS) {
    LOG(WARNING) << __func__
                 << " [AudioStream] failed to "
                    "OH_AudioStreamBuilder_SetChannelCount, result: "
                 << static_cast<int>(audio_stream_result);
  }
  audio_stream_result = OH_AudioStreamBuilder_SetLatencyMode(
      audio_stream_builder_, AUDIOSTREAM_LATENCY_MODE_NORMAL);
  if (audio_stream_result != AUDIOSTREAM_SUCCESS) {
    LOG(WARNING) << __func__
                 << " [AudioStream] failed to "
                    "OH_AudioStreamBuilder_SetLatencyMode, result: "
                 << static_cast<int>(audio_stream_result);
  }
  audio_stream_result = OH_AudioStreamBuilder_SetFrameSizeInCallback(
      audio_stream_builder_, parameters_.frames_per_buffer());
  if (audio_stream_result != AUDIOSTREAM_SUCCESS) {
    LOG(WARNING) << __func__
                 << " [AudioStream] failed to "
                    "OH_AudioStreamBuilder_SetFrameSizeInCallback, result: "
                 << static_cast<int>(audio_stream_result);
  }

  // set audio stream to private to prevent recording encrypted audio stream
  if (parameters_.GetStreamPrivacy()) {
    LOG(INFO) << __func__
              << " [WiseplayDRM] OH_AudioStreamBuilder_SetRendererPrivacy to "
                 "AUDIO_STREAM_PRIVACY_TYPE_PRIVATE due to "
                 "play encrypted audio. ";
    audio_stream_result = OH_AudioStreamBuilder_SetRendererPrivacy(
        audio_stream_builder_, AUDIO_STREAM_PRIVACY_TYPE_PRIVATE);
    if (audio_stream_result != AUDIOSTREAM_SUCCESS) {
      LOG(ERROR) << __func__
                 << " [WiseplayDRM] failed to "
                    "OH_AudioStreamBuilder_SetRendererPrivacy, result: "
                 << static_cast<int>(audio_stream_result);
      return false;
    }
  }

  // The default usage is set to music, and the playback status can be
  // controlled via mediaSession. If it cannot be controlled by mediaSession,
  // the audio stream's usage should be set to notification to ensure it is not
  // interrupted.
  if (!is_session_controllable_) {
    audio_stream_result = audio_stream_result =
        OH_AudioStreamBuilder_SetRendererInfo(audio_stream_builder_,
                                              AUDIOSTREAM_USAGE_NOTIFICATION);
    if (audio_stream_result != AUDIOSTREAM_SUCCESS) {
      LOG(WARNING) << __func__
                   << " [AudioStream] failed to "
                      "OH_AudioStreamBuilder_SetRendererInfo, result: "
                   << static_cast<int>(audio_stream_result);
    }
  }

  // set callback
  OH_AudioRenderer_Callbacks callbacks = {};
  callbacks.OH_AudioRenderer_OnWriteData = AudioRendererOnWriteData;
  callbacks.OH_AudioRenderer_OnStreamEvent = nullptr;
  callbacks.OH_AudioRenderer_OnError = AudioRendererOnError;
  callbacks.OH_AudioRenderer_OnInterruptEvent = AudioRendererOnInterruptEvent;
  OH_AudioStream_Result ret = OH_AudioStreamBuilder_SetRendererCallback(
      audio_stream_builder_, callbacks, destruction_helper_.get());
  if (audio_stream_result != AUDIOSTREAM_SUCCESS) {
    LOG(WARNING) << __func__
                 << " [AudioStream] failed to "
                    "OH_AudioStreamBuilder_SetRendererCallback, result: "
                 << static_cast<int>(audio_stream_result);
  }
  ret = OH_AudioStreamBuilder_SetRendererOutputDeviceChangeCallback(
      audio_stream_builder_, AudioRendererOnOutputDeviceChange,
      destruction_helper_.get());
  if (audio_stream_result != AUDIOSTREAM_SUCCESS) {
    LOG(WARNING) << __func__
                 << " [AudioStream] failed to "
                    "OH_AudioStreamBuilder_"
                    "SetRendererOutputDeviceChangeCallback, result: "
                 << static_cast<int>(audio_stream_result);
  }

  // create audio render
  ret = OH_AudioStreamBuilder_GenerateRenderer(audio_stream_builder_,
                                               &audio_renderer_);
  if (ret != AUDIOSTREAM_SUCCESS) {
    LOG(ERROR) << "AudioStreamBuilder GenerateRenderer failed.";
    return false;
  }
  if (ohos::adapter::device_info::DeviceInfo::GetInstance().SdkApi() >=
      ohos::adapter::device_info::SdkVersion::SDK_VERSION_24) {
    if (!OH_AudioRenderer_SetIndependentAudioSessionStrategy) {
      LOG(WARNING) << __func__
                   << " [AudioStream] "
                      "OH_AudioRenderer_SetIndependentAudioSessionStrategy API "
                      "not exist.";
    } else {
      OH_AudioSession_Strategy strategy = {};
      strategy.concurrencyMode =
          OH_AudioSession_ConcurrencyMode::CONCURRENCY_MIX_WITH_OTHERS;
      audio_stream_result = OH_AudioRenderer_SetIndependentAudioSessionStrategy(
          audio_renderer_, &strategy,
          OH_AudioSession_BehaviorFlags::MUTE_WHEN_INTERRUPTED);
      if (audio_stream_result != AUDIOSTREAM_SUCCESS) {
        LOG(WARNING)
            << __func__
            << " [AudioStream] failed to "
               "OH_AudioRenderer_SetIndependentAudioSessionStrategy, result: "
            << static_cast<int>(audio_stream_result);
      }
    }
  }

  return true;
}

bool OhosAudioOutputStream::StartRender() {
  OH_AudioStream_Result ret = OH_AudioRenderer_Start(audio_renderer_);
  if (ret != AUDIOSTREAM_SUCCESS) {
    LOG(ERROR) << "AudioRenderer Start failed, result:" << ret;
    ReportErrorInternal();
    return false;
  }
  return true;
}

void OhosAudioOutputStream::ReleaseRender() {
  if (!audio_renderer_) {
    return;
  }
  OH_AudioStream_Result ret = OH_AudioRenderer_Release(audio_renderer_);
  audio_renderer_ = nullptr;
  LOG(INFO) << "AudioRenderer Release, result:" << ret;
}

void OhosAudioOutputStream::ReportError() {
  base::AutoLock al(lock_);
  ReportErrorInternal();
}

void OhosAudioOutputStream::ReportErrorInternal() {
  LOG(ERROR) << "ohos audio render error happened";
  reference_time_ = base::TimeTicks();
  timer_.Stop();
  if (callback_) {
    callback_->OnError(AudioSourceCallback::ErrorType::kUnknown);
  }
}

base::TimeDelta OhosAudioOutputStream::GetDelay(
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

base::TimeDelta OhosAudioOutputStream::GetDelayImprove(base::TimeTicks now) {
  auto interval = now - update_delay_;
  if (interval > interval_) {
    delay_ = GetDelay(now);
    update_delay_ = base::TimeTicks::Now();
    // Sometimes, GetDelay takes 6 milliseconds.
    // This time cannot be ignored.
    delay_ = now + delay_ - update_delay_;
  }
  return delay_;
}

void OhosAudioOutputStream::PumpSamples(void* buffer, int32_t length) {
  base::AutoLock al(lock_);
  if (!audio_renderer_) {
    LOG(ERROR) << "PumpSamples failed, audio_renderer_ is nullptr";
    return;
  }
  base::TimeTicks now = base::TimeTicks::Now();
  base::TimeDelta delay = GetDelayImprove(now);
  if (!callback_) {
    LOG(WARNING) << "PumpSamples failed, callback_ is nullptr";
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

void OhosAudioOutputStream::IdlePumpSamples() {
  base::AutoLock al(lock_);

  if (!is_suspended_) {
    LOG(ERROR) << "Not a focus preemption scenario. Exit PumpSamples.";
    return;
  }

  if (!callback_) {
    LOG(WARNING) << "PumpSamples failed, callback_ is nullptr";
    return;
  }

  if (!weak_media_session_) {
    LOG(ERROR) << "weak_media_session_ is nullptr. Exit PumpSamples.";
    return;
  }

  if (weak_media_session_->IsActiveSession()) {
    // Frame loss may occur because the timing of the startup cannot be
    // implemented in real time. However, there is no extra packet, and the
    // impact is almost imperceptible.
    if (StartRender()) {
      is_suspended_ = false;
      return;
    } else {
      LOG(ERROR) << "Restarted audioStream but failed";
    }
  }

  (void)callback_->OnMoreData(base::TimeDelta(), base::TimeTicks::Now(), {},
                              audio_bus_.get());
  ScheduleIdlePumpSamples();
}

void OhosAudioOutputStream::ScheduleIdlePumpSamples() {
  timer_.Start(FROM_HERE, time_per_buffer_,
               base::BindOnce(&OhosAudioOutputStream::IdlePumpSamples,
                              weak_factory_.GetWeakPtr()));
}

void OhosAudioOutputStream::OnSuspend() {
  if (!is_session_controllable_) {
    LOG(WARNING) << "Stream is not controlled by the mediaSession. ";
    return;
  }

  SuspendPlayer();
  is_suspended_ = true;
  // After stopping playback, it is necessary to continue obtaining audio data,
  // which will trigger the pause action of the render process.
  if (audio_task_runner_) {
    audio_task_runner_->PostTask(
        FROM_HERE, base::BindOnce(&OhosAudioOutputStream::IdlePumpSamples,
                                  weak_factory_.GetWeakPtr()));
  }
}

void OhosAudioOutputStream::SuspendPlayer() {
  if (!audio_renderer_) {
    LOG(ERROR)
        << "OhosAudioOutputStream::SuspendPlayer audio_renderer_ is null.";
    return;
  }

  main_task_runner_->PostTask(
      FROM_HERE,
      base::BindOnce(&content::MediaSession::Suspend, weak_media_session_,
                     content::MediaSession::SuspendType::kSystem));
}

void OhosAudioOutputStream::OnResume() {
  if (!is_suspended_) {
    return;
  }
  if (!audio_renderer_) {
    LOG(ERROR)
        << "OhosAudioOutputStream::SuspendPlayer audio_renderer_ is null.";
    return;
  }

  main_task_runner_->PostTask(
      FROM_HERE,
      base::BindOnce(&content::MediaSession::Resume, weak_media_session_,
                     content::MediaSession::SuspendType::kSystem));
}

void OhosAudioOutputStream::FlushData() {
  base::AutoLock al(lock_);
  if (!callback_) {
    LOG(ERROR) << "FlushData failed, callback_ is nullptr";
    return;
  }

  // After flushing the audio data, reset the player delay to ensure the
  // playback progress bar functions correctly.
  callback_->OnMoreData(base::TimeDelta(), base::TimeTicks::Now(), {},
                        audio_bus_.get());
}

OH_AudioStream_State OhosAudioOutputStream::GetRenderState() {
  OH_AudioStream_State state = AUDIOSTREAM_STATE_INVALID;
  if (!audio_renderer_) {
    return state;
  }
  OH_AudioStream_Result ret =
      OH_AudioRenderer_GetCurrentState(audio_renderer_, &state);
  if (ret != AUDIOSTREAM_SUCCESS) {
    LOG(WARNING)
        << __func__
        << " [AudioStream] failed to OH_AudioRenderer_GetCurrentState, result:"
        << ret;
  }
  return state;
}

}  // namespace media
