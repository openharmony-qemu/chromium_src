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

#include "base/functional/callback.h"
#include "base/logging.h"
#include "base/no_destructor.h"
#include "base/uuid.h"
#include "content/browser/speech/tts_platform_impl.h"
#include "content/public/browser/browser_thread.h"
#include "ohos/adapter/speech/text_to_speech_adapter.h"
namespace content {

namespace {
constexpr int K_INVALID_UTTERANCE_ID = -1;
constexpr int K_DEFAULT_PITCH = 1;
constexpr int K_DEFAULT_RATE = 1;
constexpr int K_DEFAULT_PLAY_TYPE = 1;
constexpr int K_DEFAULT_SOUND_CHANNEL = 3;
constexpr int K_DEFAULT_QUEUE_MODE = 0;
constexpr int K_DEFAULT_BASE = 10;
constexpr double K_DEFAULT_FACTOR = 0.5;
const std::string K_TTS_SPEAK_REQUEST_ID_PREFIX = "OHOSTTS";
}  // namespace

// Dummy implementation to prevent a browser crash
class TtsPlatformImplOHOS : public TtsPlatformImpl {
 public:
  TtsPlatformImplOHOS(const TtsPlatformImplOHOS&) = delete;
  TtsPlatformImplOHOS& operator=(const TtsPlatformImplOHOS&) = delete;

  // TtsPlatform implementation.
  bool PlatformImplSupported() override;
  bool PlatformImplInitialized() override;

  void InitializeVoices();
  void GetVoices(std::vector<VoiceData>* out_voices) override;

  void Speak(int utterance_id,
             const std::string& utterance,
             const std::string& lang,
             const VoiceData& voice,
             const UtteranceContinuousParameters& params,
             base::OnceCallback<void(bool)> on_speak_finished) override;
  bool StopSpeaking() override;
  bool IsSpeaking() override;
  void Pause() override;
  void Resume() override {}
  void Shutdown() override;

  void OnSpeechEndEvent();
  void OnSpeechStartEvent();

  // Get the single instance of this class.
  static TtsPlatformImplOHOS* GetInstance() {
    static base::NoDestructor<TtsPlatformImplOHOS> tts_platform;
    return tts_platform.get();
  }

 private:
  friend base::NoDestructor<TtsPlatformImplOHOS>;
  TtsPlatformImplOHOS();

  ohos::adapter::textToSpeech::EngineCreationParams CreateEngineParams();
  void RegisterTtsEvent();
  void ProcessSpeech(int utterance_id,
                     const std::string& lang,
                     const VoiceData& voice,
                     const UtteranceContinuousParameters& params,
                     base::OnceCallback<void(bool)> on_speak_finished,
                     const std::string& parsed_utterance);

  std::string GenerateIdSuffix();

  // Holds the platform state.
  bool is_supported_ = false;
  bool is_initialized_ = false;
  bool paused_ = false;

  ohos::adapter::textToSpeech::OhOsVoices voices_;

