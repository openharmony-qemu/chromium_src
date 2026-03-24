
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

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_EXPORTED_WEB_SETTINGS_IMPL_EXT_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_EXPORTED_WEB_SETTINGS_IMPL_EXT_H_

#include "arkweb/build/features/features.h"
#include "third_party/blink/public/mojom/v8_cache_options.mojom-blink.h"
#include "third_party/blink/public/mojom/webpreferences/web_preferences.mojom-blink.h"
#include "third_party/blink/public/web/web_settings.h"
#include "third_party/blink/renderer/core/core_export.h"
#include "third_party/blink/renderer/core/exported/web_settings_impl.h"
#include "third_party/blink/renderer/platform/heap/persistent.h"

namespace blink {
class DevToolsEmulator;
class WebSettingsImpl;
class Settings;

class CORE_EXPORT WebSettingsImplExt final : public WebSettingsImpl {
 public:
  WebSettingsImplExt(Settings*, DevToolsEmulator*);
  ~WebSettingsImplExt() override;
  WebSettingsImplExt* AsWebSettingsImplExt() override { return this; }

#if BUILDFLAG(ARKWEB_SCROLLBAR)
  void SetScrollBarColor(uint32_t) override;
#endif  // ARKWEB_SCROLLBAR
#if BUILDFLAG(ARKWEB_AI)
  void SetImageAnalyzerEnabled(bool) override;
  void SetArkwebAgentEnabled(bool) override;
  void SetAgentNeedHighlight(bool) override;
#endif  // BUILDFLAG(ARKWEB_AI)
#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
  void SetVerticalHideScrollbars(bool) override;
  void SetHorizontalHideScrollbars(bool) override;
  void SetScrollable(bool) override;
  void SetScrollbarLayoutPolicy(int) override;
  int GetScrollbarLayoutPolicy() override;
  void SetIsSystemRtlEnable(bool) override;
  bool GetIsSystemRtlEnabled() override;
#endif

#if BUILDFLAG(ARKWEB_ZOOM)
  void SetTextZoomFactor(float factor) override;
#endif

#if BUILDFLAG(ARKWEB_COPY_OPTION)
  void SetCopyOption(mojom::blink::CopyOptionMode value) override;
#endif  // BUILDFLAG(ARKWEB_COPY_OPTION)

#if BUILDFLAG(ARKWEB_FOCUS)
  void SetGestureFocusMode(int) override;
#endif

#if BUILDFLAG(ARKWEB_EXT_FREE_COPY)
  void SetContextMenuCustomization(bool) override;
#endif

#if BUILDFLAG(ARKWEB_SYNC_RENDER)
  void SetDrawMode(int) override;
#endif

#if BUILDFLAG(ARKWEB_CSS_FONT)
  void SetFontWeightScale(float) override;
#endif

#if BUILDFLAG(ARKWEB_SAME_LAYER)
  void SetNativeEmbedModeEnabled(bool) override;
  bool NativeEmbedModeEnabled() const;
  void SetIntrinsicSizeEnabled(bool) override;
  void SetCssDisplayChangeEnabled(bool) override;
  void RegisterNativeEmbedRule(const WebString& tag,
                               const WebString& type) override;
#endif

#if BUILDFLAG(ARKWEB_CUSTOM_VIDEO_PLAYER)
  void SetCustomVideoPlayerEnabled(bool enable) override;
  void SetCustomVideoPlayerOverlay(bool overlay) override;
#endif  // ARKWEB_CUSTOM_VIDEO_PLAYER

#if BUILDFLAG(ARKWEB_MEDIA_CAPABILITIES_ENHANCE)
  void SetUsageScenario(int32_t usage_scenario) override;
#endif  // ARKWEB_MEDIA_CAPABILITIES_ENHANCE

#if BUILDFLAG(ARKWEB_LOGGER_REPORT)
  int32_t GetUsageScenario() override;
#endif

#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
  void SetVideoAssistantEnabled(bool enabled) override;
  void SetCustomMediaPlayerEnabled(bool enabled) override;
  bool GetCustomMediaPlayerEnabled() override;
#endif  // ARKWEB_VIDEO_ASSISTANT

#if BUILDFLAG(ARKWEB_BFCACHE)
  void SetMediaResumeFromBFCachePage(bool resume) override;
  bool GetMediaResumeFromBFCachePage() override;
#endif  // BUILDFLAG(ARKWEB_BFCACHE)

#if BUILDFLAG(ARKWEB_SCROLLBAR_AVOID_CORNER)
  void SetBorderRadiusFromWeb(double borderRadiusTopLeft,
                              double borderRadiusTopRight,
                              double borderRadiusBottomLeft,
                              double borderRadiusBottomRight) override;
  double GetBorderRadiusTopLeft() override;
  double GetBorderRadiusTopRight() override;
  double GetBorderRadiusBottomLeft() override;
  double GetBorderRadiusBottomRight() override;
#endif  // ARKWEB_SCROLLBAR_AVOID_CORNER

#if BUILDFLAG(ARKWEB_PASSWORD_AUTOFILL)
  void SetEnableAutoFill(bool enable) override;
  bool GetEnableAutoFill() override;
#endif  // BUILDFLAG(ARKWEB_PASSWORD_AUTOFILL)

#if BUILDFLAG(ARKWEB_CLIPBOARD)
  void SetClipboardSitePermissionEnabled(bool enable) override;
#endif  // BUILDFLAG(ARKWEB_CLIPBOARD)

#if BUILDFLAG(ARKWEB_MENU)
  void SetTouchHandleExistState(bool touchHandleExist) override;
  bool IsTouchHandleExist() override;
  void SetViewportScaleState(bool viewportScale) override;
  bool IsViewportScale() override;
#endif  // BUILDFLAG(ARKWEB_MENU)

#if BUILDFLAG(ARKWEB_MEDIA_CAST)
  void SetCastEnabled(bool enable) override;
#endif  // BUILDFLAG(ARKWEB_MEDIA_CAST)
};
}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_EXPORTED_WEB_SETTINGS_IMPL_EXT_H_
