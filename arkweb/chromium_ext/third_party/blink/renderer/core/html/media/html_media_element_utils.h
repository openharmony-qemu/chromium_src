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
#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_HTML_MEDIA_ELEMENT_UTILS_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_HTML_MEDIA_ELEMENT_UTILS_H_

#include "third_party/blink/public/platform/web_string.h"
#include "third_party/blink/renderer/platform/heap/garbage_collected.h"
#include "third_party/blink/renderer/platform/heap/member.h"

#if BUILDFLAG(ARKWEB_MEDIA_CAPABILITIES_ENHANCE)
#include "arkweb/chromium_ext/third_party/blink/public/mojom/media/video_experience_reporter.mojom-blink.h"
#endif // ARKWEB_MEDIA_CAPABILITIES_ENHANCE

namespace blink {
class HTMLMediaElement;

 class HTMLMediaElementUtils {
 public:
    enum class PlayState { kPause = 0, kPlay, kWaiting, kPlaying };
    WebString GetTitle() const;
    WebString GetIconUrl();
    DISALLOW_NEW();
    void Trace(Visitor* visitor) const;
    Member<HTMLMediaElement> htmlMediaElement_ = nullptr;
    explicit HTMLMediaElementUtils(HTMLMediaElement* element);

#if BUILDFLAG(ARKWEB_MEDIA_CAPABILITIES_ENHANCE)
    class Recorder {
        public:
        void SetThreshold(base::TimeDelta threshold);
        void StartRecord();
        void PauseRecord();
        void StopRecord();
        base::TimeDelta GetDuration();
        void Reset();
        private:
        base::TimeDelta threshold_;
        base::TimeTicks start_time_;
        base::TimeDelta accumulated_duration_;
        base::TimeDelta total_duration_;
    };
    Recorder played_time_recorder_;
    Recorder freeze_time_recorder_;
    mojom::blink::VideoExpParamsPtr ReportVideoExperienceToBI();
#endif // ARKWEB_MEDIA_CAPABILITIES_ENHANCE

    void ScheduleNamedEventUtils(const AtomicString& event_name);
    void ResetMediaPlayerAndMediaSourceUtils();

    /**
    * 场景类型
    */
    enum class ScenarioType {
        /**
        * tab页
        */
        SCENARIO_TAB_TYPE = 1,

        /**
        * cct
        */
        SCENARIO_CCT_TYPE = 2,

        /**
        * pwa
        */
        SCENARIO_PWA_TYPE = 3,

        /**
        * webui
        */
        SCENARIO_WEBUI_TYPE = 4,

        /**
        * 信息流
        */
        SCENARIO_FEEDSPAGE_TYPE = 5,

        /**
        * 其他
        */
        SCENARIO_OTHER_TYPE = 99,
    };

    bool IsFeedsPage() const;
#if BUILDFLAG(ARKWEB_CUSTOM_VIDEO_PLAYER)
    bool IsMediaPlayerShown() const;
#endif

#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
  media::mojom::blink::VideoAttributesForVASTPtr CollectVideoAttributesForVAST();
  void TryNotifyVideoPlaying();
  void UpdateVideoAssistantAttributes();
  void NotifyVideoVisible(bool visible);
  void EnterFullScreenOverlay();
  void RequestAVCastStarted();
  void UpdatePlayStateOverlay(PlayState playState);
  void MutedChangedOverlay(bool muted);
  void PlaybackRateChangedOverlay(double playback_rate);
  void DurationChangedOverlay(double duration);
  void TimeUpdateOverlay(double current_time);
  void BufferedEndTimeChangedOverlay(double buffered_end_time);
  double CalculateBufferedEndTime();
  void EndedOverlay();
  void FullscreenChangedOverlay(bool fullscreen);
  void SeekingOverlay();
  void SeekingFinishedOverlay();
  void ErrorOverlay(int32_t error_code, const String& error_msg);
  void VideoSizeChangedOverlay(int32_t width, int32_t height);
  bool IsRTL() const;
  void SetVolume(double volume);
  void OnVolumeChanged(double volume);
#endif
  void DidPlayerMutedStatusChangeExt(bool muted);

#if BUILDFLAG(ARKWEB_EXT_VIDEO_LOAD_OPTIMIZATION)
  bool IsUseVideoLoadOptimization() const;
  void SetVideoIsPlaying(bool playing);
#endif // ARKWEB_EXT_VIDEO_LOAD_OPTIMIZATION

#if BUILDFLAG(ARKWEB_MEDIA_CAST)
  void OnMediaCastEnter();
  void OnNotifyMeidaCastUri();
#endif // BUILDFLAG(ARKWEB_MEDIA_CAST)

private:
#if BUILDFLAG(ARKWEB_MEDIA_CAPABILITIES_ENHANCE)
  int media_player_waiting_cnt_ = 0;
  bool has_reported_experience_{false};
  std::string GetMainUrl() const;
#endif // ARKWEB_MEDIA_CAPABILITIES_ENHANCE
 };

 } // namespace blink

 #endif  // THIRD_PARTY_BLINK_RENDERER_CORE_HTML_MEDIA_ELEMENT_UTILS_H_