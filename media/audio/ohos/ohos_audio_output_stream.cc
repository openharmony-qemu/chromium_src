// Copyright (c) 2022 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "media/audio/ohos/ohos_audio_output_stream.h"

#include "content/public/browser/browser_task_traits.h"
#include "content/public/browser/browser_thread.h"
#include "media/base/audio_timestamp_helper.h"
#include "ohos_adapter_helper.h"
#include "ohos_nweb/src/sysevent/event_reporter.h"
#include "media/audio/ohos/ohos_audio_focus_controller.h"

namespace media {

constexpr int DEFAULT_AUDIO_ERROR_CODE = 0;

AudioAdapterSamplingRate AudioRendererOptions::GetSamplingRate() {
  return rate_;
}

AudioAdapterEncodingType AudioRendererOptions::GetEncodingType() {
  return encoding_;
}

AudioAdapterSampleFormat AudioRendererOptions::GetSampleFormat() {
  return format_;
}

AudioAdapterChannel AudioRendererOptions::GetChannel() {
  return channels_;
}

AudioAdapterContentType AudioRendererOptions::GetContentType() {
  return content_type_;
}

AudioAdapterStreamUsage AudioRendererOptions::GetStreamUsage() {
  return stream_usage_;
}

int32_t AudioRendererOptions::GetRenderFlags() {
  return renderer_flags_;
}

int32_t AudioRendererOptions::GetFrameSize() {
  return frame_size_;
}

AudioRendererCallback::AudioRendererCallback(
    const AudioParameters& params,
    const scoped_refptr<base::SingleThreadTaskRunner>& task_runner,
    base::WeakPtr<OHOSAudioOutputStream> audio_output_stream)
    : parameters_(params), main_task_runner_(task_runner),
    audio_output_stream_(audio_output_stream) {
  audioResumeInterval_ = OHOSAudioFocusController::GetAudioResumeInterval(parameters_);
  LOG(DEBUG) << "audio render callback create";
}

AudioRendererCallback::~AudioRendererCallback() {
  LOG(DEBUG) << "audio render callback destroy";
}

void AudioRendererCallback::OnSuspend() {
  LOG(DEBUG) << "AudioRendererCallback::OnSuspend";
  if (!parameters_.IsValid()) {
    LOG(ERROR) << "AudioRendererCallback::OnSuspend parameters_ is not valid.";
    return;
  }
  if (OHOSAudioFocusController::IsActive(parameters_)) {
    if (audioResumeInterval_ > 0) {
      intervalSinceLastSuspend_ = std::time(nullptr);
    }
    if (!main_task_runner_) {
      LOG(INFO) << "main task runner is nullptr";
      return;
    }
    main_task_runner_->PostTask(
        FROM_HERE,
        base::BindOnce(OHOSAudioFocusController::OnSuspend, parameters_));
    auto stream = audio_output_stream_.get();
    if (stream != nullptr) {
      stream->OnSuspend();
    }
  } else {
    LOG(INFO) << "media session is not active.";
  }
}

void AudioRendererCallback::OnResume() {
  LOG(DEBUG) << "AudioRendererCallback::OnResume audioResumeInterval is: "
             << std::time(nullptr) - intervalSinceLastSuspend_;
  if (!parameters_.IsValid()) {
    LOG(ERROR) << "AudioRendererCallback::OnResume parameters_ is not valid.";
    return;
  }
  if (audioResumeInterval_ > 0 &&
      std::time(nullptr) - intervalSinceLastSuspend_ <=
          static_cast<double>(audioResumeInterval_) &&
      OHOSAudioFocusController::IsSuspended(parameters_)) {
    if (!main_task_runner_) {
      return;
    }
    main_task_runner_->PostTask(
        FROM_HERE,
        base::BindOnce(OHOSAudioFocusController::OnResume, parameters_));
  }
}

int32_t AudioRendererCallback::OnWriteDataCallback(void* buffer, int32_t length)
{
  auto stream = audio_output_stream_.get();
  if (stream != nullptr) {
    return stream->OnWriteData(buffer, length);
  }
  return 0;
}

AudioOutputChangeCallback::AudioOutputChangeCallback(
  const scoped_refptr<base::SingleThreadTaskRunner>& main_task_runner,
  AudioParameters params,
  bool isCommunication)
    : main_task_runner_(main_task_runner), params_(params), isCommunication_(isCommunication) {}

AudioOutputChangeCallback::~AudioOutputChangeCallback() {}

void AudioOutputChangeCallback::OnOutputDeviceChange(int32_t reason) {
  LOG(INFO) << "AudioOutputChangeCallback::OnOutputDeviceChange reason: "
            << reason;
  if (reason ==
          (int32_t)AudioAdapterDeviceChangeReason::OLD_DEVICE_UNAVALIABLE &&
      !isCommunication_) {
    LOG(INFO)
        << "AudioOutputChangeCallback::OnOutputDeviceChange need stop session";
    auto OutputDeviceChangeFunc =
      [] (AudioParameters params) {
      content::RenderFrameHost* renderFrameHost =
      content::RenderFrameHost::FromID(params.render_process_id(),
                                        params.render_frame_id());
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
        base::BindOnce(OutputDeviceChangeFunc, params_));
    } else {
      OutputDeviceChangeFunc(params_);
    }
  }
}

