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

#ifndef OHOS_ADAPTER_SPEECH_TEXT_TO_SPEECH_ADAPTER_H
#define OHOS_ADAPTER_SPEECH_TEXT_TO_SPEECH_ADAPTER_H

#include <cstdint>
#include <memory>
#include <string>

#include "ohos/adapter/aki_hook/aki_hook.h"
#include "ohos/adapter/common/logging.h"
#include "ohos/adapter/export.h"

namespace ohos::adapter::textToSpeech {

enum OhosTtsEvent {
  TTS_ON_START = 0,
  TTS_ON_END = 1,
  TTS_ON_WORD = 2,
  TTS_ON_ERROR = 3,
  TTS_ON_CANCELLED = 4,
  TTS_ON_INTERRUPTED = 5,
  TTS_ON_PAUSE = 6,
  TTS_ON_RESUME = 7
};

struct VoiceInfo {
  std::string language;
  int person;
  std::string style;
  // GA:available / EOM:discard
  std::string status;
  std::string gender;
  std::string description;
};

using OhOsVoices = std::vector<VoiceInfo>;

struct EngineCreationParamsExtraParams {
  std::string style;
  std::string locate;
  std::string name;
};

struct EngineCreationParams {
  std::string language;
  int online;
  int person;
  EngineCreationParamsExtraParams extraParams;
};

struct SpeakingParamsExtraParams {
  double speed;
  double volume;
  double pitch;
  std::string languageContext;
  std::string audioType;
  int playType;
  int soundChannel;
  int queueMode;
};

struct SpeakingParams {
  std::string requestId;
  SpeakingParamsExtraParams extraParams;
};

class ADAPTER_EXPORT_API TextToSpeechAdapter {
 public:
  static TextToSpeechAdapter& GetInstance();
  TextToSpeechAdapter(const TextToSpeechAdapter&) = delete;
  TextToSpeechAdapter(TextToSpeechAdapter&&) = delete;
  TextToSpeechAdapter& operator=(const TextToSpeechAdapter&) = delete;
  virtual ~TextToSpeechAdapter() = default;

  TextToSpeechAdapter();
  bool InitializeTextToSpeechEngine(const EngineCreationParams& params);
  OhOsVoices TextToSpeechEngineListVoices();

  bool TextToSpeechlSupported();
  bool TextToSpeechInitialized();

  bool Speak(const std::string& parsed_utterance, const SpeakingParams& params);
  bool IsSpeaking();
  bool StopSpeaking();
  bool Shutdown();

  // OH is not supported pause and resume.
  // The current solution is to obtain audio streams using the onData method.
  void Pause();
  void Resume();

  void TextToSpeechEngineGetArrayBuffer();
  void TextToSpeechEngineOnError();

  void RegisterCallback(std::function<void()>, OhosTtsEvent type);

 private:
  bool GetAkiBooleanResult(const std::string& func_name,
                           const EngineCreationParams& extra_params);
  OhOsVoices GetAkiOhOsVoicesArrayResult(const std::string& func_name);

  bool is_supported_ = false;
  bool is_initialized_ = false;

  OhOsVoices voices_;

  std::function<void()> start_event_callback_;
  std::function<void()> end_event_callback_;
};

}  // namespace ohos::adapter::textToSpeech

#endif  // OHOS_ADAPTER_SPEECH_TEXT_TO_SPEECH_ADAPTER_H
