// Copyright (c) 2023 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "media/audio/ohos/ohos_audio_capturer_source.h"

#include "base/command_line.h"
#include "base/location.h"
#include "base/logging.h"
#include "base/task/task_runner.h"
#include "base/threading/thread_task_runner_handle.h"
#include "chrome/common/chrome_switches.h"
#include "media/base/audio_parameters.h"
#include "ohos_adapter_helper.h"

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

OHOSAudioCapturerSource::~OHOSAudioCapturerSource() {}

void OHOSAudioCapturerSource::Initialize(
    const AudioParameters& params,
    AudioCapturerSource::CaptureCallback* callback) {
  LOG(INFO) << "OHOSAudioCapturerSource::Initialize";
  DCHECK(!callback_);
  DCHECK(callback);
  main_task_runner_ = base::ThreadTaskRunnerHandle::Get();
  params_ = params;
  callback_ = callback;

  if (params_.format() != AudioParameters::AUDIO_PCM_LOW_LATENCY) {
    ReportError("Only AUDIO_PCM_LOW_LATENCY format is supported");
    return;
  }
  capturer_task_runner_->PostTask(
      FROM_HERE,
      base::BindOnce(&OHOSAudioCapturerSource::InitializeOnCapturerThread,
                     this));
}

void OHOSAudioCapturerSource::Start() {
  LOG(INFO) << "OHOSAudioCapturerSource::Start";
  DCHECK(main_task_runner_->BelongsToCurrentThread());
  DCHECK(callback_);

  capturer_task_runner_->PostTask(
      FROM_HERE,
      base::BindOnce(&OHOSAudioCapturerSource::StartOnCapturerThread, this));
}

void OHOSAudioCapturerSource::Stop() {
  LOG(INFO) << "OHOSAudioCapturerSource::Stop";
  if (!main_task_runner_)
    return;
  DCHECK(main_task_runner_->BelongsToCurrentThread());

  {
    base::AutoLock lock(callback_lock_);
    if (callback_) {
      callback_ = nullptr;
    }
  }

  capturer_task_runner_->PostTask(
      FROM_HERE,
      base::BindOnce(&OHOSAudioCapturerSource::StopOnCapturerThread, this));
}

void OHOSAudioCapturerSource::InitializeOnCapturerThread() {
  DCHECK(capturer_task_runner_->BelongsToCurrentThread());
  AudioAdapterCapturerOptions capturerOptions;
  capturerOptions.samplingRate =
      static_cast<AudioAdapterSamplingRate>(params_.sample_rate());
  capturerOptions.encoding = AudioAdapterEncodingType::ENCODING_PCM;
  capturerOptions.format = AudioAdapterSampleFormat::SAMPLE_S16LE;
  capturerOptions.channels =
      static_cast<AudioAdapterChannel>(params_.channels());
  capturerOptions.sourceType =
      AudioAdapterSourceType::SOURCE_TYPE_VOICE_COMMUNICATION;
  capturerOptions.capturerFlags = 0;
  capturer_->Create(capturerOptions);
  capturer_->GetFrameCount(frameCount_);
}

void OHOSAudioCapturerSource::StartOnCapturerThread() {
  LOG(INFO) << "OHOSAudioCapturerSource::StartOnCapturerThread";
  DCHECK(capturer_task_runner_->BelongsToCurrentThread());
  audioCapturerReadCallback_ =
      std::make_shared<AudioCapturerReadCallback>(base::BindRepeating(
          &OHOSAudioCapturerSource::ReadData, base::Unretained(this)));
  if (!capturer_ || !audioCapturerReadCallback_) {
    return;
  }
  capturer_->SetCapturerReadCallback(audioCapturerReadCallback_);
  if (!capturer_->Start()) {
    LOG(ERROR) << "OHOSAudioCapturerSource::StartOnCapturerThread start failed";
    ReportError("Start OHOS audio capturer failed");
    return;
  }

  main_task_runner_->PostTask(
      FROM_HERE,
      base::BindOnce(&OHOSAudioCapturerSource::NotifyCaptureStarted, this));
}

void OHOSAudioCapturerSource::StopOnCapturerThread() {
  LOG(INFO) << "OHOSAudioCapturerSource::StopOnCapturerThread";
  DCHECK(main_task_runner_->BelongsToCurrentThread());
  if (!capturer_->Stop()) {
    LOG(ERROR) << "OHOSAudioCapturerSource::StopOnCapturerThread stop failed";
    ReportError("Stop OHOS audio capturer failed");
  }
}

void OHOSAudioCapturerSource::ReadData() {
  base::AutoLock lock(callback_lock_);
  BufferDescAdapter bufferDesc;
  bufferDesc.bufLength = 0;
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
      reinterpret_cast<const int16_t*>(bufferDesc.buffer),
      static_cast<int>(frameCount_));
  if (callback_) {
    callback_->Capture(audio_bus.get(), timeStamp, 1.0, false);
  }
  capturer_->Enqueue(bufferDesc);
}

void OHOSAudioCapturerSource::SetVolume(double volume) {
  NOTREACHED();
}

void OHOSAudioCapturerSource::SetAutomaticGainControl(bool enable) {
  NOTREACHED();
}

void OHOSAudioCapturerSource::SetOutputDeviceForAec(
    const std::string& output_device_id) {
  NOTREACHED();
}

void OHOSAudioCapturerSource::NotifyCaptureError(const std::string& message) {
  DCHECK(main_task_runner_->BelongsToCurrentThread());
  if (!callback_)
    return;
  callback_->OnCaptureError(AudioCapturerSource::ErrorCode::kUnknown, message);
}

void OHOSAudioCapturerSource::NotifyCaptureStarted() {
  DCHECK(main_task_runner_->BelongsToCurrentThread());
  if (callback_) {
    callback_->OnCaptureStarted();
  }
}

void OHOSAudioCapturerSource::ReportError(const std::string& message) {
  DCHECK(capturer_task_runner_->BelongsToCurrentThread());
  main_task_runner_->PostTask(
      FROM_HERE, base::BindOnce(&OHOSAudioCapturerSource::NotifyCaptureError,
                                this, message));
}
}  // namespace media