std::map<AudioParameters, std::vector<base::WeakPtr<OHOSAudioOutputStream>>>
  OHOSAudioOutputStream::audioParameterMap_ = {};

OHOSAudioOutputStream::OHOSAudioOutputStream(OHOSAudioManager* manager,
                                             const AudioParameters& parameters,
                                             bool isCommunication)
    : manager_(manager),
      parameters_(parameters),
      audio_bus_(AudioBus::Create(parameters)),
      isCommunication_(isCommunication) {
  audio_renderer_ =
      OhosAdapterHelper::GetInstance().CreateAudioRendererAdapter();
  sample_format_ = kSampleFormatS16;
  bytes_per_frame_ = parameters.GetBytesPerFrame(sample_format_);
  buffer_size_bytes_ = parameters.GetBytesPerBuffer(sample_format_);

  time_per_buffer_ = AudioTimestampHelper::FramesToTime(
    parameters_.frames_per_buffer(), parameters_.sample_rate());
  main_task_runner_ = content::GetUIThreadTaskRunner({});
}

OHOSAudioOutputStream::~OHOSAudioOutputStream() {
  LOG(DEBUG) << "ohos audio output stream destroy";
  {
    // Ensure that OnWriteData can exit quickly and does not block the destructor.
    base::AutoLock lock(lock_);
    running_ = false;
  }

  // Close() must be called first.
  LOG(INFO) << "OHOSAudioOutputStream::~OHOSAudioOutputStream";
  if (audio_renderer_ != nullptr) {
    (void)audio_renderer_->Release();
    audio_renderer_ = nullptr;
  }
}

bool OHOSAudioOutputStream::Open() {
  base::AutoLock lock(lock_);
  std::shared_ptr<AudioRendererOptions> rendererOptions =
      std::make_shared<AudioRendererOptions>();
  rendererOptions->rate_ =
      static_cast<AudioAdapterSamplingRate>(parameters_.sample_rate());
  rendererOptions->encoding_ = AudioAdapterEncodingType::ENCODING_PCM;
  rendererOptions->format_ = AudioAdapterSampleFormat::SAMPLE_S16LE;
  rendererOptions->channels_ =
      static_cast<AudioAdapterChannel>(parameters_.channels());
  rendererOptions->content_type_ =
      isCommunication_ ? AudioAdapterContentType::CONTENT_TYPE_SPEECH
                       : AudioAdapterContentType::CONTENT_TYPE_MUSIC;
  rendererOptions->stream_usage_ =
      isCommunication_
          ? AudioAdapterStreamUsage::STREAM_USAGE_VOICE_COMMUNICATION
          : AudioAdapterStreamUsage::STREAM_USAGE_MEDIA;
  rendererOptions->renderer_flags_ = 0;
  rendererOptions->frame_size_ = parameters_.frames_per_buffer();

  if (!InitRender(rendererOptions)) {
    return false;
  }
  return true;
}

void OHOSAudioOutputStream::Close() {
  LOG(INFO) << "OHOSAudioOutputStream::Close";
  Stop();
  manager_->ReleaseOutputStream(this);
}

