// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_AUDIO_OHOS_AUDIO_OUTPUT_STREAM_H_
#define MEDIA_AUDIO_OHOS_AUDIO_OUTPUT_STREAM_H_

#include "arkweb/ohos_adapter_ndk/ndk_callback_wrapper/callback_shared_wrapper.h"
#include "third_party/ohos_ndk/includes/ohos_adapter/audio_renderer_adapter.h"
#include "base/feature_list.h"
#include "base/logging.h"
#include "base/task/single_thread_task_runner.h"
#include "base/task/task_runner.h"
#include "base/timer/timer.h"
#include "base/files/scoped_file.h"
#include "media/audio/ohos/ohos_audio_manager.h"
#include "media/audio/ohos/ohos_audio_output_callback.h"
#include "ohaudio/native_audiorenderer.h"
#include "ohaudio/native_audiostreambuilder.h"

namespace content {
  class WebContents;
  class MediaSessionImpl;
}

namespace media {

using namespace OHOS::NWeb;

class OHOSAudioManager;
class OHOSAudioOutputCallback;

enum class AudioSessionType {AUTO = 0, AMBIENT = 3};

class OHOSAudioOutputStream : public AudioOutputStream {
 public:
  static std::vector<AudioParameters> audioParameterSet_;
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
  void Refresh();

  // Requests data from AudioSourceCallback
  void OnWriteData(void* buffer, int32_t length);

  void PumpSamples();
  // Resets internal state and reports an error to |callback_|.
  void ReportError();

  void OnSuspend();

  void OnResume();

  void OldDeviceUnavailable();
  
  bool isNeedResume(int32_t resumeInterval);

  void SchedulePumpSamples();

  // Call to set audio_render silentMode
  void SetUpAudioSilentState();

  static int32_t AudioRendererOnWriteData(OH_AudioRenderer* renderer,
                                          void* userData,
                                          void* buffer,
                                          int32_t length);

  static int32_t AudioRendererOnError(OH_AudioRenderer* renderer,
                                      void* userData,
                                      OH_AudioStream_Result error);

  static int32_t AudioRendererOnInterruptEvent(OH_AudioRenderer* renderer,
                                              void* userData,
                                              OH_AudioInterrupt_ForceType type,
                                              OH_AudioInterrupt_Hint hint);

  static void AudioRendererOutputDeviceChangeCallback(OH_AudioRenderer* renderer,
                                                      void* userData,
                                                      OH_AudioStream_DeviceChangeReason reason);
#if BUILDFLAG(ARKWEB_TEST)
 public:
#else
 private:
#endif
  ~OHOSAudioOutputStream() override;

  base::TimeDelta GetDelay(base::TimeTicks delay_timestamp);

  bool InitRender();

  bool StartRender();

  void Prepare(const AudioParameters& parameters);

  void SuspendOtherMediaSession();

  // Call to determine whether media is preload
  bool IsPreloadOrMutedMediaMode();

  void SetStreamUsage();

  void OneShotMediaPlayerStopped(AudioParameters parameters);

  void StopTimer();

  raw_ptr<OHOSAudioManager> manager_ = nullptr;

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

  raw_ptr<OH_AudioRenderer> audio_renderer_ = nullptr;

  raw_ptr<OH_AudioStreamBuilder> audio_stream_builder_ = nullptr;

  bool isCommunication_ = false;

  base::Lock lock_;
  // Timer that's scheduled to call PumpSamples().
  base::OneShotTimer timer_;

  bool isSuspended_ = false;

  bool running_ = false;

  time_t intervalSinceLastSuspend_ = 0.0;

  base::TimeDelta time_per_buffer_ = base::Microseconds(0);

  scoped_refptr<base::SingleThreadTaskRunner> main_task_runner_;

  scoped_refptr<base::SingleThreadTaskRunner> audio_task_runner_;

  bool audioExclusive_ = false;

  bool isSilentMode_ = false;

  std::atomic<bool> isDestroyed_ = {false};

  int audioResumeInterval_ = 0;

  int write_data_counts_ = 0;

  base::ScopedFILE dumpFile_ = nullptr;

  size_t callback_index_ = 0;

  static CallbackSharedWrapper<OHOSAudioOutputCallback> callback_wrapper_;

  base::WeakPtrFactory<OHOSAudioOutputStream> weak_factory_{this};
};

}  // namespace media

#endif  // MEDIA_AUDIO_OHOS_AUDIO_OUTPUT_STREAM_H_
