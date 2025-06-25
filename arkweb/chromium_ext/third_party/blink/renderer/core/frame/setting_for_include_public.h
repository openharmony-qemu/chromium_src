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

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_FRAME_SETTINGS_EXT_PUBLIC_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_FRAME_SETTINGS_EXT_PUBLIC_H_

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_FRAME_SETTINGS_H_
#error "must be in include form THIRD_PARTY_BLINK_RENDERER_CORE_FRAME_SETTINGS_H_"
#endif

#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
  void SetScrollable(bool enabled) { scroll_enabled_ = enabled; }
  bool GetScrollable() { return scroll_enabled_; }
#endif  // BUILDFLAG(ARKWEB_INPUT_EVENTS)

#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
  void SetVerticalHideScrollbars(bool hide_vertical_scrollbars) {
    hide_vertical_scrollbars_ = hide_vertical_scrollbars;
  }
  void SetHorizontalHideScrollbars(bool hide_horizontal_scrollbars) {
    hide_horizontal_scrollbars_ = hide_horizontal_scrollbars;
  }
  bool GetVerticalHideScrollbars() { return hide_vertical_scrollbars_; }
  bool GetHorizontalHideScrollbars() { return hide_horizontal_scrollbars_; }
  void SetOverscrollMode(int overscrollMode) {
    overscroll_mode_ = overscrollMode;
  }
  int GetOverscrollMode() { return overscroll_mode_; }
#endif  // BUILDFLAG(ARKWEB_INPUT_EVENTS)

#if BUILDFLAG(ARKWEB_SAME_LAYER)
  void RegisterNativeEmbedRule(const WebString& tag, const WebString& type) {
    embed_rule_[tag] = type;
  }

  std::map<WebString, WebString> NativeEmbedRule() { return embed_rule_; }
#endif

#if BUILDFLAG(ARKWEB_EXT_FREE_COPY)
  void SetContextMenuCustomization(bool contextmenu_customization_enabled) {
    contextmenu_customization_enabled_ = contextmenu_customization_enabled;
  }

  bool IsContextMenuCustomizationEnabled() {
    return contextmenu_customization_enabled_;
  }
#endif

#if BUILDFLAG(ARKWEB_CUSTOM_VIDEO_PLAYER)
  void SetCustomVideoPlayerEnabled(bool enable) {
    custom_video_player_enabled_ = enable;
  }
  bool IsCustomVideoPlayerEnabled() const {
    return custom_video_player_enabled_;
  }
  void SetCustomVideoPlayerOverlay(bool overlay) {
    custom_video_player_overlay_ = overlay;
  }
  bool IsCustomVideoPlayerOverlay() const {
    return custom_video_player_overlay_;
  }
#endif  // ARKWEB_CUSTOM_VIDEO_PLAYER

#if BUILDFLAG(ARKWEB_MEDIA_CAPABILITIES_ENHANCE)
  void SetUsageScenario(int32_t usage_scenario) {
    usage_scenario_ = usage_scenario;
  }

  int32_t GetUsageScenario() const { return usage_scenario_; }
#endif  // ARKWEB_MEDIA_CAPABILITIES_ENHANCE

#if BUILDFLAG(ARKWEB_COPY_OPTION)
  void SetCopyOption(blink::mojom::CopyOptionMode copyOption) {
    copy_option_ = copyOption;
  }

  mojom::CopyOptionMode GetCopyOption() { return copy_option_; }
#endif  // BUILDFLAG(ARKWEB_COPY_OPTION)

#if BUILDFLAG(ARKWEB_SCROLLBAR)
  void SetScrollBarColor(uint32_t colorValue) { scrollbar_color_ = colorValue; }

  uint32_t GetScrollBarColor() { return scrollbar_color_; }
#endif

#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
  void SetVideoAssistantEnabled(bool enabled) {
    video_assistant_enabled_ = enabled;
  }
  bool GetVideoAssistantEnabled() { return video_assistant_enabled_; }
  void SetCustomMediaPlayerEnabled(bool enabled) {
    custom_media_player_enabled_ = enabled;
  }
  bool GetCustomMediaPlayerEnabled() { return custom_media_player_enabled_; }
#endif  // ARKWEB_VIDEO_ASSISTANT

#if BUILDFLAG(ARKWEB_SCROLLBAR_AVOID_CORNER)
  void SetBorderRadiusFromWeb(double borderRadiusTopLeft,
                              double borderRadiusTopRight,
                              double borderRadiusBottomLeft,
                              double borderRadiusBottomRight)
  {
    border_radius_top_left_ = borderRadiusTopLeft;
    border_radius_top_right_ = borderRadiusTopRight;
    border_radius_bottom_left_ = borderRadiusBottomLeft;
    border_radius_bottom_right_ = borderRadiusBottomRight;
  }
  double GetBorderRadiusTopLeft() { return border_radius_top_left_; }
  double GetBorderRadiusTopRight() { return border_radius_top_right_; }
  double GetBorderRadiusBottomLeft() { return border_radius_bottom_left_; }
  double GetBorderRadiusBottomRight() { return border_radius_bottom_right_; }
#endif  // ARKWEB_SCROLLBAR_AVOID_CORNER

#if BUILDFLAG(ARKWEB_MENU)
  void SetTouchHandleExistState(bool touchHandleExist) {
    touch_handle_exist_ = touchHandleExist;
  }
  bool IsTouchHandleExist() {
    return touch_handle_exist_;
  }
  void SetViewportScaleState(bool viewportScale) {
    viewport_scale_ = viewportScale;
  }
  bool IsViewportScale() {
    return viewport_scale_;
  }
#endif  // BUILDFLAG(ARKWEB_MENU)

#endif