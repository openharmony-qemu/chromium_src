// Copyright (c) 2022 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "media/audio/ohos/ohos_audio_output_stream.h"

#include <ctime>

#include "content/public/browser/browser_task_traits.h"
#include "content/public/browser/browser_thread.h"
#include "media/audio/ohos/audio_dump.h"
#include "media/base/audio_timestamp_helper.h"
#include "ohos_adapter_helper.h"
#include "ohos_nweb/src/sysevent/event_reporter.h"
#include "third_party/bounds_checking_function/include/securec.h"

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

AudioAdapterConcurrencyMode AudioRendererOptions::GetConcurrencyMode() {
  return concurrency_mode_;
}

AudioRendererCallback::AudioRendererCallback(
    content::MediaSessionImpl* media_session,
    const scoped_refptr<base::SingleThreadTaskRunner>& task_runner)
    : media_session_(media_session), main_task_runner_(task_runner) {}

AudioRendererCallback::~AudioRendererCallback() {}

void AudioRendererCallback::OnSuspend() {
  LOG(INFO) << "AudioRendererCallback::OnSuspend";
  if (!media_session_) {
    LOG(ERROR) << "AudioRendererCallback::OnSuspend media_session_ is null.";
    return;
  }
  if (media_session_->audioResumeInterval_ > 0) {
    intervalSinceLastSuspend_ = std::time(nullptr);
  }
}

void AudioRendererCallback::OnResume() {
  LOG(INFO) << "AudioRendererCallback::OnResume audioResumeInterval is: "
             << std::time(nullptr) - intervalSinceLastSuspend_;
  if (!media_session_) {
    LOG(ERROR) << "AudioRendererCallback::OnResume media_session_ is null.";
    return;
  }
  if (media_session_->audioResumeInterval_ > 0 &&
      std::time(nullptr) - intervalSinceLastSuspend_ <=
          static_cast<double>(media_session_->audioResumeInterval_) &&
      media_session_->IsSuspended()) {
    if (!main_task_runner_) {
      return;
    }
    LOG(INFO) << "AudioRendererCallback::OnResume MediaSession Resume.";
    main_task_runner_->PostTask(
        FROM_HERE,
        base::BindOnce(&content::MediaSessionImpl::Resume,
                       media_session_->weakMediaSessionFactory_.GetWeakPtr(),
                       content::MediaSession::SuspendType::kSystem));
  }
}

bool AudioRendererCallback::GetSuspendFlag() {
  return suspendFlag_;
}

void AudioRendererCallback::SetSuspendFlag(bool flag) {
  suspendFlag_ = flag;
}

AudioOutputChangeCallback::AudioOutputChangeCallback(
    const scoped_refptr<base::SingleThreadTaskRunner>& main_task_runner,
    AudioParameters params,
    bool isCommunication)
    : main_task_runner_(main_task_runner),
      params_(params),
      isCommunication_(isCommunication) {}

AudioOutputChangeCallback::~AudioOutputChangeCallback() {}

void AudioOutputChangeCallback::OnOutputDeviceChange(int32_t reason) {
  LOG(INFO) << "AudioOutputChangeCallback::OnOutputDeviceChange reason: "
            << reason;
  if (reason ==
          (int32_t)AudioAdapterDeviceChangeReason::OLD_DEVICE_UNAVALIABLE &&
      !isCommunication_) {
    LOG(INFO)
        << "AudioOutputChangeCallback::OnOutputDeviceChange need stop session";
    auto OutputDeviceChangeFunc = [](AudioParameters params) {
      content::RenderFrameHost* renderFrameHost =
          content::RenderFrameHost::FromID(params.render_process_id(),
                                           params.render_frame_id());
      if (!renderFrameHost) {
        LOG(ERROR) << "AudioOutputStream get renderhost failed.";
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
      auto weakMediaSession =
          mediaSession->weakMediaSessionFactory_.GetWeakPtr();
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
          FROM_HERE, base::BindOnce(OutputDeviceChangeFunc, params_));
    } else {
      OutputDeviceChangeFunc(params_);
    }
  }
}

static std::set<content::WebContents*> WEBCONTENT_SET = {};

OHOSAudioOutputStream::OHOSAudioOutputStream(OHOSAudioManager* manager,
                                             const AudioParameters& parameters,
                                             bool isCommunication)
    : manager_(manager),
      parameters_(parameters),
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
  rendererOptions->concurrency_mode_ = AudioAdapterConcurrencyMode::INVALID;
  if (weakMediaSession_) {
    media::MediaContentType contentType = weakMediaSession_.get()->getMediaContentType();
    if (contentType == media::MediaContentType::Transient) {
      rendererOptions->concurrency_mode_ = AudioAdapterConcurrencyMode::DUCK_OTHERS;
    }
  }

  if (!InitRender(rendererOptions)) {
    return false;
  }
  if (!SetupAudioBuffer()) {
    return false;
  }
  active_buffer_index_ = 0;
  time_t now = time(nullptr);
  std::string dumpFileName = std::to_string(now) + "_" +
      std::to_string(parameters_.sample_rate()) + "_" +
      std::to_string(parameters_.channels()) + "_" +
      std::to_string(1) + "_output_write.pcm";
  DumpFileUtil::OpenDumpFile(dumpFileName, &dumpFile_);
  return true;
}

