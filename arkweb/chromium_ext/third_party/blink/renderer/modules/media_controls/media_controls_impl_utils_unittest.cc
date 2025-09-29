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

#include "base/run_loop.h"
#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"

#define private public
#include "third_party/blink/renderer/core/html/media/html_media_element.h"
#include "third_party/blink/renderer/modules/media_controls/media_controls_impl.h"
#include "arkweb/chromium_ext/third_party/blink/renderer/modules/media_controls/media_controls_impl_utils.h"
#undef private

#include "arkweb/chromium_ext/base/ohos/sys_info_utils_ext.h"
#include "third_party/blink/renderer/core/dom/document.h"
#include "third_party/blink/renderer/core/dom/events/event.h"
#include "third_party/blink/renderer/core/event_type_names.h"
#include "third_party/blink/renderer/core/html/media/html_video_element.h"
#include "third_party/blink/renderer/core/input_type_names.h"
#include "third_party/blink/renderer/core/testing/page_test_base.h"
#include "third_party/blink/renderer/platform/heap/garbage_collected.h"
#include "third_party/blink/renderer/core/frame/settings.h"
#include "third_party/blink/renderer/modules/media_controls/elements/media_control_scrubbing_message_element.h"
#include "third_party/blink/renderer/platform/mediastream/media_stream_component.h"
#include "third_party/blink/renderer/platform/mediastream/media_stream_descriptor.h"
#include "third_party/blink/renderer/core/html/media/html_media_element_controls_list.h"
#include "third_party/blink/renderer/modules/media_controls/elements/media_control_panel_element.h"
#include "third_party/blink/renderer/core/frame/local_frame.h"
#include "third_party/blink/renderer/core/fullscreen/fullscreen.h"
#include "third_party/blink/renderer/platform/testing/unit_test_helpers.h"
#include "third_party/blink/renderer/modules/media_controls/elements/media_control_playback_speed_button_element.h"
#include "third_party/blink/renderer/modules/media_controls/elements/media_control_current_time_display_element.h"

using ::testing::_;
using ::testing::AnyNumber;
using ::testing::Return;

namespace blink {

class MediaControlsImplUtilsTest : public PageTestBase {
 public:
  void SetUp() final {
    PageTestBase::SetUp();
    media_element_ = MakeGarbageCollected<HTMLVideoElement>(GetDocument());
    media_element_->SetBooleanAttribute(html_names::kControlsAttr, true);
    GetDocument().body()->AppendChild(media_element_);
    media_controls_ = static_cast<MediaControlsImpl*>(media_element_->GetMediaControls());
    ASSERT_NE(media_controls_, nullptr);
  }

  void TearDown() {}

 protected:
  HTMLMediaElement& GetMediaElement() { return *media_element_.Get(); }

