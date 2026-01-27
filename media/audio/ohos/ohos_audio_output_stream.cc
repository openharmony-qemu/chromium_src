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

#include "media/audio/ohos/ohos_audio_output_stream.h"

#include "base/logging.h"
#include "content/public/browser/browser_task_traits.h"
#include "content/public/browser/browser_thread.h"
#include "media/base/audio_timestamp_helper.h"

namespace media {

namespace {
int32_t AudioRendererOnWriteData(OH_AudioRenderer* renderer,
                                 void* userData,
                                 void* buffer,
                                 int32_t length) {
  if (userData && buffer) {
    ((OHOSAudioOutputStream*)(userData))->PumpSamples(buffer, length);
  }
  return 0;
}

int32_t AudioRendererOnError(OH_AudioRenderer* renderer,
                             void* userData,
                             OH_AudioStream_Result error) {
  if (userData) {
    ((OHOSAudioOutputStream*)(userData))->ReportError();
  }
  return 0;
}

int32_t AudioRendererOnInterruptEvent(OH_AudioRenderer* renderer,
                                      void* userData,
                                      OH_AudioInterrupt_ForceType type,
                                      OH_AudioInterrupt_Hint hint) {
  if (!userData) {
    LOG(ERROR) << "AudioRendererOnInterruptEvent failed, userData is nullptr";
    return 0;
  }

  if (hint == AUDIOSTREAM_INTERRUPT_HINT_RESUME) {
    ((OHOSAudioOutputStream*)(userData))->OnResume();
    return 0;
  }

  if (type == AUDIOSTREAM_INTERRUPT_SHARE) {
    return 0;
  }

  if (hint == AUDIOSTREAM_INTERRUPT_HINT_PAUSE ||
      hint == AUDIOSTREAM_INTERRUPT_HINT_STOP) {
    ((OHOSAudioOutputStream*)(userData))->OnSuspend();
  }
  return 0;
}

void AudioRendererOnOutputDeviceChange(
    OH_AudioRenderer* renderer,
    void* userData,
    OH_AudioStream_DeviceChangeReason reason) {
  if (reason == REASON_OLD_DEVICE_UNAVAILABLE && userData) {
    ((OHOSAudioOutputStream*)(userData))->SuspendPlayer();
  }
}
}  // namespace

OHOSAudioOutputStream::OHOSAudioOutputStream(OHOSAudioManager* manager,
                                             const AudioParameters& parameters)
    : manager_(manager),
      parameters_(parameters),
      ns_per_frame_(base::Time::kNanosecondsPerSecond /
                    static_cast<double>(parameters.sample_rate())),
      audio_bus_(AudioBus::Create(parameters)) {
  weak_media_session_ = content::MediaSession::Get(
      parameters_.render_process_id(), parameters_.render_frame_id());
  if (!weak_media_session_) {
    LOG(ERROR) << "OHOSAudioOutputStream weak_media_session_ get failed";
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
}

OHOSAudioOutputStream::~OHOSAudioOutputStream() {
  // Close() must be called first.
  ReleaseRender();
  if (audio_stream_builder_ != nullptr) {
    OH_AudioStreamBuilder_Destroy(audio_stream_builder_);
    audio_stream_builder_ = nullptr;
  }
}

bool OHOSAudioOutputStream::Open() {
  base::AutoLock lock(stream_lock_);
  if (!InitRender()) {
    return false;
  }
  return true;
}

void OHOSAudioOutputStream::Close() {
  Stop();
  manager_->ReleaseOutputStream(this);
}

void OHOSAudioOutputStream::Start(AudioSourceCallback* callback) {
  base::AutoLock lock(stream_lock_);
  DCHECK(!callback_);
  DCHECK(reference_time_.is_null());
  callback_ = callback;

  if (GetRenderState() == AUDIOSTREAM_STATE_STOPPED) {
    FlushData();
  }

  if (!StartRender()) {
    LOG(ERROR) << "OHOSAudioOutputStream::StartRender failed";
  }
  FlushData();
}

void OHOSAudioOutputStream::Stop() {
  base::AutoLock lock(stream_lock_);
  if (!audio_renderer_) {
    return;
  }
  callback_ = nullptr;
  timer_.Stop();
  if (!reference_time_.is_null()) {
    reference_time_ = base::TimeTicks();
  }
  OH_AudioStream_Result ret = OH_AudioRenderer_Stop(audio_renderer_);
  if (ret != AUDIOSTREAM_SUCCESS) {
    ReportError();
  }
  Flush();
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
void OHOSAudioOutputStream::Flush() {
  if (!audio_renderer_) {
    LOG(ERROR) << "OHOSAudioOutputStream::Flush audio_renderer_ is null.";
    return;
  }
  OH_AudioStream_Result ret = OH_AudioRenderer_Flush(audio_renderer_);
  if (ret != AUDIOSTREAM_SUCCESS) {
    LOG(ERROR) << "OHOSAudioOutputStream::Flush failed, result:" << ret;
  }
}

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
  if (audio_stream_builder_ == nullptr) {
    LOG(ERROR) << __func__ << " [AudioStream] audio_stream_builder_ is not initialized";
    return false;
  }
  if (!parameters_.IsValid()) {
    LOG(ERROR) << __func__ << " [AudioStream] parameters_ is not initialized";
    return false;
  }

  // set params
  OH_AudioStream_Result audio_stream_result =
      OH_AudioStreamBuilder_SetSamplingRate(audio_stream_builder_, parameters_.sample_rate());
  if (audio_stream_result != AUDIOSTREAM_SUCCESS) {
    LOG(WARNING) << __func__ << " [AudioStream] failed to OH_AudioStreamBuilder_SetSamplingRate, result: "
                 << static_cast<int>(audio_stream_result);
  }
  audio_stream_result = OH_AudioStreamBuilder_SetChannelCount(audio_stream_builder_, parameters_.channels());
  if (audio_stream_result != AUDIOSTREAM_SUCCESS) {
    LOG(WARNING) << __func__ << " [AudioStream] failed to OH_AudioStreamBuilder_SetChannelCount, result: "
                 << static_cast<int>(audio_stream_result);
  }
  audio_stream_result = OH_AudioStreamBuilder_SetLatencyMode(audio_stream_builder_, AUDIOSTREAM_LATENCY_MODE_NORMAL);
  if (audio_stream_result != AUDIOSTREAM_SUCCESS) {
    LOG(WARNING) << __func__ << " [AudioStream] failed to OH_AudioStreamBuilder_SetLatencyMode, result: "
                 << static_cast<int>(audio_stream_result);
  }
  audio_stream_result =
      OH_AudioStreamBuilder_SetFrameSizeInCallback(audio_stream_builder_, parameters_.frames_per_buffer());
  if (audio_stream_result != AUDIOSTREAM_SUCCESS) {
    LOG(WARNING) << __func__ << " [AudioStream] failed to OH_AudioStreamBuilder_SetFrameSizeInCallback, result: "
                 << static_cast<int>(audio_stream_result);
  }

  // set audio stream to private to prevent recording encrypted audio stream
  if (parameters_.GetStreamPrivacy()) {
    LOG(INFO) << __func__
              << " [WiseplayDRM] OH_AudioStreamBuilder_SetRendererPrivacy to AUDIO_STREAM_PRIVACY_TYPE_PRIVATE due to "
                 "play encrypted audio. ";
    audio_stream_result =
        OH_AudioStreamBuilder_SetRendererPrivacy(audio_stream_builder_, AUDIO_STREAM_PRIVACY_TYPE_PRIVATE);
    if (audio_stream_result != AUDIOSTREAM_SUCCESS) {
        LOG(ERROR) << __func__ << " [WiseplayDRM] failed to OH_AudioStreamBuilder_SetRendererPrivacy, result: "
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
        OH_AudioStreamBuilder_SetRendererInfo(audio_stream_builder_, AUDIOSTREAM_USAGE_NOTIFICATION);
    if (audio_stream_result != AUDIOSTREAM_SUCCESS) {
        LOG(WARNING) << __func__ << " [AudioStream] failed to OH_AudioStreamBuilder_SetRendererInfo, result: "
                     << static_cast<int>(audio_stream_result);
    }
  }

  // set callback
  OH_AudioRenderer_Callbacks callbacks = {};
  callbacks.OH_AudioRenderer_OnWriteData = AudioRendererOnWriteData;
  callbacks.OH_AudioRenderer_OnStreamEvent = nullptr;
  callbacks.OH_AudioRenderer_OnError = AudioRendererOnError;
  callbacks.OH_AudioRenderer_OnInterruptEvent = AudioRendererOnInterruptEvent;
  audio_stream_result = OH_AudioStreamBuilder_SetRendererCallback(audio_stream_builder_, callbacks, this);
  if (audio_stream_result != AUDIOSTREAM_SUCCESS) {
    LOG(WARNING) << __func__ << " [AudioStream] failed to OH_AudioStreamBuilder_SetRendererCallback, result: "
                 << static_cast<int>(audio_stream_result);
  }
  audio_stream_result = OH_AudioStreamBuilder_SetRendererOutputDeviceChangeCallback(
      audio_stream_builder_, AudioRendererOnOutputDeviceChange, this);
  if (audio_stream_result != AUDIOSTREAM_SUCCESS) {
    LOG(WARNING) << __func__
                 << " [AudioStream] failed to OH_AudioStreamBuilder_SetRendererOutputDeviceChangeCallback, result: "
                 << static_cast<int>(audio_stream_result);
  }
  OH_AudioStream_Result ret;
  // create audio render
  ret = OH_AudioStreamBuilder_GenerateRenderer(audio_stream_builder_,
                                               &audio_renderer_);
  if (ret != AUDIOSTREAM_SUCCESS) {
    LOG(ERROR) << "AudioStreamBuilder GenerateRenderer failed.";
    return false;
  }
  return true;
}

bool OHOSAudioOutputStream::StartRender() {
  OH_AudioStream_Result ret = OH_AudioRenderer_Start(audio_renderer_);
  if (ret != AUDIOSTREAM_SUCCESS) {
    LOG(ERROR) << "AudioRenderer Start failed, result:" << ret;
    ReleaseRender();
    ReportError();
    return false;
  }
  return true;
}

void OHOSAudioOutputStream::ReleaseRender() {
  if (!audio_renderer_) {
    return;
  }
  OH_AudioStream_Result ret = OH_AudioRenderer_Release(audio_renderer_);
  audio_renderer_ = nullptr;
  LOG(INFO) << "AudioRenderer Release, result:" << ret;
}

void OHOSAudioOutputStream::ReportError() {
  LOG(ERROR) << "ohos audio render error happened";
  reference_time_ = base::TimeTicks();
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

base::TimeDelta OHOSAudioOutputStream::GetDelayImprove(base::TimeTicks now) {
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

void OHOSAudioOutputStream::PumpSamples(void* buffer, int32_t length) {
  base::TimeTicks now = base::TimeTicks::Now();
  base::TimeDelta delay = GetDelayImprove(now);
  if (!callback_) {
    LOG(INFO) << "PumpSamples failed, callback_ is nullptr";
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

void OHOSAudioOutputStream::IdlePumpSamples() {
  base::AutoLock lock(stream_lock_);

  if (!is_suspended_) {
    LOG(ERROR) << "Not a focus preemption scenario. Exit PumpSamples.";
    return;
  }

  if (!callback_) {
    ReportError();
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

void OHOSAudioOutputStream::ScheduleIdlePumpSamples() {
  timer_.Start(FROM_HERE, time_per_buffer_,
               base::BindOnce(&OHOSAudioOutputStream::IdlePumpSamples,
                              weak_factory_.GetWeakPtr()));
}

void OHOSAudioOutputStream::OnSuspend() {
  if (!is_session_controllable_) {
    LOG(WARNING) << "Stream is not controlled by the mediaSession. ";
    return;
  }

  SuspendPlayer();
  is_suspended_ = true;
  // After stopping playback, it is necessary to continue obtaining audio data,
  // which will trigger the pause action of the render process.
  main_task_runner_->PostTask(
      FROM_HERE, base::BindOnce(&OHOSAudioOutputStream::IdlePumpSamples,
                                weak_factory_.GetWeakPtr()));
}

void OHOSAudioOutputStream::SuspendPlayer() {
  if (!audio_renderer_) {
    LOG(ERROR)
        << "OHOSAudioOutputStream::SuspendPlayer audio_renderer_ is null.";
    return;
  }

  main_task_runner_->PostTask(
      FROM_HERE,
      base::BindOnce(&content::MediaSession::Suspend, weak_media_session_,
                     content::MediaSession::SuspendType::kSystem));
}

void OHOSAudioOutputStream::OnResume() {
  if (!is_suspended_) {
    return;
  }
  if (!audio_renderer_) {
    LOG(ERROR)
        << "OHOSAudioOutputStream::SuspendPlayer audio_renderer_ is null.";
    return;
  }

  main_task_runner_->PostTask(
      FROM_HERE,
      base::BindOnce(&content::MediaSession::Resume, weak_media_session_,
                     content::MediaSession::SuspendType::kSystem));
}

void OHOSAudioOutputStream::FlushData() {
  if (!callback_) {
    LOG(ERROR) << "FlushData failed, callback_ is nullptr";
    return;
  }

  // After flushing the audio data, reset the player delay to ensure the
  // playback progress bar functions correctly.
  callback_->OnMoreData(base::TimeDelta(), base::TimeTicks::Now(), {},
                        audio_bus_.get());
}

OH_AudioStream_State OHOSAudioOutputStream::GetRenderState() {
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