void OHOSAudioOutputStream::Close() {
  LOG(INFO) << "OHOSAudioOutputStream::Close";
  Stop();
  DumpFileUtil::CloseDumpFile(&dumpFile_);
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
    return;
  }
  ret = audio_renderer_->SetAudioOutputChangeCallback(outputChangeCallback_);
  if (ret != AudioAdapterCode::AUDIO_OK) {
    LOG(ERROR) << "OHOSAudioOutputStream::Start Set audio output change "
                  "callback failed.";
    return;
  }
  Prepare(weakMediaSession_);

  auto it = WEBCONTENT_SET.begin();
  while (it != WEBCONTENT_SET.end()) {
    auto otherMediaSession = content::MediaSessionImpl::FromWebContents(*it);
    if (!otherMediaSession) {
      it = WEBCONTENT_SET.erase(it);
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

  WEBCONTENT_SET.insert(webContent_);
  if (StartRender()) {
    callback_ = callback;
    if (memset_s(audio_data_[active_buffer_index_],
        buffer_size_bytes_, 0, buffer_size_bytes_) != EOK) {
      LOG(ERROR) << "audio data memset_s failed.";
      return;
    }
    active_buffer_index_ = (active_buffer_index_ + 1) % kMaxNumOfBuffersInQueue;
    if (callback_) {
      DCHECK(!timer_.IsRunning());
      FlushData();
      PumpSamples();
    }
  }
}

void OHOSAudioOutputStream::Stop() {
  LOG(INFO) << "OHOSAudioOutputStream::Stop";
  callback_ = nullptr;
  if (!reference_time_.is_null()) {
    reference_time_ = base::TimeTicks();
  }
  timer_.Stop();
  WEBCONTENT_SET.erase(webContent_);
  if (rendererCallback_ && rendererCallback_->GetSuspendFlag()) {
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

base::TimeTicks OHOSAudioOutputStream::GetCurrentStreamTime() {
  DCHECK(!reference_time_.is_null());
  return reference_time_ +
         AudioTimestampHelper::FramesToTime(stream_position_samples_,
                                            parameters_.sample_rate());
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
  if (!weakMediaSession_) {
    LOG(ERROR) << "OHOSAudioOutputStream::InitRender Get mediaSession failed.";
    return false;
  }
  rendererCallback_ = std::make_shared<AudioRendererCallback>(
      weakMediaSession_.get(), main_task_runner_);
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
  if (IsPreloadOrMutedMediaMode()) {
    audio_renderer_->SetAudioSilentMode(true);
    LOG(INFO) << "OHOSAudioOutputStream SetAudioSilentMode true";
    isSilentMode_ = true;
    silentFrameNum_ = 0;
  }
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
  SetInterruptMode(mediaSession->audioExclusive_);
}

void OHOSAudioOutputStream::ReportError() {
  LOG(ERROR) << "ohos audio render error happened";
  reference_time_ = base::TimeTicks();
  timer_.Stop();
  if (callback_) {
    callback_->OnError(AudioSourceCallback::ErrorType::kUnknown);
  }
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
  if (!callback_) {
    std::string errorType = "audio play error";
    int errorCode = DEFAULT_AUDIO_ERROR_CODE;
    std::string errorDesc = "audio renderer get AudioSourceCallback failed";
    ReportAudioPlayErrorInfo(errorType, errorCode, errorDesc);
    ReportError();
    return;
  }
  int frames_filled = callback_->OnMoreData(delay, now, {}, audio_bus_.get());
  DCHECK_EQ(frames_filled, audio_bus_->frames());
  audio_bus_->Scale(volume_);
  audio_bus_->ToInterleaved<SignedInt16SampleTypeTraits>(
      frames_filled,
      reinterpret_cast<int16_t*>(audio_data_[active_buffer_index_]));
  const size_t num_filled_bytes = frames_filled * bytes_per_frame_;
  size_t bytesWritten = 0;
  bool writeFailed = false;
  while (bytesWritten < num_filled_bytes) {
    int32_t bytesSingle =
        audio_renderer_->Write(audio_data_[active_buffer_index_] + bytesWritten,
                               num_filled_bytes - bytesWritten);
    SetUpAudioSilentState(bytesSingle);
    if (bytesSingle <= 0) {
      LOG(DEBUG) << "Audio renderer write audio data failed";
      if (!audio_renderer_->IsRendererStateRunning()) {
        rendererCallback_->SetSuspendFlag(true);
        writeFailed = true;
        if (!weakMediaSession_) {
          LOG(ERROR) << "Try to suspend audio but get mediaSession failed";
          ReportError();
          std::string errorType = "audio play error";
          int errorCode = DEFAULT_AUDIO_ERROR_CODE;
          std::string errorDesc = "audio renderer get MediaSession failed";
          ReportAudioPlayErrorInfo(errorType, errorCode, errorDesc);
          return;
        }
        if (weakMediaSession_.get()->IsActive()) {
          if (isSuspended_) {
            LOG(INFO) << "AudioStream should be restarted";
            if (!audio_renderer_->Start()) {
              LOG(ERROR) << "Restarted audioStream but failed";
            }
            writeFailed = false;
            isSuspended_ = false;
            break;
          }
          LOG(INFO) << "MediaSession is suspending the audio";
          main_task_runner_->PostTask(
              FROM_HERE,
              base::BindOnce(&content::MediaSessionImpl::Suspend, weakMediaSession_,
                             content::MediaSession::SuspendType::kSystem));
        } else {
          LOG(DEBUG) << "MediaSession is suspended";
          isSuspended_ = true;
        }
      } else {
        ReportError();
        std::string errorType = "audio play error";
        int errorCode = DEFAULT_AUDIO_ERROR_CODE;
        std::string errorDesc = "audio renderer running state error";
        ReportAudioPlayErrorInfo(errorType, errorCode, errorDesc);
        return;
      }
      break;
    }
    DumpFileUtil::WriteDumpFile(
        dumpFile_, audio_data_[active_buffer_index_] + bytesWritten,
        num_filled_bytes - bytesWritten
    );
    bytesWritten += bytesSingle;
  }

  active_buffer_index_ = (active_buffer_index_ + 1) % kMaxNumOfBuffersInQueue;

  if (reference_time_.is_null()) {
    reference_time_ = now;
  }

  stream_position_samples_ += frames_filled;
  if (writeFailed && weakMediaSession_ && (weakMediaSession_.get()->HasOnlyOneShotPlayersPublic() ||
      weakMediaSession_.get()->GetSessionState() == content::MediaSessionImpl::NWebMediaSessionState::NOINITIAL)) {
    LOG(INFO) << "OHOSAudioOutputStream::PumpSamples OneShotPlayers write failed";
#ifdef OHOS_PERFORMANCE_PERSISTENT_TASK
    if (!webContent_) {
      LOG(ERROR) << "AudioOutputStream get webContent failed.";
      return;
    }
    webContent_->OneShotMediaPlayerStopped();
#endif
    return;
  }
  SchedulePumpSamples(now);
}

void OHOSAudioOutputStream::SetUpAudioSilentState(int32_t bytesSingle)
{
  if (!weakMediaSession_ || !audio_renderer_) {
    LOG(ERROR) << "OHOSAudioOutputStream: Try to set audio silent but get mediaSession or audioRender failed!";
    return;
  }
  if (isSilentMode_) {
    bool is_playing = weakMediaSession_.get()->GetPlayingState() || silentFrameNum_ >= 1;
    bool is_muted = weakMediaSession_.get()->GetMuteState();
    if (is_playing && !is_muted) {
      audio_renderer_->SetAudioSilentMode(false);
      LOG(INFO) << "OHOSAudioOutputStream SetAudioSilentMode false!";
      isSilentMode_ = false;
      silentFrameNum_ = 0;
    } else {
      if (bytesSingle > 0) {
        silentFrameNum_++;
      }
    }
  }
}

bool OHOSAudioOutputStream::IsPreloadOrMutedMediaMode()
{
  if (!weakMediaSession_) {
    return false;
  }

  content::MediaSessionImpl::NWebMediaSessionState sessionState = weakMediaSession_.get()->GetSessionState();
  bool is_muted = weakMediaSession_.get()->GetMuteState();
  LOG(INFO) << "OHOSAudioOutputStream sessionState:" << static_cast<uint32_t>(sessionState)
      << ", mutedMode:" << is_muted;

  bool is_preload = false;
  if (sessionState == content::MediaSessionImpl::NWebMediaSessionState::NOINITIAL) {
    return false;
  } else if (sessionState == content::MediaSessionImpl::NWebMediaSessionState::NONEED) {
    is_preload = true;
  }
  return is_preload || is_muted;
}

void OHOSAudioOutputStream::SchedulePumpSamples(base::TimeTicks now) {
  // the audio syterm also have a schedule to read the data,
  // so we need to write it faster to solve underrun problem.
  auto maxSleepTime = base::Microseconds(10);
  auto sleepTime = GetCurrentStreamTime() - now;
  if (sleepTime > maxSleepTime) {
    sleepTime = maxSleepTime;
  }
  timer_.Start(FROM_HERE, sleepTime,
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

void OHOSAudioOutputStream::FlushData() {
  base::TimeTicks now = base::TimeTicks::Now();
  base::TimeDelta delay;
  uint64_t latency = 0;
  if (!audio_renderer_) {
    LOG(INFO) << "OHOSAudioOutputStream::FlushData !audio_renderer_";
    return;
  }
  audio_renderer_->GetLatency(latency);
  delay = base::Microseconds(latency);
  if (!callback_) {
    LOG(INFO) << "OHOSAudioOutputStream::FlushData !callback_";
    return;
  }
  (void)callback_->OnMoreData(delay, now, {}, audio_bus_.get());
}
}  // namespace media
