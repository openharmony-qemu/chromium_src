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

#ifndef MEDIA_AUDIO_OHOS_AUDIO_INPUT_STREAM_H_
#define MEDIA_AUDIO_OHOS_AUDIO_INPUT_STREAM_H_

#include "base/feature_list.h"
#include "base/logging.h"
#include "base/timer/timer.h"
#include "media/audio/audio_io.h"
#include "media/audio/ohos/ohos_audio_capturer_source.h"
#include "media/audio/ohos/ohos_audio_manager.h"
#include "media/base/audio_parameters.h"

namespace media {

class OHOSAudioManager;

class OHOSAudioInputStream : public AudioInputStream {
 public:
  // Caller must ensure that manager outlives the stream.
  OHOSAudioInputStream(OHOSAudioManager* manager,
                       const AudioParameters& parameters);

  OpenOutcome Open() override;
  void Start(AudioInputCallback* callback) override;
  void Stop() override;
  void Close() override;
  double GetMaxVolume() override;
  void SetVolume(double volume) override;
  double GetVolume() override;
  bool SetAutomaticGainControl(bool enabled) override;
  bool GetAutomaticGainControl() override;
  bool IsMuted() override;
  void SetOutputDeviceForAec(const std::string& output_device_id) override;

 private:
  class CaptureCallbackAdapter;

  ~OHOSAudioInputStream() override;

  OHOSAudioManager* const manager_;

  AudioParameters parameters_;

  std::unique_ptr<CaptureCallbackAdapter> callback_adapter_;

  scoped_refptr<OHOSAudioCapturerSource> capturer_source_;

  double volume_ = 1.0;

  bool automatic_gain_control_ = false;
};

}  // namespace media

#endif  // MEDIA_AUDIO_OHOS_AUDIO_INPUT_STREAM_H_
