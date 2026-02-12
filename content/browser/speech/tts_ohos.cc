// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/functional/callback.h"
#include "base/logging.h"
#include "base/no_destructor.h"
#include "base/uuid.h"
#include "content/browser/speech/tts_platform_impl.h"
#include "content/public/browser/browser_thread.h"
#include "ohos/adapter/speech/text_to_speech_adapter.h"
namespace content {

namespace {
constexpr int kInvalidUtteranceId = -1;
constexpr int kDefaultPitch = 1;
constexpr int kDefaultRate = 1;
constexpr int kDefaultPlayType = 1;
constexpr int kDefaultSoundChannel = 3;
constexpr int kDefaultQueueMode = 0;
constexpr int kDefaultBase = 10;
constexpr double kDefaultFactor = 0.5;
const char kSpeakRequestIdPrefix[] = "OHOSTTS";
const char kDefaultAudioType[] = "pcm";
const char kDefaultLanguage[] = "zh-CN";
const char kDefaultLocale[] = "CN";
const char kEngineName[] = "OHOS-zh-CN-engine";
constexpr int kStatusOffline = 1;
constexpr int kDefaultPerson = 13;
const char kDefaultStyle[] = "interaction-broadcast";
const char kVoiceStatusInstalled[] = "INSTALLED";
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
  int utterance_id_ = kInvalidUtteranceId;
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
  if (!succeeded) {
    LOG(ERROR) << __func__
               << " TTS TtsPlatformImplOHOS engine initialized unsuccessfully";
    return;
  }

  is_initialized_ = true;
  is_supported_ = true;
  TtsPlatformImplOHOS::InitializeVoices();
}

std::string TtsPlatformImplOHOS::GenerateIdSuffix() {
  return base::Uuid::GenerateRandomV4().AsLowercaseString();
}

ohos::adapter::textToSpeech::EngineCreationParams
TtsPlatformImplOHOS::CreateEngineParams() {
  // Creating a TTS engine requires a group of parameters with the specified
  // language as the input parameter. Currently, the TTS engine supports only
  // Chinese.
  // refer
  // https://developer.huawei.com/consumer/cn/doc/harmonyos-references/hms-ai-texttospeech#section1638144844811
  ohos::adapter::textToSpeech::EngineCreationParamsExtraParams extra_params;
  extra_params.style = kDefaultStyle;
  extra_params.locate = kDefaultLocale;
  extra_params.name = kEngineName;

  ohos::adapter::textToSpeech::EngineCreationParams params;
  params.language = kDefaultLanguage;
  params.person = kDefaultPerson;
  params.online = kStatusOffline;
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
  if (voices_.empty()) {
    LOG(ERROR) << __func__ << " [TTS] no available voice.";
  }
}

void TtsPlatformImplOHOS::GetVoices(std::vector<VoiceData>* out_voices) {
  if (!is_initialized_ || !out_voices) {
    LOG(ERROR)
        << __func__
        << " [TTS] The value cannot be assigned to out_voices by GetVoices";
    return;
  }

  for (auto item = voices_.begin(); item != voices_.end(); ++item) {
    if (item->status == kVoiceStatusInstalled) {
      out_voices->push_back(VoiceData());
      VoiceData& voice = out_voices->back();
      voice.native = true;
      voice.name = item->description;
      // convert language format from POSIX / Unicode CLDR to BCP 47 (IETF)
      std::string normalized_language(item->language);
      std::ranges::replace(normalized_language, '_', '-');
      voice.lang = normalized_language;

      voice.events.insert(TTS_EVENT_START);
      voice.events.insert(TTS_EVENT_END);
      // currently do not support
      voice.events.insert(TTS_EVENT_CANCELLED);
      voice.events.insert(TTS_EVENT_MARKER);
      voice.events.insert(TTS_EVENT_PAUSE);
      voice.events.insert(TTS_EVENT_RESUME);
    }
  }
  if (out_voices->empty()) {
    LOG(WARNING) << __func__ << " [TTS] no available voice.";
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
    LOG(WARNING) << __func__ << " [TTS] not speek since IsSpeaking";
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
  if (params.rate == kDefaultRate) {
    extra_param.speed = kDefaultRate;
  } else if (params.rate > kDefaultRate) {
    extra_param.speed = params.rate / kDefaultBase + 1;
  } else {
    double linear_mapping_rate =
        (params.rate - 0.1) / (1 - 0.1) * (1 - kDefaultFactor) +
        kDefaultFactor;
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
  if (params.pitch == kDefaultPitch) {
    extra_param.pitch = kDefaultPitch;
  } else if (params.pitch < kDefaultPitch) {
    extra_param.pitch =
        params.pitch * (1 - kDefaultFactor) + kDefaultFactor;
  } else {
    extra_param.pitch = params.pitch;
  }

  // The Chromium parameter is not automatically set. The default value is
  // assigned based on the system interface document.
  extra_param.languageContext = kDefaultLanguage;
  extra_param.audioType = kDefaultAudioType;
  extra_param.playType = kDefaultPlayType;
  extra_param.soundChannel = kDefaultSoundChannel;
  extra_param.queueMode = kDefaultQueueMode;

  ohos::adapter::textToSpeech::SpeakingParams speak_params;
  // to keep id unique
  std::string id_suffix = TtsPlatformImplOHOS::GenerateIdSuffix();
  speak_params.requestId = kSpeakRequestIdPrefix + id_suffix;
  speak_params.extraParams = extra_param;

  bool succeeded =
      ohos::adapter::textToSpeech::TextToSpeechAdapter::GetInstance().Speak(
          parsed_utterance, speak_params);

  std::move(on_speak_finished).Run(succeeded);
}

bool TtsPlatformImplOHOS::StopSpeaking() {
  if (!TtsPlatformImplOHOS::IsSpeaking()) {
    LOG(INFO)
        << __func__
        << " TTS The engine is not speaking at the moment, no need to stop";
    return true;
  }

  utterance_id_ = kInvalidUtteranceId;

  bool succeeded =
      ohos::adapter::textToSpeech::TextToSpeechAdapter::GetInstance()
          .StopSpeaking();
  if (!succeeded) {
    LOG(ERROR) << __func__ << " TTS engine stop speaking unsuccessfully";
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
    LOG(ERROR) << __func__
               << " TTS TtsPlatformImplOHOS engine shutdown unsuccessfully";
    return;
  }
  is_initialized_ = false;
  LOG(INFO) << __func__
            << " TTS TtsPlatformImplOHOS engine shutdown successfully";
}
}  // namespace content
