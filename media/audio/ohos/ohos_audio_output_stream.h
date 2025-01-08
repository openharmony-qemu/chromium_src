// Copyright (c) 2022 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Based on audio_output_stream_fuchsia.h originally written by
// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_AUDIO_OHOS_AUDIO_OUTPUT_STREAM_H_
#define MEDIA_AUDIO_OHOS_AUDIO_OUTPUT_STREAM_H_

#include "audio_renderer_adapter.h"
#include "base/feature_list.h"
#include "base/logging.h"
#include "base/task/single_thread_task_runner.h"
#include "base/task/task_runner.h"
#include "base/timer/timer.h"
#include "content/browser/media/audio_output_stream_broker.h"
#include "content/browser/media/session/media_session_impl.h"
#include "content/public/browser/web_contents.h"
#include "media/audio/ohos/ohos_audio_manager.h"

namespace media {
using namespace OHOS::NWeb;

class OHOSAudioManager;
class OHOSAudioOutputStream;

class AudioRendererOptions : public AudioRendererOptionsAdapter {
 public:
  AudioRendererOptions() = default;

  AudioAdapterSamplingRate GetSamplingRate() override;

  AudioAdapterEncodingType GetEncodingType() override;

  AudioAdapterSampleFormat GetSampleFormat() override;

  AudioAdapterChannel GetChannel() override;

  AudioAdapterContentType GetContentType() override;

  AudioAdapterStreamUsage GetStreamUsage() override;

  int32_t GetRenderFlags() override;

  AudioAdapterConcurrencyMode GetConcurrencyMode() override;

 private:
  friend class OHOSAudioOutputStream;
  AudioAdapterSamplingRate rate_;
  AudioAdapterEncodingType encoding_;
  AudioAdapterSampleFormat format_;
  AudioAdapterChannel channels_;
  AudioAdapterContentType content_type_;
  AudioAdapterStreamUsage stream_usage_;
  int32_t renderer_flags_;
  AudioAdapterConcurrencyMode concurrency_mode_;
};

class AudioRendererCallback : public AudioRendererCallbackAdapter {
 public:
  AudioRendererCallback(content::MediaSessionImpl* media_session,
                        const scoped_refptr<base::SingleThreadTaskRunner>& task_runner);
  ~AudioRendererCallback();
  void OnSuspend() override;
  void OnResume() override;
  bool GetSuspendFlag();
  void SetSuspendFlag(bool flag);

 private:
  content::MediaSessionImpl* media_session_;
  scoped_refptr<base::SingleThreadTaskRunner> main_task_runner_ = nullptr;
  time_t intervalSinceLastSuspend_ = 0.0;
  bool suspendFlag_ = false;
};

class AudioOutputChangeCallback : public AudioOutputChangeCallbackAdapter {
 public:
  AudioOutputChangeCallback(
    const scoped_refptr<base::SingleThreadTaskRunner>& task_runner,
    AudioParameters params,
    bool isCommunication);
  ~AudioOutputChangeCallback();
  void OnOutputDeviceChange(int32_t reason) override;

 private:
  scoped_refptr<base::SingleThreadTaskRunner> main_task_runner_ = nullptr;
  AudioParameters params_;
  bool isCommunication_ = false;
};

class OHOSAudioOutputStream : public AudioOutputStream {
 public:
  static const int kMaxNumOfBuffersInQueue = 2;
  static std::set<content::WebContents*> webContentSet_;

  OHOSAudioOutputStream(const OHOSAudioOutputStream&) = delete;
  OHOSAudioOutputStream& operator=(const OHOSAudioOutputStream&) = delete;

  // Caller must ensure that manager outlives the stream.
  OHOSAudioOutputStream(OHOSAudioManager* manager,
                        const AudioParameters& parameters,
                        bool isCommunication);

  // AudioOutputStream interface.
  bool Open() override;
  void Start(AudioSourceCallback* callback) override;
  void Stop() override;
  void Flush() override;
  void SetVolume(double volume) override;
  void GetVolume(double* volume) override;
  void Close() override;
  bool GetInterruptMode();
  void SetInterruptMode(bool audioExclusive);
  bool GetAudioExclusive();
  void FlushData();

 private:
  ~OHOSAudioOutputStream() override;

  base::TimeTicks GetCurrentStreamTime();

  // Resets internal state and reports an error to |callback_|.
  void ReportError();

  // Requests data from AudioSourceCallback, passes it to the mixer and
  // schedules |timer_| for the next call.
  void PumpSamples();

  // Schedules |timer_| to call PumpSamples() when appropriate for the next
  // packet.
  void SchedulePumpSamples(base::TimeTicks now);

  // Called in Open();
  bool SetupAudioBuffer();

  // Called in Close();
  void ReleaseAudioBuffer();

  // Call to set audio_render silentMode
  void SetUpAudioSilentState();

  // Call to determine whether media is preload
  bool IsPreloadOrMutedMediaMode();

  bool InitRender(const std::shared_ptr<AudioRendererOptionsAdapter> options);

  bool StartRender();

  void Prepare(base::WeakPtr<content::MediaSessionImpl> weakMediaSession);

  raw_ptr<OHOSAudioManager> manager_;

  AudioParameters parameters_;

  // |audio_bus_| is used only in PumpSamples(). It is kept here to avoid
  // reallocating the memory every time.
  std::unique_ptr<AudioBus> audio_bus_;

  raw_ptr<AudioSourceCallback> callback_ = nullptr;

  double volume_ = 1.0;

  base::TimeTicks reference_time_;

  int64_t stream_position_samples_;

  // Timer that's scheduled to call PumpSamples().
  base::OneShotTimer timer_;

  uint8_t* audio_data_[kMaxNumOfBuffersInQueue];

  int active_buffer_index_;

  int bytes_per_frame_;

  size_t buffer_size_bytes_;

  SampleFormat sample_format_;

  std::unique_ptr<AudioRendererAdapter> audio_renderer_;

  content::WebContents* webContent_ = nullptr;

  base::WeakPtr<content::MediaSessionImpl> weakMediaSession_ = nullptr;

  std::shared_ptr<AudioRendererCallback> rendererCallback_ = nullptr;

  bool isCommunication_ = false;

  bool audioExclusive_ = false;

  bool isSuspended_ = false;

  bool isSilentMode_ = false;

  scoped_refptr<base::SingleThreadTaskRunner> main_task_runner_;

  std::shared_ptr<AudioOutputChangeCallback> outputChangeCallback_ = nullptr;
};

}  // namespace media

#endif  // MEDIA_AUDIO_OHOS_AUDIO_OUTPUT_STREAM_H_