void OHOSAudioOutputStream::SuspendOtherMediaSession() {
  bool registered = false;
  auto it = audioParameterMap_.begin();
  while (it != audioParameterMap_.end()) {
    if (!(it->first).IsValid() || ((it->first).render_process_id() <= 0 || (it->first).render_frame_id() <= 0)) {
      LOG(INFO) << "Delete invalid parameter.";
      it = audioParameterMap_.erase(it);
      continue;
    }
    if ((it->first).Equals(parameters_)) {
      LOG(INFO) << "skip mediaSession control because of same audioparameters.";
      it->second.push_back(weak_factory_.GetWeakPtr());
      it++;
      registered = true;
      continue;
    }
    if (GetInterruptMode()) {
      LOG(INFO) << "MediaSession is suspending the audio in other web.";
      main_task_runner_->PostTask(
          FROM_HERE,
          base::BindOnce(OHOSAudioFocusController::OnSuspend, (it->first)));
      std::vector<base::WeakPtr<OHOSAudioOutputStream>> vec = it->second;
      for (auto weakStream = vec.begin(); weakStream != vec.end(); ++weakStream) {
          auto stream = weakStream->get();
          if (stream != nullptr) {
              main_task_runner_->PostTask(FROM_HERE,
                  base::BindOnce(&OHOSAudioOutputStream::OnSuspend, base::Unretained(stream)));
          } 
      }
    }
    it++;
  }
  if (!registered) {
    audioParameterMap_.insert(std::make_pair(parameters_, std::vector<base::WeakPtr<OHOSAudioOutputStream>>()));
  }
}

void OHOSAudioOutputStream::Start(AudioSourceCallback* callback) {
  LOG(INFO) << "OHOSAudioOutputStream::Start";
  base::AutoLock lock(lock_);
  DCHECK(!callback_);
  DCHECK(!timer_.IsRunning());
  isSuspended_ = false;
  int32_t ret = audio_renderer_->SetAudioRendererCallback(rendererCallback_);
  if (ret != AudioAdapterCode::AUDIO_OK) {
    LOG(ERROR)
        << "OHOSAudioOutputStream::Start Set audio renderer callback failed.";
    return;
  }
  ret = audio_renderer_->SetAudioOutputChangeCallback(outputChangeCallback_);
  if (ret != AudioAdapterCode::AUDIO_OK) {
    LOG(ERROR) << "OHOSAudioOutputStream::Start Set audio output change "
                  "callback failed.";
    return;
  }
  Prepare(parameters_);

  SuspendOtherMediaSession();

  if (StartRender()) {
    DCHECK(!timer_.IsRunning());
    callback_ = callback;
    running_ = true;
  }
}

void OHOSAudioOutputStream::Stop() {
  LOG(INFO) << "OHOSAudioOutputStream::Stop";
  base::AutoLock lock(lock_);
  callback_ = nullptr;

  running_ = false;
  timer_.Stop();
  audioParameterMap_.erase(parameters_);
  if (rendererCallback_ && isSuspended_) {
    LOG(DEBUG) << "OHOSAudioOutputStream::Stop cannot continue.";
    return;
  }
  if (!audio_renderer_->Stop()) {
    ReportError();
  }
}

bool OHOSAudioOutputStream::GetInterruptMode() {
  return audioExclusive_;
}