  // The current utterance being spoke.
  int utterance_id_ = K_INVALID_UTTERANCE_ID;
  std::string utterance_;
  size_t utterance_length_ = 0;
};

// static
TtsPlatformImpl* TtsPlatformImpl::GetInstance() {
  return TtsPlatformImplOHOS::GetInstance();
}

TtsPlatformImplOHOS::TtsPlatformImplOHOS() {
  TtsPlatformImplOHOS::RegisterTtsEvent();

  ohos::adapter::textToSpeech::EngineCreationParams params =
      TtsPlatformImplOHOS::CreateEngineParams();

  bool succeeded =
      ohos::adapter::textToSpeech::TextToSpeechAdapter::GetInstance()
          .InitializeTextToSpeechEngine(params);
  if (succeeded) {
    is_initialized_ = true;
    is_supported_ = true;
    LOG(INFO) << "TTS TtsPlatformImplOHOS engine initialized "
                 "successfully";
    TtsPlatformImplOHOS::InitializeVoices();
    return;
  }

  LOG(ERROR) << "TTS TtsPlatformImplOHOS engine initialized unsuccessfully";
}

std::string TtsPlatformImplOHOS::GenerateIdSuffix() {
  return base::Uuid::GenerateRandomV4().AsLowercaseString();
}

ohos::adapter::textToSpeech::EngineCreationParams
TtsPlatformImplOHOS::CreateEngineParams() {
  // Creating a TTS engine requires a group of parameters with the specified
  // language as the input parameter. Currently, the TTS engine supports only
  // Chinese.
  ohos::adapter::textToSpeech::EngineCreationParamsExtraParams extra_params;
  extra_params.style = "interaction-broadcast";
  extra_params.locate = "CN";
  extra_params.name = "OHOS-zh-CN-engine";

  ohos::adapter::textToSpeech::EngineCreationParams params;
  params.language = "zh-CN";
  params.person = 0;
  params.online = 1;
  params.extraParams = extra_params;

  return params;
}

void TtsPlatformImplOHOS::OnSpeechStartEvent() {
  TtsController::GetInstance()->OnTtsEvent(utterance_id_, TTS_EVENT_START, 0,
                                           -1, std::string());
}

void StartCallback() {
  auto task = base::BindOnce([]() {
    content::TtsPlatformImplOHOS::GetInstance()->OnSpeechStartEvent();
  });
  content::GetUIThreadTaskRunner({})->PostTask(FROM_HERE, std::move(task));
}

void TtsPlatformImplOHOS::OnSpeechEndEvent() {
  TtsController::GetInstance()->OnTtsEvent(utterance_id_, TTS_EVENT_END,
                                           utterance_length_, 0, std::string());
}

void EndCallback() {
  auto task = base::BindOnce([]() {
    content::TtsPlatformImplOHOS::GetInstance()->OnSpeechEndEvent();
  });
  content::GetUIThreadTaskRunner({})->PostTask(FROM_HERE, std::move(task));
}

void TtsPlatformImplOHOS::RegisterTtsEvent() {
  ohos::adapter::textToSpeech::TextToSpeechAdapter::GetInstance()
      .RegisterCallback(&StartCallback,
                        ohos::adapter::textToSpeech::TTS_ON_START);

  ohos::adapter::textToSpeech::TextToSpeechAdapter::GetInstance()
      .RegisterCallback(&EndCallback, ohos::adapter::textToSpeech::TTS_ON_END);
}

void TtsPlatformImplOHOS::InitializeVoices() {
  voices_ = ohos::adapter::textToSpeech::TextToSpeechAdapter::GetInstance()
                .TextToSpeechEngineListVoices();
  if (!voices_.empty()) {
    LOG(INFO)
        << "TTS TtsPlatformImplOHOS engine initialize voice list successfully";
  }
}

void TtsPlatformImplOHOS::GetVoices(std::vector<VoiceData>* out_voices) {
  if (!is_initialized_ || !out_voices) {
    LOGE("TTS The value cannot be assigned to out_voices by GetVoices");
    return;
  }

  for (auto item = voices_.begin(); item != voices_.end(); ++item) {
    out_voices->push_back(VoiceData());
    VoiceData& voice = out_voices->back();
    voice.native = true;
    voice.name = item->description;
    voice.lang = item->language;

    voice.events.insert(TTS_EVENT_START);
    voice.events.insert(TTS_EVENT_END);
    // currently do not support
    voice.events.insert(TTS_EVENT_CANCELLED);
    voice.events.insert(TTS_EVENT_MARKER);
    voice.events.insert(TTS_EVENT_PAUSE);
    voice.events.insert(TTS_EVENT_RESUME);
  }
}

void TtsPlatformImplOHOS::Speak(
    int utterance_id,
    const std::string& utterance,
    const std::string& lang,
    const VoiceData& voice,
    const UtteranceContinuousParameters& params,
    base::OnceCallback<void(bool)> on_speak_finished) {
  if (TtsPlatformImplOHOS::IsSpeaking()) {
    std::move(on_speak_finished).Run(false);
    return;
  }

  utterance_id_ = utterance_id;

  TtsController::GetInstance()->StripSSML(
      utterance, base::BindOnce(&TtsPlatformImplOHOS::ProcessSpeech,
                                base::Unretained(this), utterance_id, lang,
                                voice, params, std::move(on_speak_finished)));
}

void TtsPlatformImplOHOS::ProcessSpeech(
    int utterance_id,
    const std::string& lang,
    const VoiceData& voice,
    const UtteranceContinuousParameters& params,
    base::OnceCallback<void(bool)> on_speak_finished,
    const std::string& parsed_utterance) {
  utterance_ = parsed_utterance;
  utterance_length_ = parsed_utterance.size();

  ohos::adapter::textToSpeech::SpeakingParamsExtraParams extra_param;
  // The standard speech(rate) range of chromium is 0.1 to 10, which is
  // converted into the pitch range accepted by ohos through linear mapping. The
  // pitch range accepted by ohos is 0.5 to 2.
  if (params.rate == K_DEFAULT_RATE) {
    extra_param.speed = K_DEFAULT_RATE;
  } else if (params.rate > K_DEFAULT_RATE) {
    extra_param.speed = params.rate / K_DEFAULT_BASE + 1;
  } else {
    double linear_mapping_rate =
        (params.rate - 0.1) / (1 - 0.1) * (1 - K_DEFAULT_FACTOR) + K_DEFAULT_FACTOR;
    extra_param.speed = linear_mapping_rate;
  }

  // The standard volume range of chromium is 0 to 1, which is converted into
  // the pitch range accepted by ohos through linear mapping. The pitch range
  // accepted by ohos is 0 to 2.
  double linear_mapping_volume = params.volume * 2;
  extra_param.volume = linear_mapping_volume;

  // The standard pitch range of chromium is 0 to 2, which is converted into the
  // pitch range accepted by ohos through linear mapping. The pitch range
  // accepted by ohos is 0.5 to 2.
  if (params.pitch == K_DEFAULT_PITCH) {
    extra_param.pitch = K_DEFAULT_PITCH;
  } else if (params.pitch < K_DEFAULT_PITCH) {
    extra_param.pitch = params.pitch * (1 - K_DEFAULT_FACTOR) + K_DEFAULT_FACTOR;
  } else {
    extra_param.pitch = params.pitch;
  }

  // The Chromium parameter is not automatically set. The default value is
  // assigned based on the system interface document.
  extra_param.languageContext = "zh-CN";
  extra_param.audioType = "pcm";
  extra_param.playType = K_DEFAULT_PLAY_TYPE;
  extra_param.soundChannel = K_DEFAULT_SOUND_CHANNEL;
  extra_param.queueMode = K_DEFAULT_QUEUE_MODE;

  ohos::adapter::textToSpeech::SpeakingParams speak_params;
  // to keep id unique
  std::string id_suffix = TtsPlatformImplOHOS::GenerateIdSuffix();
  speak_params.requestId = K_TTS_SPEAK_REQUEST_ID_PREFIX + id_suffix;
  speak_params.extraParams = extra_param;

  bool succeeded =
      ohos::adapter::textToSpeech::TextToSpeechAdapter::GetInstance().Speak(
          parsed_utterance, speak_params);

  std::move(on_speak_finished).Run(succeeded);
}

bool TtsPlatformImplOHOS::StopSpeaking() {
  if (!TtsPlatformImplOHOS::IsSpeaking()) {
    LOG(INFO)
        << "TTS The engine is not speaking at the moment, no need to stop";
    return true;
  }

  utterance_id_ = K_INVALID_UTTERANCE_ID;

  bool succeeded =
      ohos::adapter::textToSpeech::TextToSpeechAdapter::GetInstance()
          .StopSpeaking();
  if (!succeeded) {
    LOG(ERROR) << "TTS engine stop speaking unsuccessfully";
  }
  return succeeded;
}

bool TtsPlatformImplOHOS::IsSpeaking() {
  return ohos::adapter::textToSpeech::TextToSpeechAdapter::GetInstance()
      .IsSpeaking();
}

bool TtsPlatformImplOHOS::PlatformImplSupported() {
  return is_supported_;
}

bool TtsPlatformImplOHOS::PlatformImplInitialized() {
  return is_initialized_;
}

void TtsPlatformImplOHOS::Pause() {
  TtsPlatformImplOHOS::StopSpeaking();
}

// Use with caution
void TtsPlatformImplOHOS::Shutdown() {
  bool succeeded =
      ohos::adapter::textToSpeech::TextToSpeechAdapter::GetInstance()
          .Shutdown();
  if (!succeeded) {
    LOG(ERROR) << "TTS TtsPlatformImplOHOS engine shutdown unsuccessfully";
    return;
  }
  is_initialized_ = false;
  LOG(INFO) << "TTS TtsPlatformImplOHOS engine shutdown successfully";
}
}  // namespace content
