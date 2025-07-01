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

#ifndef ARKWEB_WEB_SETTINGS_EXT_H_
#define ARKWEB_WEB_SETTINGS_EXT_H_

#include <unicode/uscript.h>

#include "arkweb/build/features/features.h"
#include "build/build_config.h"
#if BUILDFLAG(IS_ARKWEB_EXT)
#include "arkweb/ohos_nweb_ex/build/features/features.h"
#endif
#include "third_party/blink/public/common/css/navigation_controls.h"
#include "third_party/blink/public/common/web_preferences/web_preferences.h"
#include "third_party/blink/public/mojom/v8_cache_options.mojom-forward.h"
#include "third_party/blink/public/mojom/webpreferences/web_preferences.mojom-forward.h"
#include "third_party/blink/public/platform/web_common.h"
#include "third_party/blink/public/platform/web_effective_connection_type.h"

namespace blink {

enum class LCDTextPreference;
class WebString;

class WebSettingsArkwebInterface {
public:
#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
  virtual void SetVerticalHideScrollbars(bool) = 0;
  virtual void SetHorizontalHideScrollbars(bool) = 0;
  virtual void SetScrollable(bool) = 0;
#endif  // BUILDFLAG(ARKWEB_INPUT_EVENTS)
#if BUILDFLAG(ARKWEB_EXT_FREE_COPY)
  virtual void SetContextMenuCustomization(bool) = 0;
#endif
#if BUILDFLAG(ARKWEB_SCROLLBAR)
  virtual void SetScrollBarColor(uint32_t) = 0;
#endif  // ARKWEB_SCROLLBAR
#if BUILDFLAG(ARKWEB_SAME_LAYER)
  virtual void SetNativeEmbedModeEnabled(bool) = 0;
  virtual void SetIntrinsicSizeEnabled(bool) = 0;
  virtual void SetCssDisplayChangeEnabled(bool) = 0;
  virtual void RegisterNativeEmbedRule(const WebString&, const WebString&) = 0;
#endif

#if BUILDFLAG(ARKWEB_SYNC_RENDER)
  virtual void SetDrawMode(int) = 0;
#endif

#if BUILDFLAG(ARKWEB_ZOOM)
  virtual void SetTextZoomFactor(float factor) = 0;
#endif

#if BUILDFLAG(ARKWEB_CSS_FONT)
  virtual void SetFontWeightScale(float) = 0;
#endif

#if BUILDFLAG(ARKWEB_CUSTOM_VIDEO_PLAYER)
  virtual void SetCustomVideoPlayerEnabled(bool enable) = 0;
  virtual void SetCustomVideoPlayerOverlay(bool overlay) = 0;
#endif  // ARKWEB_CUSTOM_VIDEO_PLAYER

#if BUILDFLAG(ARKWEB_COPY_OPTION)
  virtual void SetCopyOption(mojom::CopyOptionMode) = 0;
#endif  // BUILDFLAG(ARKWEB_COPY_OPTION)

#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
  virtual void SetVideoAssistantEnabled(bool enabled) {}
  virtual void SetCustomMediaPlayerEnabled(bool enabled) {}
  virtual bool GetCustomMediaPlayerEnabled() { return false; }
#endif  // ARKWEB_VIDEO_ASSISTANT

#if BUILDFLAG(ARKWEB_MEDIA_CAPABILITIES_ENHANCE)
  virtual void SetUsageScenario(int32_t usage_scenario) = 0;
#endif  // ARKWEB_MEDIA_CAPABILITIES_ENHANCE

#if BUILDFLAG(ARKWEB_SCROLLBAR_AVOID_CORNER)
  virtual void SetBorderRadiusFromWeb(double, double, double, double) = 0;
  virtual double GetBorderRadiusTopLeft() = 0;
  virtual double GetBorderRadiusTopRight() = 0;
  virtual double GetBorderRadiusBottomLeft() = 0;
  virtual double GetBorderRadiusBottomRight() = 0;
#endif  // ARKWEB_SCROLLBAR_AVOID_CORNER

#if BUILDFLAG(ARKWEB_MENU)
  virtual void SetTouchHandleExistState(bool) = 0;
  virtual bool IsTouchHandleExist() = 0;
  virtual void SetViewportScaleState(bool) = 0;
  virtual bool IsViewportScale() = 0;
#endif  // BUILDFLAG(ARKWEB_MENU)

protected:
    virtual ~WebSettingsArkwebInterface() = default;
};

}  // namespace blink

#endif ARKWEB_WEB_SETTINGS_EXT_H_