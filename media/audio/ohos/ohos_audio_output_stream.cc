// Copyright (c) 2022 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "media/audio/ohos/ohos_audio_output_stream.h"

#include "content/public/browser/browser_task_traits.h"
#include "content/public/browser/browser_thread.h"
#include "media/base/audio_timestamp_helper.h"
#include "ohos_adapter_helper.h"

namespace media {

AudioRendererCallback::AudioRendererCallback(
    content::MediaSessionImpl* media_session)
    : media_session_(media_session) {}

AudioRendererCallback::~AudioRendererCallback() {}

void AudioRendererCallback::OnSuspend() {
  LOG(DEBUG) << "AudioRendererCallback::OnSuspend";
  if (!media_session_) {
    LOG(ERROR) << "AudioRendererCallback::OnSuspend media_session_ is null.";
    return;
  }
  if (media_session_->audioResumeInterval_ > 0) {
    intervalSinceLastSuspend_ = std::time(nullptr);
  }
}

void AudioRendererCallback::OnResume() {
  LOG(DEBUG) << "AudioRendererCallback::OnResume audioResumeInterval is: "
             << std::time(nullptr) - intervalSinceLastSuspend_;
  if (!media_session_) {
    LOG(ERROR) << "AudioRendererCallback::OnResume media_session_ is null.";
    return;
  }
  if (media_session_->audioResumeInterval_ > 0 &&
      std::time(nullptr) - intervalSinceLastSuspend_ <=
          static_cast<double>(media_session_->audioResumeInterval_) &&
      media_session_->IsSuspended()) {
    media_session_->Resume(content::MediaSession::SuspendType::kSystem);
  }
}

bool AudioRendererCallback::GetSuspendFlag() {
  return suspendFlag_;
}

void AudioRendererCallback::SetSuspendFlag(bool flag) {
  suspendFlag_ = flag;
}

OHOSAudioOutputStream::OHOSAudioOutputStream(OHOSAudioManager* manager,
                                             const AudioParameters& parameters,
                                             bool isCommunication)
    : manager_(manager),
      parameters_(parameters),
      audio_bus_(AudioBus::Create(parameters)),
      isCommunication_(isCommunication) {
  content::RenderFrameHost* renderFrameHost = content::RenderFrameHost::FromID(
      parameters_.render_process_id(), parameters_.render_frame_id());
  content::WebContents* webContent =
      content::WebContents::FromRenderFrameHost(renderFrameHost);
  if (!webContent) {
    LOG(ERROR) << "AudioOutputStream get webContent failed.";
  } else {
    content::MediaSessionImpl* mediaSession =
        content::MediaSessionImpl::Get(webContent);
    if (!mediaSession) {
      LOG(ERROR) << "AudioOutputStream get mediaSession failed.";
    }
    weakMediaSession_ = mediaSession->weakMediaSessionFactory_.GetWeakPtr();
    if (!weakMediaSession_)
      LOG(ERROR) << "OHOSAudioOutputStream::OHOSAudioOutputStream "
                    "weakMediaSession get failed";
  }
  audio_renderer_ =
      OhosAdapterHelper::GetInstance().CreateAudioRendererAdapter();
  sample_format_ = kSampleFormatS16;
  bytes_per_frame_ = parameters.GetBytesPerFrame(sample_format_);
  buffer_size_bytes_ = parameters.GetBytesPerBuffer(sample_format_);

  for (int i = 0; i < kMaxNumOfBuffersInQueue; i++) {
    audio_data_[i] = nullptr;
  }
  main_task_runner_ = content::GetUIThreadTaskRunner({});
}

OHOSAudioOutputStream::~OHOSAudioOutputStream() {
  // Close() must be called first.
  LOG(INFO) << "OHOSAudioOutputStream::~OHOSAudioOutputStream";
  if (audio_renderer_ != nullptr) {
    (void)audio_renderer_->Release();
    audio_renderer_ = nullptr;
  }
}

bool OHOSAudioOutputStream::Open() {
  AudioAdapterRendererOptions rendererOptions;
  rendererOptions.samplingRate =
      static_cast<AudioAdapterSamplingRate>(parameters_.sample_rate());
  rendererOptions.encoding = AudioAdapterEncodingType::ENCODING_PCM;
  rendererOptions.format = AudioAdapterSampleFormat::SAMPLE_S16LE;
  rendererOptions.channels =
      static_cast<AudioAdapterChannel>(parameters_.channels());
  rendererOptions.contentType =
      isCommunication_ ? AudioAdapterContentType::CONTENT_TYPE_SPEECH
                       : AudioAdapterContentType::CONTENT_TYPE_MUSIC;
  rendererOptions.streamUsage =
      isCommunication_
          ? AudioAdapterStreamUsage::STREAM_USAGE_VOICE_COMMUNICATION
          : AudioAdapterStreamUsage::STREAM_USAGE_MEDIA;
  rendererOptions.rendererFlags = 0;

  if (!InitRender(rendererOptions)) {
    return false;
  }
  if (!SetupAudioBuffer()) {
    return false;
  }
  active_buffer_index_ = 0;
  return true;
}

void OHOSAudioOutputStream::Close() {
  LOG(INFO) << "OHOSAudioOutputStream::Close";
  Stop();
  ReleaseAudioBuffer();
  manager_->ReleaseOutputStream(this);
}

void OHOSAudioOutputStream::Start(AudioSourceCallback* callback) {
  LOG(INFO) << "OHOSAudioOutputStream::Start";
  DCHECK(!callback_);
  DCHECK(reference_time_.is_null());
  DCHECK(!timer_.IsRunning());
  rendererCallback_->SetSuspendFlag(false);
  int32_t ret = audio_renderer_->SetAudioRendererCallback(rendererCallback_);
  if (ret != AudioAdapterCode::AUDIO_OK) {
    LOG(ERROR)
        << "OHOSAudioOutputStream::Start Set audio renderer callback failed.";
    rendererCallback_.reset();
    return;
  }
  main_task_runner_->PostTask(
      FROM_HERE, base::BindOnce(&OHOSAudioOutputStream::Prepare,
                                base::Unretained(this), weakMediaSession_));
  if (StartRender()) {
    callback_ = callback;
    memset(audio_data_[active_buffer_index_], 0, buffer_size_bytes_);
    active_buffer_index_ = (active_buffer_index_ + 1) % kMaxNumOfBuffersInQueue;
    if (callback_) {
      DCHECK(!timer_.IsRunning());
      PumpSamples();
    }
  }
}

void OHOSAudioOutputStream::Stop() {
  LOG(INFO) << "OHOSAudioOutputStream::Stop";
  main_task_runner_->PostTask(
      FROM_HERE, base::BindOnce(&OHOSAudioOutputStream::Erase,
                                base::Unretained(this), weakMediaSession_));
  callback_ = nullptr;
  if (!reference_time_.is_null()) {
    reference_time_ = base::TimeTicks();
  }
  timer_.Stop();
  if (rendererCallback_ && rendererCallback_->GetSuspendFlag()) {
    LOG(DEBUG) << "OHOSAudioOutputStream::Stop cannot continue.";
    return;
  }
  if (!audio_renderer_->Stop()) {
    ReportError();
  }
}

void OHOSAudioOutputStream::Refresh() {
  LOG(INFO) << "OHOSAudioOutputStream::Refresh";
  if (!audio_renderer_) {
    LOG(ERROR) << "OHOSAudioOutputStream::Refresh audio_renderer_ is null.";
    return;
  }
  isRefreshing_ = true;
  audio_renderer_->Stop();
  audio_renderer_->Start();
  isRefreshing_ = false;
}

void OHOSAudioOutputStream::SetInterruptMode(bool audioExclusive) {
  LOG(INFO) << "OHOSAudioOutputStream::SetInterruptMode audioExclusive: "
            << audioExclusive;
  if (!audio_renderer_) {
    LOG(ERROR)
        << "OHOSAudioOutputStream::SetInterruptMode audio_renderer_ is null.";
    return;
  }
  audio_renderer_->SetInterruptMode(audioExclusive);
}

// This stream is always used with sub second buffer sizes, where it's
// sufficient to simply always flush upon Start().
void OHOSAudioOutputStream::Flush() {}

void OHOSAudioOutputStream::SetVolume(double volume) {
  DCHECK(0.0 <= volume && volume <= 1.0) << volume;
  (void)audio_renderer_->SetVolume(volume);
}

void OHOSAudioOutputStream::GetVolume(double* volume) {
  *volume = static_cast<double>(audio_renderer_->GetVolume());
}

base::TimeTicks OHOSAudioOutputStream::GetCurrentStreamTime() {
  DCHECK(!reference_time_.is_null());
  return reference_time_ +
         AudioTimestampHelper::FramesToTime(stream_position_samples_,
                                            parameters_.sample_rate());
}

bool OHOSAudioOutputStream::InitRender(
    const AudioAdapterRendererOptions& rendererOptions) {
  int32_t ret = audio_renderer_->Create(rendererOptions);
  if (ret != 0) {
    if (!audio_renderer_->Release()) {
      LOG(ERROR) << "ohos audio render release failed.";
    }
    return false;
  }
  if (!weakMediaSession_) {
    LOG(ERROR) << "OHOSAudioOutputStream::InitRender Get mediaSession failed.";
    return false;
  }
  rendererCallback_ =
      std::make_shared<AudioRendererCallback>(weakMediaSession_.get());
  if (!rendererCallback_) {
    LOG(ERROR)
        << "OHOSAudioOutputStream::InitRender Get rendererCallback failed.";
    return false;
  }
  if (ret != AudioAdapterCode::AUDIO_OK) {
    LOG(ERROR) << "OHOSAudioOutputStream::InitRender Set audio renderer "
                  "callback failed.";
    rendererCallback_.reset();
    return false;
  }
  return true;
}

bool OHOSAudioOutputStream::StartRender() {
  if (!audio_renderer_->Start()) {
    LOG(ERROR) << "ohos audio render start failed";
    if (!audio_renderer_->Release()) {
      LOG(ERROR) << "ohos audio render release failed";
    }
    ReportError();
    return false;
  }
  return true;
}

void OHOSAudioOutputStream::Erase(
    base::WeakPtr<content::MediaSessionImpl> weakMediaSession) {
  LOG(INFO) << "OHOSAudioOutputStream::Erase";
  if (!weakMediaSession) {
    LOG(ERROR) << "OHOSAudioOutputStream::Erase mediaSession is null";
    return;
  }
  content::MediaSessionImpl* mediaSession = weakMediaSession.get();
  if (mediaSession && !mediaSession->activeAudioStream_.empty()) {
    mediaSession->activeAudioStream_.erase(this);
    for (auto stream = mediaSession->activeAudioStream_.begin();
         stream != mediaSession->activeAudioStream_.end();) {
      if (!(*stream)) {
        LOG(ERROR) << "OHOSAudioOutputStream::Erase the active stream is null";
        stream = mediaSession->activeAudioStream_.erase(stream);
        continue;
      }
      if (mediaSession->isStreamSuspended_) {
        LOG(INFO) << "OHOSAudioOutputStream::Erase has suspended stream";
        (*stream)->Stop();
      }
      ++stream;
    }
  }
}

void OHOSAudioOutputStream::Prepare(
    base::WeakPtr<content::MediaSessionImpl> weakMediaSession) {
  LOG(INFO) << "OHOSAudioOutputStream::Prepare";
  if (!weakMediaSession) {
    LOG(ERROR) << "OHOSAudioOutputStream::Prepare mediaSession is null";
    return;
  }
  content::MediaSessionImpl* mediaSession = weakMediaSession.get();
  if (mediaSession && !mediaSession->activeAudioStream_.empty()) {
    for (auto stream = mediaSession->activeAudioStream_.begin();
         stream != mediaSession->activeAudioStream_.end();) {
      LOG(INFO)
          << "OHOSAudioOutputStream::Prepare refresh other active streams";
      if (!(*stream)) {
        LOG(ERROR)
            << "OHOSAudioOutputStream::Prepare the active stream is null";
        stream = mediaSession->activeAudioStream_.erase(stream);
        continue;
      }
      (*stream)->SetInterruptMode(false);
      (*stream)->Refresh();
      ++stream;
    }
    LOG(ERROR) << "OHOSAudioOutputStream::Prepare setInterruptMode";
    SetInterruptMode(false);
  } else {
    SetInterruptMode(mediaSession->audioExclusive_);
  }
  mediaSession->activeAudioStream_.insert(this);
  mediaSession->isStreamSuspended_ = false;
}

void OHOSAudioOutputStream::ReportError() {
  LOG(ERROR) << "ohos audio render error happened";
  reference_time_ = base::TimeTicks();
  timer_.Stop();
  if (callback_)
    callback_->OnError(AudioSourceCallback::ErrorType::kUnknown);
}

void OHOSAudioOutputStream::PumpSamples() {
  base::TimeTicks now = base::TimeTicks::Now();
  base::TimeDelta delay;

  if (reference_time_.is_null()) {
    stream_position_samples_ = 0;
  } else {
    auto stream_time = GetCurrentStreamTime();
    // Adjust stream position if we missed timer deadline.
    if (now > stream_time + parameters_.GetBufferDuration() / 2) {
      stream_position_samples_ += AudioTimestampHelper::TimeToFrames(
          now - stream_time - parameters_.GetBufferDuration() / 2,
          parameters_.sample_rate());
    }

    uint64_t latency = 0;
    audio_renderer_->GetLatency(latency);
    delay = base::Microseconds(latency);
  }

  // Request more samples from |callback_|.
  int frames_filled = callback_->OnMoreData(delay, now, 0, audio_bus_.get());
  DCHECK_EQ(frames_filled, audio_bus_->frames());
  audio_bus_->Scale(volume_);
  audio_bus_->ToInterleaved<SignedInt16SampleTypeTraits>(
      frames_filled,
      reinterpret_cast<int16_t*>(audio_data_[active_buffer_index_]));
  const size_t num_filled_bytes = frames_filled * bytes_per_frame_;
  size_t bytesWritten = 0;
  while (bytesWritten < num_filled_bytes) {
    int32_t bytesSingle =
        audio_renderer_->Write(audio_data_[active_buffer_index_] + bytesWritten,
                               num_filled_bytes - bytesWritten);
    if (bytesSingle <= 0) {
      LOG(DEBUG) << "Audio renderer write audio data failed.";
      if (!audio_renderer_->IsRendererStateRunning() && !isRefreshing_) {
        rendererCallback_->SetSuspendFlag(true);
        if (!weakMediaSession_) {
          LOG(ERROR) << "Try to suspend audio but get mediaSession failed.";
          ReportError();
          return;
        }
        if (weakMediaSession_.get()->IsActive()) {
          LOG(INFO) << "MediaSession is suspending the audio.";
          weakMediaSession_.get()->Suspend(
              content::MediaSession::SuspendType::kSystem);
          weakMediaSession_.get()->isStreamSuspended_ = true;
        }
      } else {
        ReportError();
        return;
      }
      break;
    }
    bytesWritten += bytesSingle;
  }

  active_buffer_index_ = (active_buffer_index_ + 1) % kMaxNumOfBuffersInQueue;

  if (reference_time_.is_null()) {
    reference_time_ = now;
  }

  stream_position_samples_ += frames_filled;
  SchedulePumpSamples(now);
}

void OHOSAudioOutputStream::SchedulePumpSamples(base::TimeTicks now) {
  timer_.Start(FROM_HERE, GetCurrentStreamTime() - now,
               base::BindOnce(&OHOSAudioOutputStream::PumpSamples,
                              base::Unretained(this)));
}

bool OHOSAudioOutputStream::SetupAudioBuffer() {
  DCHECK(!audio_data_[0]);
  for (int i = 0; i < kMaxNumOfBuffersInQueue; ++i) {
    audio_data_[i] = new uint8_t[buffer_size_bytes_];
    if (!audio_data_[i]) {
      LOG(ERROR) << "audio stream buffer setup failed";
      return false;
    }
  }
  return true;
}

void OHOSAudioOutputStream::ReleaseAudioBuffer() {
  for (int i = 0; i < kMaxNumOfBuffersInQueue; ++i) {
    if (audio_data_[i]) {
      delete[] audio_data_[i];
      audio_data_[i] = nullptr;
    }
  }
}
}  // namespace media
