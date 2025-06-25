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

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_FRAME_SETTINGS_EXT_PRIVATE_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_FRAME_SETTINGS_EXT_PRIVATE_H_

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_FRAME_SETTINGS_H_
#error "must be in include form THIRD_PARTY_BLINK_RENDERER_CORE_FRAME_SETTINGS_H_"
#endif

#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
  bool hide_vertical_scrollbars_ = true;
  bool hide_horizontal_scrollbars_ = true;
  bool scroll_enabled_ = true;
  int overscroll_mode_ = 0;
#endif  // BUILDFLAG(ARKWEB_INPUT_EVENTS)

#if BUILDFLAG(ARKWEB_SAME_LAYER)
  std::map<WebString, WebString> embed_rule_;
#endif
#if BUILDFLAG(ARKWEB_EXT_FREE_COPY)
  bool contextmenu_customization_enabled_ = false;
#endif  // ARKWEB_EXT_FREE_COPY

#if BUILDFLAG(ARKWEB_CUSTOM_VIDEO_PLAYER)
  bool custom_video_player_enabled_ = false;
  bool custom_video_player_overlay_ = false;
#endif  // ARKWEB_CUSTOM_VIDEO_PLAYER

#if BUILDFLAG(ARKWEB_MEDIA_CAPABILITIES_ENHANCE)
  int32_t usage_scenario_ = 1;
#endif  // ARKWEB_MEDIA_CAPABILITIES_ENHANCE

#if BUILDFLAG(ARKWEB_COPY_OPTION)
  mojom::CopyOptionMode copy_option_ = mojom::CopyOptionMode::CROSS_DEVICE;
#endif  // BUILDFLAG(ARKWEB_COPY_OPTION)

#if BUILDFLAG(ARKWEB_SCROLLBAR)
  uint32_t scrollbar_color_ = 0;
#endif

#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
  bool video_assistant_enabled_ = false;
  bool custom_media_player_enabled_ = false;
#endif  // ARKWEB_VIDEO_ASSISTANT

#if BUILDFLAG(ARKWEB_SCROLLBAR_AVOID_CORNER)
  double border_radius_top_left_ = 0.0;
  double border_radius_top_right_ = 0.0;
  double border_radius_bottom_left_ = 0.0;
  double border_radius_bottom_right_ = 0.0;
#endif  // ARKWEB_SCROLLBAR_AVOID_CORNER

#if BUILDFLAG(ARKWEB_ERROR_PAGE)
  bool error_page_enabled_ = false;
#endif

#endif