  Persistent<HTMLMediaElement> media_element_;
  Persistent<MediaControlsImpl> media_controls_;
};

TEST_F(MediaControlsImplUtilsTest, MaybeParserAppendChild) {
  void MaybeParserAppendChild(Element* parent, Element* child);
  MaybeParserAppendChild(media_element_.Get(), nullptr);
  EXPECT_FALSE(media_element_->hasChildren());
}

TEST_F(MediaControlsImplUtilsTest, ShouldShowPlaybackSpeedButton) {
  bool ShouldShowPlaybackSpeedButton(HTMLMediaElement& media_element);
  bool ret = ShouldShowPlaybackSpeedButton(GetMediaElement());
  EXPECT_TRUE(ret);

  GetMediaElement().GetDocument().GetSettings()->SetCustomMediaPlayerEnabled(true);
  GetMediaElement().duration_ = 0.0;
  GetMediaElement().ready_state_ = HTMLMediaElement::kHaveMetadata;
  ret = ShouldShowPlaybackSpeedButton(GetMediaElement());
  EXPECT_FALSE(ret);

  GetMediaElement().duration_ = std::numeric_limits<double>::infinity();
  GetMediaElement().ready_state_ = HTMLMediaElement::kHaveMetadata;
  ret = ShouldShowPlaybackSpeedButton(GetMediaElement());
  EXPECT_FALSE(ret);

  MediaStreamComponentVector dummy_components;
  auto* media_stream_descriptor = MakeGarbageCollected<MediaStreamDescriptor>(dummy_components, dummy_components);
  GetMediaElement().src_object_stream_descriptor_ = media_stream_descriptor;
  ret = ShouldShowPlaybackSpeedButton(GetMediaElement());
  EXPECT_FALSE(ret);

  GetMediaElement().ControlsListInternal()->Add(keywords::kNoplaybackrate);
  ret = ShouldShowPlaybackSpeedButton(GetMediaElement());
  EXPECT_FALSE(ret);
}

TEST_F(MediaControlsImplUtilsTest, InitMediaControlsImplUtils) {
  testing::internal::CaptureStderr();
  MediaControlsImplUtils mediaControlsImplUtils_ = MediaControlsImplUtils(media_controls_.Get(), GetMediaElement());
  mediaControlsImplUtils_.InitMediaControlsImplUtils(nullptr, GetMediaElement());
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_NE(log_output.find("InitMediaControlsImplUtils media_controls_impl is nullptr"), std::string::npos);
}

TEST_F(MediaControlsImplUtilsTest, InitMediaControlsImplUtils_001) {
  MediaControlsImplUtils mediaControlsImplUtils_ = MediaControlsImplUtils(media_controls_.Get(), GetMediaElement());
  GetMediaElement().GetDocument().GetSettings()->SetCustomMediaPlayerEnabled(true);
  mediaControlsImplUtils_.InitMediaControlsImplUtils(media_controls_.Get(), GetMediaElement());
  EXPECT_EQ(mediaControlsImplUtils_.media_controls_impl_->top_row_panel_, nullptr);
  EXPECT_EQ(mediaControlsImplUtils_.media_controls_impl_->timeline_row_panel_, nullptr);
}

TEST_F(MediaControlsImplUtilsTest, ShouldShowPlaybackSpeedButtonExt) {
  GetMediaElement().GetDocument().GetSettings()->SetCustomMediaPlayerEnabled(true);
  bool ret = MediaControlsImplUtils::ShouldShowPlaybackSpeedButtonExt(GetMediaElement());
  EXPECT_TRUE(ret);

  GetMediaElement().duration_ = 1.0;
  ret = MediaControlsImplUtils::ShouldShowPlaybackSpeedButtonExt(GetMediaElement());
  EXPECT_TRUE(ret);

  GetMediaElement().duration_ = 0.0;
  GetMediaElement().ready_state_ = HTMLMediaElement::kHaveMetadata;
  ret = MediaControlsImplUtils::ShouldShowPlaybackSpeedButtonExt(GetMediaElement());
  EXPECT_FALSE(ret);
}

TEST_F(MediaControlsImplUtilsTest, UpdateOverflowMenuWantedExt) {
  MediaControlsImplUtils mediaControlsImplUtils_ = MediaControlsImplUtils(media_controls_.Get(), GetMediaElement());
  GetMediaElement().GetDocument().GetSettings()->SetCustomMediaPlayerEnabled(true);
  std::pair<MediaControlElementBase*, bool> row_elements[kRowElementsCount];
  for (auto& element : row_elements) {
      element = {nullptr, false};
  }
  mediaControlsImplUtils_.UpdateOverflowMenuWantedExt(row_elements);
  EXPECT_TRUE(row_elements[kPlaybackSpeedIndexToRowColumns].second);
}

TEST_F(MediaControlsImplUtilsTest, ShouldShowVideoControlsHM) {
  MediaControlsImplUtils mediaControlsImplUtils_ = MediaControlsImplUtils(media_controls_.Get(), GetMediaElement());
  mediaControlsImplUtils_.media_controls_impl_ = nullptr;
  bool ret = mediaControlsImplUtils_.ShouldShowVideoControlsHM();
  EXPECT_FALSE(ret);
}

TEST_F(MediaControlsImplUtilsTest, UpdateDeviceCSSClassExt) {
  MediaControlsImplUtils mediaControlsImplUtils_ = MediaControlsImplUtils(media_controls_.Get(), GetMediaElement());
  mediaControlsImplUtils_.media_controls_impl_ = nullptr;
  mediaControlsImplUtils_.UpdateDeviceCSSClassExt();
  mediaControlsImplUtils_.media_controls_impl_ = media_controls_.Get();
  mediaControlsImplUtils_.UpdateDeviceCSSClassExt();
  if (base::ohos::IsPcDevice()) {
    EXPECT_TRUE(mediaControlsImplUtils_.media_controls_impl_->classList().contains(AtomicString("device-pc")));
  } else {
    EXPECT_TRUE(mediaControlsImplUtils_.media_controls_impl_->classList().contains(AtomicString("device-phone")));
  }
}

TEST_F(MediaControlsImplUtilsTest, InitializeControlsExt) {
  MediaControlsImplUtils mediaControlsImplUtils_ = MediaControlsImplUtils(media_controls_.Get(), GetMediaElement());
  testing::internal::CaptureStderr();
  mediaControlsImplUtils_.media_controls_impl_ = nullptr;
  mediaControlsImplUtils_.InitializeControlsExt();
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_NE(log_output.find("InitializeControlsExt media_controls_impl_ is nullptr"), std::string::npos);
}

TEST_F(MediaControlsImplUtilsTest, InitializeControlsExt_001) {
  MediaControlsImplUtils mediaControlsImplUtils_ = MediaControlsImplUtils(media_controls_.Get(), GetMediaElement());
  GetMediaElement().GetDocument().GetSettings()->SetCustomMediaPlayerEnabled(true);
  mediaControlsImplUtils_.InitializeControlsExt();
  EXPECT_NE(mediaControlsImplUtils_.media_controls_impl_->top_row_panel_, nullptr);
}

TEST_F(MediaControlsImplUtilsTest, PopulatePanelExt) {
  MediaControlsImplUtils mediaControlsImplUtils_ = MediaControlsImplUtils(media_controls_.Get(), GetMediaElement());
  mediaControlsImplUtils_.media_controls_impl_->scrubbing_panel_ = nullptr;
  mediaControlsImplUtils_.PopulatePanelExt();
  EXPECT_TRUE(mediaControlsImplUtils_.media_controls_impl_->panel_->hasChildren());
}

TEST_F(MediaControlsImplUtilsTest, PopulatePanelHM) {
  testing::internal::CaptureStderr();
  MediaControlsImplUtils mediaControlsImplUtils_ = MediaControlsImplUtils(media_controls_.Get(), GetMediaElement());
  mediaControlsImplUtils_.media_controls_impl_->download_button_ = nullptr;
  mediaControlsImplUtils_.PopulatePanelHM();
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_NE(log_output.find("PopulatePanelHM download_button_ is nullptr"), std::string::npos);

  testing::internal::CaptureStderr();
  mediaControlsImplUtils_.media_controls_impl_ = nullptr;
  mediaControlsImplUtils_.PopulatePanelHM();
  log_output = testing::internal::GetCapturedStderr();
  EXPECT_NE(log_output.find("PopulatePanelHM media_controls_impl_ is nullptr"), std::string::npos);
}

TEST_F(MediaControlsImplUtilsTest, UpdateCSSClassFromStateEnablePlaybackSpeedButton) {
  MediaControlsImplUtils mediaControlsImplUtils_ = MediaControlsImplUtils(media_controls_.Get(), GetMediaElement());
  GetMediaElement().GetDocument().GetSettings()->SetCustomMediaPlayerEnabled(true);
  bool ret = mediaControlsImplUtils_.UpdateCSSClassFromStateEnablePlaybackSpeedButton();
  EXPECT_TRUE(ret);
}

TEST_F(MediaControlsImplUtilsTest, UpdateCSSClassFromStateDisablePlaybackSpeedButton) {
  MediaControlsImplUtils mediaControlsImplUtils_ = MediaControlsImplUtils(media_controls_.Get(), GetMediaElement());
  GetMediaElement().GetDocument().GetSettings()->SetCustomMediaPlayerEnabled(true);
  media_controls_->playback_speed_button_->setAttribute(html_names::kDisabledAttr, AtomicString(""));
  bool ret = mediaControlsImplUtils_.UpdateCSSClassFromStateDisablePlaybackSpeedButton();
  EXPECT_TRUE(ret);
}

TEST_F(MediaControlsImplUtilsTest, UpdateCSSClassFromStateEnableCurrentTimeDisplay) {
  MediaControlsImplUtils mediaControlsImplUtils_ = MediaControlsImplUtils(media_controls_.Get(), GetMediaElement());
  GetMediaElement().GetDocument().GetSettings()->SetCustomMediaPlayerEnabled(true);
  mediaControlsImplUtils_.UpdateCSSClassFromStateEnableCurrentTimeDisplay();
  EXPECT_FALSE(media_controls_->current_time_display_->classList().contains(AtomicString("disabled")));
}

TEST_F(MediaControlsImplUtilsTest, UpdateCSSClassFromStateDisableCurrentTimeDisplay) {
  MediaControlsImplUtils mediaControlsImplUtils_ = MediaControlsImplUtils(media_controls_.Get(), GetMediaElement());
  GetMediaElement().GetDocument().GetSettings()->SetCustomMediaPlayerEnabled(true);
  mediaControlsImplUtils_.UpdateCSSClassFromStateDisableCurrentTimeDisplay();
  EXPECT_TRUE(media_controls_->current_time_display_->classList().contains(AtomicString("disabled")));
}

TEST_F(MediaControlsImplUtilsTest, BeginScrubbingStartTimer) {
  MediaControlsImplUtils mediaControlsImplUtils_ = MediaControlsImplUtils(media_controls_.Get(), GetMediaElement());
  // stop to active
  mediaControlsImplUtils_.BeginScrubbingStartTimer();
  EXPECT_TRUE(media_controls_->scrubbing_timer_.IsActive());

  // active to active
  mediaControlsImplUtils_.BeginScrubbingStartTimer();
  EXPECT_TRUE(media_controls_->scrubbing_timer_.IsActive());
}

TEST_F(MediaControlsImplUtilsTest, BeginScrubbingStopTimer) {
  MediaControlsImplUtils mediaControlsImplUtils_ = MediaControlsImplUtils(media_controls_.Get(), GetMediaElement());
  mediaControlsImplUtils_.BeginScrubbingStartTimer();
  mediaControlsImplUtils_.BeginScrubbingStopTimer();
  EXPECT_FALSE(media_controls_->scrubbing_timer_.IsActive());

  mediaControlsImplUtils_.BeginScrubbingStopTimer();
  EXPECT_FALSE(media_controls_->scrubbing_timer_.IsActive());

  testing::internal::CaptureStderr();
  mediaControlsImplUtils_.media_controls_impl_ = nullptr;
  mediaControlsImplUtils_.BeginScrubbingStopTimer();
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_NE(log_output.find("BeginScrubbingStopTimer media_controls_impl_ is nullptr"), std::string::npos);
}

TEST_F(MediaControlsImplUtilsTest, GetSizingClassHM) {
  MediaControlsImplUtils mediaControlsImplUtils_ = MediaControlsImplUtils(media_controls_.Get(), GetMediaElement());
  auto ret = mediaControlsImplUtils_.GetSizingClassHM();
  EXPECT_EQ(ret, MediaControlsSizingClass::kSmall);

  mediaControlsImplUtils_.media_controls_impl_->size_.set_width(720);
  ret = mediaControlsImplUtils_.GetSizingClassHM();
  EXPECT_EQ(ret, MediaControlsSizingClass::kMedium);

  mediaControlsImplUtils_.media_controls_impl_->size_.set_width(960);
  ret = mediaControlsImplUtils_.GetSizingClassHM();
  EXPECT_EQ(ret, MediaControlsSizingClass::kLarge);
}

TEST_F(MediaControlsImplUtilsTest, MakeTransparentImmediately) {
  MediaControlsImplUtils mediaControlsImplUtils_ = MediaControlsImplUtils(media_controls_.Get(), GetMediaElement());
  testing::internal::CaptureStderr();
  mediaControlsImplUtils_.media_controls_impl_ = nullptr;
  mediaControlsImplUtils_.MakeTransparentImmediately();
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_NE(log_output.find("MakeTransparentImmediately media_controls_impl_ is nullptr"), std::string::npos);
}

TEST_F(MediaControlsImplUtilsTest, UpdateSizingCSSClassExt) {
  MediaControlsImplUtils mediaControlsImplUtils_ = MediaControlsImplUtils(media_controls_.Get(), GetMediaElement());
  mediaControlsImplUtils_.media_controls_impl_->size_.set_width(480);
  mediaControlsImplUtils_.UpdateSizingCSSClassExt();
  EXPECT_TRUE(mediaControlsImplUtils_.media_controls_impl_->classList().contains(AtomicString("sizing-small")));

  mediaControlsImplUtils_.media_controls_impl_->size_.set_width(720);
  mediaControlsImplUtils_.UpdateSizingCSSClassExt();
  EXPECT_TRUE(mediaControlsImplUtils_.media_controls_impl_->classList().contains(AtomicString("sizing-medium")));

  mediaControlsImplUtils_.media_controls_impl_->size_.set_width(960);
  mediaControlsImplUtils_.UpdateSizingCSSClassExt();
  EXPECT_TRUE(mediaControlsImplUtils_.media_controls_impl_->classList().contains(AtomicString("sizing-large")));

  testing::internal::CaptureStderr();
  mediaControlsImplUtils_.media_controls_impl_ = nullptr;
  mediaControlsImplUtils_.UpdateSizingCSSClassExt();
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_NE(log_output.find("UpdateSizingCSSClassExt media_controls_impl_ is nullptr"), std::string::npos);
}

TEST_F(MediaControlsImplUtilsTest, OnDurationChangeExt) {
  MediaControlsImplUtils mediaControlsImplUtils_ = MediaControlsImplUtils(media_controls_.Get(), GetMediaElement());
  testing::internal::CaptureStderr();
  mediaControlsImplUtils_.media_controls_impl_ = nullptr;
  mediaControlsImplUtils_.OnDurationChangeExt();
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_NE(log_output.find("OnDurationChangeExt media_controls_impl_ is nullptr"), std::string::npos);
}

TEST_F(MediaControlsImplUtilsTest, OnDurationChangeExt_001) {
  MediaControlsImplUtils mediaControlsImplUtils_ = MediaControlsImplUtils(media_controls_.Get(), GetMediaElement());
  media_controls_->playback_speed_button_->SetIsWanted(false);

  mediaControlsImplUtils_.OnDurationChangeExt();
  EXPECT_FALSE(media_controls_->playback_speed_button_->IsWanted());

  GetMediaElement().GetDocument().GetSettings()->SetCustomMediaPlayerEnabled(true);
  mediaControlsImplUtils_.OnDurationChangeExt();
  EXPECT_TRUE(media_controls_->playback_speed_button_->IsWanted());
}

TEST_F(MediaControlsImplUtilsTest, OnSeekingExt) {
  MediaControlsImplUtils mediaControlsImplUtils_ = MediaControlsImplUtils(media_controls_.Get(), GetMediaElement());
  GetMediaElement().GetDocument().GetSettings()->SetCustomMediaPlayerEnabled(true);
  mediaControlsImplUtils_.media_controls_impl_->scrubbing_panel_ =
      MakeGarbageCollected<MediaControlScrubbingPanelElement>(*mediaControlsImplUtils_.media_controls_impl_);

  mediaControlsImplUtils_.media_controls_impl_->is_begin_scrubbing = false;
  mediaControlsImplUtils_.OnSeekingExt();
  EXPECT_FALSE(media_controls_->scrubbing_panel_->IsWanted());

  mediaControlsImplUtils_.media_controls_impl_->is_begin_scrubbing = true;
  mediaControlsImplUtils_.OnSeekingExt();
  EXPECT_TRUE(media_controls_->scrubbing_panel_->IsWanted());
}

TEST_F(MediaControlsImplUtilsTest, OnEnteredFullscreenSetIswanted) {
  MediaControlsImplUtils mediaControlsImplUtils_ = MediaControlsImplUtils(media_controls_.Get(), GetMediaElement());
  mediaControlsImplUtils_.media_controls_impl_->entered_fullscreen_panel_ =
      MakeGarbageCollected<MediaControlEnteredFullscreenPanelElement>(*mediaControlsImplUtils_.media_controls_impl_);
  mediaControlsImplUtils_.media_controls_impl_->entered_fullscreen_title_display_ =
      MakeGarbageCollected<MediaControlEnteredFullscreenTitleDisplayElement>(*media_controls_);
  mediaControlsImplUtils_.OnEnteredFullscreenSetIswanted();
  EXPECT_TRUE(mediaControlsImplUtils_.media_controls_impl_->classList().contains(AtomicString("fullscreen")));
}

TEST_F(MediaControlsImplUtilsTest, OnEnteredFullscreenAddStyleElement) {
  MediaControlsImplUtils mediaControlsImplUtils_ = MediaControlsImplUtils(media_controls_.Get(), GetMediaElement());
  mediaControlsImplUtils_.media_controls_impl_ = nullptr;
  mediaControlsImplUtils_.OnEnteredFullscreenAddStyleElement();
  EXPECT_TRUE(media_controls_->hasChildren());

  mediaControlsImplUtils_.media_controls_impl_ = media_controls_.Get();
  mediaControlsImplUtils_.media_controls_impl_->MediaElement().video_assistant_enabled_ = true;
  mediaControlsImplUtils_.media_controls_impl_->MediaElement().video_assistant_ = true;
  mediaControlsImplUtils_.OnEnteredFullscreenAddStyleElement();
  EXPECT_EQ(media_controls_->style_element_->parentNode(), media_controls_);
}

TEST_F(MediaControlsImplUtilsTest, OnExitedFullscreenSetIswanted) {
  MediaControlsImplUtils mediaControlsImplUtils_ = MediaControlsImplUtils(media_controls_.Get(), GetMediaElement());
  mediaControlsImplUtils_.media_controls_impl_->entered_fullscreen_panel_ = nullptr;
  mediaControlsImplUtils_.OnExitedFullscreenSetIswanted();
  mediaControlsImplUtils_.media_controls_impl_->entered_fullscreen_panel_ =
      MakeGarbageCollected<MediaControlEnteredFullscreenPanelElement>(*media_controls_);
  mediaControlsImplUtils_.media_controls_impl_->entered_fullscreen_title_display_ =
      MakeGarbageCollected<MediaControlEnteredFullscreenTitleDisplayElement>(*media_controls_);
  mediaControlsImplUtils_.OnExitedFullscreenSetIswanted();
  EXPECT_FALSE(mediaControlsImplUtils_.media_controls_impl_->classList().contains(AtomicString("fullscreen")));
}

TEST_F(MediaControlsImplUtilsTest, OnExitedFullscreenRemoveStyleElement) {
  MediaControlsImplUtils mediaControlsImplUtils_ = MediaControlsImplUtils(media_controls_.Get(), GetMediaElement());
  mediaControlsImplUtils_.OnExitedFullscreenRemoveStyleElement();
  EXPECT_TRUE(media_controls_->hasChildren());

  mediaControlsImplUtils_.media_controls_impl_->style_element_ = MakeGarbageCollected<HTMLStyleElement>(
      mediaControlsImplUtils_.media_controls_impl_->GetDocument(), CreateElementFlags());
  mediaControlsImplUtils_.OnExitedFullscreenRemoveStyleElement();
  EXPECT_NE(media_controls_->style_element_->parentNode(), media_controls_);
}

TEST_F(MediaControlsImplUtilsTest, OnPlaybackSpeedRateChanged) {
  MediaControlsImplUtils mediaControlsImplUtils_ = MediaControlsImplUtils(media_controls_.Get(), GetMediaElement());
  mediaControlsImplUtils_.media_controls_impl_ = nullptr;
  mediaControlsImplUtils_.OnPlaybackSpeedRateChanged();
  EXPECT_FALSE(media_controls_->playback_speed_button_->FastHasAttribute(html_names::kAriaHiddenAttr));

  mediaControlsImplUtils_.media_controls_impl_ = media_controls_.Get();
  mediaControlsImplUtils_.OnPlaybackSpeedRateChanged();
  EXPECT_TRUE(media_controls_->playback_speed_button_->FastHasAttribute(html_names::kAriaHiddenAttr));
}

TEST_F(MediaControlsImplUtilsTest, CreateExt) {
  MediaControlsImplUtils mediaControlsImplUtils_ = MediaControlsImplUtils(media_controls_.Get(), GetMediaElement());
  mediaControlsImplUtils_.CreateExt(nullptr, GetMediaElement());
  mediaControlsImplUtils_.CreateExt(media_controls_.Get(), GetMediaElement());
  EXPECT_FALSE(media_controls_->classList().contains(AtomicString("rtl")));
}

TEST_F(MediaControlsImplUtilsTest, BeginScrubbingExt) {
  MediaControlsImplUtils mediaControlsImplUtils_ = MediaControlsImplUtils(media_controls_.Get(), GetMediaElement());
  mediaControlsImplUtils_.media_controls_impl_->scrubbing_panel_ =
      MakeGarbageCollected<MediaControlScrubbingPanelElement>(*mediaControlsImplUtils_.media_controls_impl_);

  mediaControlsImplUtils_.BeginScrubbingExt(false);
  EXPECT_FALSE(media_controls_->scrubbing_panel_->IsWanted());

  mediaControlsImplUtils_.BeginScrubbingExt(true);
  EXPECT_TRUE(media_controls_->scrubbing_panel_->IsWanted());
}

TEST_F(MediaControlsImplUtilsTest, ScrubbingTimerFiredExt) {
  MediaControlsImplUtils mediaControlsImplUtils_ = MediaControlsImplUtils(media_controls_.Get(), GetMediaElement());
  mediaControlsImplUtils_.media_controls_impl_->is_begin_scrubbing = true;
  mediaControlsImplUtils_.ScrubbingTimerFiredExt();
  EXPECT_FALSE(mediaControlsImplUtils_.media_controls_impl_->is_begin_scrubbing);

  mediaControlsImplUtils_.ScrubbingTimerFiredExt();
  EXPECT_FALSE(mediaControlsImplUtils_.media_controls_impl_->is_begin_scrubbing);

  mediaControlsImplUtils_.media_controls_impl_->scrubbing_panel_ =
      MakeGarbageCollected<MediaControlScrubbingPanelElement>(*mediaControlsImplUtils_.media_controls_impl_);
  mediaControlsImplUtils_.media_controls_impl_->is_begin_scrubbing = true;
  mediaControlsImplUtils_.ScrubbingTimerFiredExt();
  EXPECT_FALSE(mediaControlsImplUtils_.media_controls_impl_->is_begin_scrubbing);

  GetMediaElement().ClearFlag(Node::kIsConnectedFlag);
  mediaControlsImplUtils_.media_controls_impl_->is_begin_scrubbing = true;
  mediaControlsImplUtils_.ScrubbingTimerFiredExt();
  EXPECT_TRUE(mediaControlsImplUtils_.media_controls_impl_->is_begin_scrubbing);
}

}  // namespace blink