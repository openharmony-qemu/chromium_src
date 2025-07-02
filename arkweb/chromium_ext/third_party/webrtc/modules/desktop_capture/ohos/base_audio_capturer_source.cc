/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "third_party/webrtc/modules/desktop_capture/ohos/base_audio_capturer_source.h"

#include <ctime>

#include "base/command_line.h"
#include "base/location.h"
#include "base/logging.h"
#include "base/task/task_runner.h"
#include "media/base/audio_parameters.h"
#include "media/audio/ohos/audio_dump.h"
#include "media/base/audio_bus.h"
#include "media/base/audio_glitch_info.h"
#include "third_party/webrtc/modules/desktop_capture/ohos/audio_buffer_adapter_impl.h"
#include "third_party/webrtc/modules/desktop_capture/ohos/audio_capture_info_adapter_impl.h"
#include "third_party/webrtc/modules/desktop_capture/ohos/audio_enc_info_adapter_impl.h"
#include "third_party/webrtc/modules/desktop_capture/ohos/audio_info_adapter_impl.h"
#include "third_party/webrtc/modules/desktop_capture/ohos/recorder_info_adapter_impl.h"
#include "third_party/webrtc/modules/desktop_capture/ohos/screen_capture_config_adapter_impl.h"

namespace webrtc {

namespace {
constexpr int32_t kInnerAudioSampleRate = 48000;
constexpr int32_t kInnerAudioFrameCount = 960;
}

AudioScreenCapturerReadCallback::AudioScreenCapturerReadCallback(
    const OnAudioReadDataCallback& readDataCallback)
    : readDataCallback_(readDataCallback) {}

AudioScreenCapturerReadCallback::~AudioScreenCapturerReadCallback() {}

#if BUILDFLAG(ARKWEB_ARKWEB_EXTENSIONS)
void AudioScreenCapturerReadCallback::OnDisplaySelected(uint64_t displayId) {}
#endif

void AudioScreenCapturerReadCallback::OnReadData(OHOS::NWeb::AudioCaptureSourceTypeAdapter type) {
  if (!readDataCallback_.is_null()) {
    readDataCallback_.Run(type);
  }
}

BaseAudioCapturerSource::BaseAudioCapturerSource(
    scoped_refptr<base::SingleThreadTaskRunner> capturer_task_runner,
    int nwebId)
    : capturer_task_runner_(capturer_task_runner) {  // 初始化新成员变量
  nwebId_ = nwebId;
  LOG(INFO) << "BaseAudioCapturerSource::BaseAudioCapturerSource, screen capture adapter init";
}

BaseAudioCapturerSource::~BaseAudioCapturerSource() {
  LOG(INFO) << "BaseAudioCapturerSource::~BaseAudioCapturerSource";
}

void BaseAudioCapturerSource::SetScreenCaptureState(const OHOS::NWeb::ScreenCaptureStateCodeAdapter& stateCode) {
    capture_state_code_ = stateCode;
}

void BaseAudioCapturerSource::Initialize(
    const AudioParameters& params,
    AudioCapturerSource::CaptureCallback* callback) {
  portal_init_failed_ = true;
  LOG(INFO) << "BaseAudioCapturerSource::Initialize";
  DCHECK(!base_callback_);
  DCHECK(callback);
  main_task_runner_ = base::SingleThreadTaskRunner::GetCurrentDefault();
  params_ = params;
  base_callback_ = callback;
  if (params_.format() != AudioParameters::AUDIO_PCM_LOW_LATENCY) {
    LOG(ERROR) << "BaseAudioCapturerSource::Initialize fail";
    ReportError("Only AUDIO_PCM_LOW_LATENCY format is supported");
    return;
  }

  time_t now = time(nullptr);
  std::string dumpFileName = std::to_string(now) + "_" +
      std::to_string(kInnerAudioSampleRate) + "_" +
      std::to_string(params_.channels()) + "_" +
      std::to_string(1) + "_webrtc_capturer_in.pcm";
  DumpFileUtil::OpenDumpFile(dumpFileName, &dumpFile_);

  DCHECK(capturer_task_runner_->BelongsToCurrentThread());

  LOG(INFO) << "BaseAudioCapturerSource init success";
  portal_init_failed_ = false;
}

void BaseAudioCapturerSource::Start() {
  LOG(INFO) << "BaseAudioCapturerSource::Start";
  DCHECK(base_callback_);
  DCHECK(capturer_task_runner_->BelongsToCurrentThread());

  if (portal_init_failed_) {
    LOG(ERROR) << "init failed";
    return;
  }
  if (isStart_) {
    LOG(INFO) << "already started, no need to start again";
    return;
  }

  LOG(INFO) << "BaseAudioCapturerSource, CreateBaseScreenCaptureSource: "
            << &BaseScreenCaptureSource::GetInstance();
  AudioScreenCapturerReadCallback_ =
      std::make_shared<AudioScreenCapturerReadCallback>(base::BindRepeating(
          &BaseAudioCapturerSource::HandleAudioBuffer, weak_factory_.GetWeakPtr()));
  if (!AudioScreenCapturerReadCallback_) {
    LOG(ERROR) << "audio screen capturer read callback is nullptr";
    return;
  }
  BaseScreenCaptureSource::GetInstance().RegisterAudioCaptureCallback(AudioScreenCapturerReadCallback_, nwebId_);

  isStart_ = true;

  LOG(INFO) << "BaseAudioCapturerSource Start success";
  main_task_runner_->PostTask(
      FROM_HERE, base::BindOnce(&BaseAudioCapturerSource::NotifyCaptureStarted,
                                weak_factory_.GetWeakPtr()));
}

void BaseAudioCapturerSource::Stop() {
  LOG(INFO) << "BaseAudioCapturerSource::Stop";
  {
    base::AutoLock lock(base_callback_lock_);
    if (base_callback_) {
      base_callback_ = nullptr;
    }
  }

  DCHECK(capturer_task_runner_->BelongsToCurrentThread());

  DumpFileUtil::CloseDumpFile(&dumpFile_);
}

void BaseAudioCapturerSource::HandleAudioBuffer(OHOS::NWeb::AudioCaptureSourceTypeAdapter type) {
  base::AutoLock lock(base_callback_lock_);

  if (portal_init_failed_) {
    LOG(ERROR) << "init failed";
    return;
  }

  std::shared_ptr<OHOS::NWeb::AudioBufferAdapterImpl> audiobuffer =
        std::make_shared<OHOS::NWeb::AudioBufferAdapterImpl>();
  if (!BaseScreenCaptureSource::GetInstance().ScreenCaptureAdapterIsExist(nwebId_) ||
    BaseScreenCaptureSource::GetInstance().screen_capture_adapter_map_[nwebId_] == nullptr) {
    LOG(ERROR) << "screencap adapter not exist";
    return;
  }
  int ret = BaseScreenCaptureSource::GetInstance().screen_capture_adapter_map_[nwebId_]->
    AcquireAudioBuffer(audiobuffer, type);
  if (ret != 0) {
    LOG(ERROR) << "acquire audio buffer failed";
    return;
  }

  OHOS::NWeb::AudioCaptureSourceTypeAdapter sourcetype = audiobuffer->GetSourcetype();
  if (sourcetype != type) {
    LOG(ERROR) << "buffer type error, sourcetype: " << (int32_t)sourcetype;
    BaseScreenCaptureSource::GetInstance().screen_capture_adapter_map_[nwebId_]->ReleaseAudioBuffer(sourcetype);
    return;
  }

  base::TimeTicks timeStamp =
      base::TimeTicks() + base::Nanoseconds(audiobuffer->GetTimestamp());
  auto audio_bus = media::AudioBus::Create(params_.channels(), kInnerAudioFrameCount);
  audio_bus->FromInterleaved<media::SignedInt16SampleTypeTraits>(
      reinterpret_cast<const int16_t*>(audiobuffer->GetBuffer()),
      static_cast<int>(kInnerAudioFrameCount));
  if (base_callback_) {
    base_callback_->Capture(audio_bus.get(), timeStamp, {}, 1.0, false);
    DumpFileUtil::WriteDumpFile(dumpFile_, audiobuffer->GetBuffer(), audiobuffer->GetLength());
  }

  BaseScreenCaptureSource::GetInstance().screen_capture_adapter_map_[nwebId_]->ReleaseAudioBuffer(type);
}

void BaseAudioCapturerSource::SetVolume(double volume) {
  return;
}

void BaseAudioCapturerSource::SetAutomaticGainControl(bool enable) {
  NOTREACHED();
}

void BaseAudioCapturerSource::SetOutputDeviceForAec(
    const std::string& output_device_id) {
  return;
}

void BaseAudioCapturerSource::NotifyCaptureError(const std::string& message) {
  DCHECK(main_task_runner_->BelongsToCurrentThread());
  if (!base_callback_) {
    return;
  }
  base_callback_->OnCaptureError(AudioCapturerSource::ErrorCode::kUnknown, message);
}

void BaseAudioCapturerSource::NotifyCaptureStarted() {
  DCHECK(main_task_runner_->BelongsToCurrentThread());
  if (base_callback_) {
    base_callback_->OnCaptureStarted();
  }
}

void BaseAudioCapturerSource::ReportError(const std::string& message) {
  DCHECK(capturer_task_runner_->BelongsToCurrentThread());
  LOG(INFO) << "BaseAudioCapturerSource::ReportError";
  main_task_runner_->PostTask(
      FROM_HERE, base::BindOnce(&BaseAudioCapturerSource::NotifyCaptureError,
                                weak_factory_.GetWeakPtr(), message));
}
}  // namespace media