void OHOSAudioOutputStream::SetInterruptMode(bool audioExclusive) {
  LOG(INFO) << "OHOSAudioOutputStream::SetInterruptMode audioExclusive: "
            << audioExclusive;
  if (!audio_renderer_) {
    LOG(ERROR)
        << "OHOSAudioOutputStream::SetInterruptMode audio_renderer_ is null.";
    return;
  }
  audioExclusive_ = audioExclusive;
  audio_renderer_->SetInterruptMode(false);
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

bool OHOSAudioOutputStream::InitRender(
    const std::shared_ptr<AudioRendererOptionsAdapter> rendererOptions) {
    int32_t ret = audio_renderer_->Create(rendererOptions);
  if (ret != 0) {
    if (!audio_renderer_->Release()) {
      LOG(ERROR) << "ohos audio render release failed.";
    }
    return false;
  }
  if (!parameters_.IsValid()) {
    LOG(ERROR) << "OHOSAudioOutputStream::InitRender parameters is not valid.";
    return false;
  }
  rendererCallback_ = std::make_shared<AudioRendererCallback>(
      parameters_, main_task_runner_, weak_factory_.GetWeakPtr());
  if (!rendererCallback_) {
    LOG(ERROR)
        << "OHOSAudioOutputStream::InitRender Get rendererCallback failed.";
    return false;
  }
  outputChangeCallback_ = std::make_shared<AudioOutputChangeCallback>(
      main_task_runner_, parameters_, isCommunication_);
  if (!outputChangeCallback_) {
    LOG(ERROR) << "OHOSAudioOutputStream::InitRender Get outputChangeCallback_ "
                  "failed.";
    return false;
  }
  if (ret != AudioAdapterCode::AUDIO_OK) {
    LOG(ERROR) << "OHOSAudioOutputStream::InitRender Set audio renderer "
                  "callback failed.";
    rendererCallback_.reset();
    outputChangeCallback_.reset();
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
    std::string errorType = "audio play error";
    int errorCode = DEFAULT_AUDIO_ERROR_CODE;
    std::string errorDesc = "audio renderer start failed";
    ReportAudioPlayErrorInfo(errorType, errorCode, errorDesc);
    return false;
  }
  isSuspended_ = false;
  return true;
}

void OHOSAudioOutputStream::Prepare(const AudioParameters& parameters) {
  LOG(INFO) << "OHOSAudioOutputStream::Prepare";
  if (!parameters_.IsValid()) {
    LOG(ERROR) << "OHOSAudioOutputStream::Prepare parameters_ is not valid.";
    return;
  }
  bool audioExclusive = OHOSAudioFocusController::GetAudioExclusive(parameters);
  SetInterruptMode(audioExclusive);
}

void OHOSAudioOutputStream::ReportError() {
  LOG(ERROR) << "ohos audio render error happened";
  running_ = false;
  timer_.Stop();
  if (callback_) {
    callback_->OnError(AudioSourceCallback::ErrorType::kUnknown);
  }
}


int32_t OHOSAudioOutputStream::OnWriteData(void* buffer, int32_t length) {
  base::AutoLock lock(lock_);
  if (!running_) {
    return 0;
  }

  base::TimeTicks now = base::TimeTicks::Now();
  uint64_t latency = 0;
  audio_renderer_->GetLatency(latency);
  base::TimeDelta delay = base::Nanoseconds(latency);
  
  // Request more samples from |callback_|.
  if (!callback_) {
    std::string errorType = "audio play error";
    int errorCode = DEFAULT_AUDIO_ERROR_CODE;
    std::string errorDesc = "audio renderer get AudioSourceCallback failed";
    ReportAudioPlayErrorInfo(errorType, errorCode, errorDesc);
    ReportError();
    return 0;
  }
  int frames_filled = callback_->OnMoreData(delay, now, {}, audio_bus_.get());
  DCHECK_EQ(frames_filled, audio_bus_->frames());
  audio_bus_->Scale(volume_);
  audio_bus_->ToInterleaved<SignedInt16SampleTypeTraits>(
      frames_filled, reinterpret_cast<int16_t*>(buffer));

  return frames_filled;
}

void OHOSAudioOutputStream::OnSuspend() {
  LOG(INFO) << "OHOSAudioOutputStream::OnSuspend";
  isSuspended_ = true;
  // After stopping playback, it is necessary to continue obtaining audio data,
  // which will trigger the pause action of the render process.
  main_task_runner_->PostTask(FROM_HERE,
      base::BindOnce(&OHOSAudioOutputStream::PumpSamples,
                      base::Unretained(this)));
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
    std::string errorType = "audio play error";
    int errorCode = DEFAULT_AUDIO_ERROR_CODE;
    std::string errorDesc = "audio renderer get AudioSourceCallback failed";
    ReportAudioPlayErrorInfo(errorType, errorCode, errorDesc);
    ReportError();
    return;
  }

  // Obtains data. The data does not need to be processed and may be empty.
  (void)callback_->OnMoreData(base::TimeDelta(), base::TimeTicks::Now(), {}, audio_bus_.get());
  SchedulePumpSamples();
}

void OHOSAudioOutputStream::SchedulePumpSamples() {
  timer_.Start(FROM_HERE, time_per_buffer_,
               base::BindOnce(&OHOSAudioOutputStream::PumpSamples,
                              base::Unretained(this)));
}
}  // namespace media
