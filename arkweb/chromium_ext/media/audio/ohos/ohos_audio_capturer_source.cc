// Copyright (c) 2023 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ohos_audio_capturer_source.h"

#include <ctime>

#include "audio_capturer_options_adapter_impl.h"
#include "audio_dump.h"
#include "base/command_line.h"
#include "base/location.h"
#include "base/logging.h"
#include "base/task/single_thread_task_runner.h"
#include "base/task/task_runner.h"
#include "buffer_desc_adapter_impl.h"
#include "media/base/audio_glitch_info.h"
#include "media/base/audio_parameters.h"
#include "third_party/ohos_ndk/includes/ohos_adapter/ohos_adapter_helper.h"
#include "arkweb/build/features/features.h"

namespace media {

AudioCapturerReadCallback::AudioCapturerReadCallback(
    const OnReadDataCallback& readDataCallback)
    : readDataCallback_(readDataCallback) {}

AudioCapturerReadCallback::~AudioCapturerReadCallback() {}

void AudioCapturerReadCallback::OnReadData(size_t length) {
  if (!readDataCallback_.is_null()) {
    readDataCallback_.Run();
  }
}

OHOSAudioCapturerSource::OHOSAudioCapturerSource(
    scoped_refptr<base::SingleThreadTaskRunner> capturer_task_runner)
    : capturer_task_runner_(capturer_task_runner) {
  capturer_ = OhosAdapterHelper::GetInstance().CreateAudioCapturerAdapter();
  DCHECK(capturer_);
}

OHOSAudioCapturerSource::~OHOSAudioCapturerSource() {
  LOG(INFO) << "OHOSAudioCapturerSource::~OHOSAudioCapturerSource";
  weak_factory_.InvalidateWeakPtrs();
}

void OHOSAudioCapturerSource::Initialize(
    const AudioParameters& params,
    AudioCapturerSource::CaptureCallback* callback) {
  LOG(INFO) << "OHOSAudioCapturerSource::Initialize";
  DCHECK(!callback_);
  DCHECK(callback);
  main_task_runner_ = base::SingleThreadTaskRunner::GetCurrentDefault();
  params_ = params;
  callback_ = callback;

  if (!capturer_) {
    return;
  }
  if (params_.format() != AudioParameters::AUDIO_PCM_LOW_LATENCY) {
    ReportError("Only AUDIO_PCM_LOW_LATENCY format is supported");
    return;
  }

  DCHECK(capturer_task_runner_->BelongsToCurrentThread());

  std::shared_ptr<OHOS::NWeb::AudioCapturerOptionsAdapterImpl> capturerOptions =
      std::make_shared<OHOS::NWeb::AudioCapturerOptionsAdapterImpl>();
  capturerOptions->SetSamplingRate(
      static_cast<AudioAdapterSamplingRate>(params_.sample_rate()));
  capturerOptions->SetEncoding(AudioAdapterEncodingType::ENCODING_PCM);
  capturerOptions->SetSampleFormat(AudioAdapterSampleFormat::SAMPLE_S16LE);
  capturerOptions->SetChannels(
      static_cast<AudioAdapterChannel>(params_.channels()));
  OHOS::NWeb::AudioAdapterSourceType source_type;
  if (params_.effects() & media::AudioParameters::ECHO_CANCELLER) {
    source_type = OHOS::NWeb::AudioAdapterSourceType::SOURCE_TYPE_VOICE_COMMUNICATION;
  } else {
    LOG(INFO) << "[webrtc_logging]OHOSAudioCapturerSource::Initialize, effects disable ECHO_CANCELLER";
    source_type = OHOS::NWeb::AudioAdapterSourceType::SOURCE_TYPE_MIC;
  }
  capturerOptions->SetSourceType(source_type);
  capturerOptions->SetCapturerFlags(0);

  capturer_->Create(capturerOptions);
  capturer_->GetFrameCount(frameCount_);
  time_t now = time(nullptr);
  std::string dumpFileName = std::to_string(now) + "_" +
                             std::to_string(params_.sample_rate()) + "_" +
                             std::to_string(params_.channels()) + "_" +
                             std::to_string(1) + "_capturer_in.pcm";
  FILE* tempFile = dumpFile_.get();
  DumpFileUtil::OpenDumpFile(dumpFileName, &tempFile);
  dumpFile_ = tempFile;
}

void OHOSAudioCapturerSource::Start() {
  LOG(INFO) << "OHOSAudioCapturerSource::Start";
  DCHECK(callback_);
  DCHECK(capturer_task_runner_->BelongsToCurrentThread());
  audioCapturerReadCallback_ =
      std::make_shared<AudioCapturerReadCallback>(base::BindRepeating(
          &OHOSAudioCapturerSource::ReadData, weak_factory_.GetWeakPtr()));
  if (!capturer_ || !audioCapturerReadCallback_) {
    return;
  }
  capturer_->SetCapturerReadCallback(audioCapturerReadCallback_);
  if (!capturer_->Start()) {
    LOG(ERROR) << "OHOSAudioCapturerSource::Start start failed";
    ReportError("Start OHOS audio capturer failed");
    return;
  }
  main_task_runner_->PostTask(
      FROM_HERE, base::BindOnce(&OHOSAudioCapturerSource::NotifyCaptureStarted,
                                weak_factory_.GetWeakPtr()));
}

// LCOV_EXCL_START
void OHOSAudioCapturerSource::Stop() {
  LOG(INFO) << "OHOSAudioCapturerSource::Stop";
  {
    base::AutoLock lock(callback_lock_);
    if (callback_) {
      callback_ = nullptr;
    }
    is_stopped_.store(true);
  }
  if (!capturer_) {
    return;
  }

  DCHECK(capturer_task_runner_->BelongsToCurrentThread());
  if (!capturer_->Stop()) {
    LOG(ERROR) << "OHOSAudioCapturerSource::Stop stop failed";
    ReportError("Stop OHOS audio capturer failed");
  }
  FILE* tempFile = dumpFile_.get();
  DumpFileUtil::CloseDumpFile(&tempFile);
  dumpFile_ = tempFile;
}

void OHOSAudioCapturerSource::ReadData() {
  if (!capturer_) {
    return;
  }
  if (is_stopped_.load()) {
    LOG(INFO) << "OHOSAudioCapturerSource::ReadData, has been set to stop";
    return;
  }
  std::shared_ptr<OHOS::NWeb::BufferDescAdapterImpl> bufferDesc =
      std::make_shared<OHOS::NWeb::BufferDescAdapterImpl>();

  bufferDesc->SetBufLength(0);
  capturer_->GetBufferDesc(bufferDesc);
  if (static_cast<int>(frameCount_) > 2 * params_.sample_rate() / 100) {
    LOG(ERROR) << "audioBus cannot handle input audio data more than 20ms. "
                  "frameCount: "
               << static_cast<int>(frameCount_);
    capturer_->Enqueue(bufferDesc);
    return;
  }
  base::TimeTicks timeStamp =
      base::TimeTicks() + base::Nanoseconds(capturer_->GetAudioTime());
  auto audio_bus = AudioBus::Create(params_.channels(), frameCount_);
  audio_bus->FromInterleaved<SignedInt16SampleTypeTraits>(
      reinterpret_cast<const int16_t*>(bufferDesc->GetBuffer()),
      static_cast<int>(frameCount_));
  {
    base::AutoLock lock(callback_lock_);
    if (callback_) {
      callback_->Capture(audio_bus.get(), timeStamp, {}, 1.0, false);
      DumpFileUtil::WriteDumpFile(dumpFile_, bufferDesc->GetBuffer(),
                                  bufferDesc->GetBufLength());
    }
  }
  capturer_->Enqueue(bufferDesc);
}
// LCOV_EXCL_STOP

void OHOSAudioCapturerSource::SetVolume(double volume) {
#if BUILDFLAG(ARKWEB_TEST)
  return;
#endif  
  NOTREACHED();
}

void OHOSAudioCapturerSource::SetAutomaticGainControl(bool enable) {
#if BUILDFLAG(ARKWEB_TEST)
  return;
#endif
  NOTREACHED();
}

void OHOSAudioCapturerSource::SetOutputDeviceForAec(
    const std::string& output_device_id) {
  return;
}

void OHOSAudioCapturerSource::NotifyCaptureError(const std::string& message) {
  DCHECK(main_task_runner_->BelongsToCurrentThread());
  if (!callback_) {
    return;
  }
  callback_->OnCaptureError(AudioCapturerSource::ErrorCode::kUnknown, message);
}

// LCOV_EXCL_START
void OHOSAudioCapturerSource::NotifyCaptureStarted() {
  DCHECK(main_task_runner_->BelongsToCurrentThread());
  if (callback_) {
    callback_->OnCaptureStarted();
  }
}
// LCOV_EXCL_STOP

void OHOSAudioCapturerSource::ReportError(const std::string& message) {
  DCHECK(capturer_task_runner_->BelongsToCurrentThread());
  LOG(INFO) << "OHOSAudioCapturerSource::ReportError";
  main_task_runner_->PostTask(
      FROM_HERE, base::BindOnce(&OHOSAudioCapturerSource::NotifyCaptureError,
                                weak_factory_.GetWeakPtr(), message));
}
}  // namespace media
