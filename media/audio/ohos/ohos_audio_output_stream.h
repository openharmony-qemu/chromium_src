// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_AUDIO_OHOS_AUDIO_OUTPUT_STREAM_H_
#define MEDIA_AUDIO_OHOS_AUDIO_OUTPUT_STREAM_H_

#include "base/feature_list.h"
#include "base/logging.h"
#include "base/task/single_thread_task_runner.h"
#include "base/task/task_runner.h"
#include "base/timer/timer.h"
#include "content/public/browser/media_session.h"
#include "media/audio/ohos/ohos_audio_manager.h"
#include "ohaudio/native_audiorenderer.h"
#include "ohaudio/native_audiostreambuilder.h"

namespace media {

class OHOSAudioManager;

class OHOSAudioOutputStream : public AudioOutputStream {
 public:
  OHOSAudioOutputStream(const OHOSAudioOutputStream&) = delete;
  OHOSAudioOutputStream& operator=(const OHOSAudioOutputStream&) = delete;

  // Caller must ensure that manager outlives the stream.
  OHOSAudioOutputStream(OHOSAudioManager* manager,
                        const AudioParameters& parameters);

  // AudioOutputStream interface.
  bool Open() override;
  void Start(AudioSourceCallback* callback) override;
  void Stop() override;
  void Flush() override;
  void SetVolume(double volume) override;
  void GetVolume(double* volume) override;
  void Close() override;
  void SetInterruptMode(bool audioExclusive);
  void Refresh();

  // Requests data from AudioSourceCallback
  void PumpSamples(void* buffer, int32_t length);

  // Resets internal state and reports an error to |callback_|.
  void ReportError();

  void OnSuspend();

  void SuspendPlayer();

  void OnResume();

  void IdlePumpSamples();

  void ScheduleIdlePumpSamples();

 private:
  ~OHOSAudioOutputStream() override;

  base::TimeDelta GetDelay(base::TimeTicks delay_timestamp);

  bool InitRender();

  void ReleaseRender();

  bool StartRender();

  void FlushData();

  OH_AudioStream_State GetRenderState();

  // It is recommended to avoid calling OH_AudioRenderer_GetTimestamp too frequently.
  // Once per minute is acceptable, and ideally no more than once every 200ms.
  // Frequent calls may cause power issues, so it's not necessary to query the timestamp
  // frequently as long as audio-video sync is maintained.
  base::TimeDelta GetDelayImprove(base::TimeTicks now);

  raw_ptr<OHOSAudioManager> manager_;

  AudioParameters parameters_;

  // Constant used for calculating latency. Amount of nanoseconds per frame.
  const double ns_per_frame_;

  // |audio_bus_| is used only in PumpSamples(). It is kept here to avoid
  // reallocating the memory every time.
  std::unique_ptr<AudioBus> audio_bus_;

  raw_ptr<AudioSourceCallback> callback_ = nullptr;

  double volume_ = 1.0;

  base::TimeTicks reference_time_;

  int bytes_per_frame_;

  size_t buffer_size_bytes_;

  SampleFormat sample_format_;

  RAW_PTR_EXCLUSION OH_AudioRenderer* audio_renderer_ = nullptr;

  RAW_PTR_EXCLUSION OH_AudioStreamBuilder* audio_stream_builder_ = nullptr;

  scoped_refptr<base::SingleThreadTaskRunner> main_task_runner_;

  // hardware latency
  base::TimeDelta delay_ = base::Milliseconds(0);

  // The audio stream can be controlled when it has a media session and is not a
  // short audio. This is set to false by default.
  bool is_session_controllable_ = false;

  // The time point of the last hardware update latency.
  base::TimeTicks update_delay_ = base::TimeTicks();
  // The time interval between hardware update latencies.
  // ideally no more than once every 200ms.
  base::TimeDelta interval_ = base::Milliseconds(200);

  base::TimeDelta time_per_buffer_ = base::Microseconds(0);

  bool is_suspended_ = false;

  base::Lock stream_lock_;

  base::OneShotTimer timer_;

  base::WeakPtr<content::MediaSession> weak_media_session_ = nullptr;

  base::WeakPtrFactory<OHOSAudioOutputStream> weak_factory_{this};
};

}  // namespace media

#endif  // MEDIA_AUDIO_OHOS_AUDIO_OUTPUT_STREAM_H_
