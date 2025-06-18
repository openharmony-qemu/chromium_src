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

#ifndef MEDIA_AUDIO_OHOS_AUDIO_OUTPUT_STREAM_H_
#define MEDIA_AUDIO_OHOS_AUDIO_OUTPUT_STREAM_H_

#include "base/feature_list.h"
#include "base/logging.h"
#include "base/task/single_thread_task_runner.h"
#include "base/task/task_runner.h"
#include "base/timer/timer.h"
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

 private:
  ~OHOSAudioOutputStream() override;

  base::TimeDelta GetDelay(base::TimeTicks delay_timestamp);

  bool InitRender();

  bool StartRender();

  // It is recommended to avoid calling OH_AudioRenderer_GetTimestamp too frequently.
  // Once per minute is acceptable, and ideally no more than once every 200ms.
  // Frequent calls may cause power issues, so it's not necessary to query the timestamp
  // frequently as long as audio-video sync is maintained.
  base::TimeDelta GetDelayImprove(base::TimeTicks now);

  OHOSAudioManager* manager_;

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
  // The time point of the last hardware update latency.
  base::TimeTicks update_delay_ = base::TimeTicks();
  // The time interval between hardware update latencies.
  // ideally no more than once every 200ms.
  base::TimeDelta interval_ = base::Milliseconds(200);
};

}  // namespace media

#endif  // MEDIA_AUDIO_OHOS_AUDIO_OUTPUT_STREAM_H_
