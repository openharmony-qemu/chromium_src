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

#include "arkweb/chromium_ext/third_party/blink/renderer/core/exported/web_settings_impl_ext.h"

#include "arkweb/build/features/features.h"
#include "third_party/blink/renderer/core/frame/settings.h"
#include "third_party/blink/renderer/core/inspector/dev_tools_emulator.h"
#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
#include "third_party/blink/renderer/modules/media_controls/media_controls_resource_loader.h"
#endif

namespace blink {

// LCOV_EXCL_START
WebSettingsImplExt::WebSettingsImplExt(Settings* settings,
                                       DevToolsEmulator* dev_tools_emulator)
    : WebSettingsImpl(settings, dev_tools_emulator) {}

WebSettingsImplExt::~WebSettingsImplExt() {}

#if BUILDFLAG(ARKWEB_SCROLLBAR)
void WebSettingsImplExt::SetScrollBarColor(uint32_t value) {
  settings_->SetScrollBarColor(value);
}
#endif  // ARKWEB_SCROLLBAR

#if BUILDFLAG(ARKWEB_AI)
void WebSettingsImplExt::SetImageAnalyzerEnabled(bool enabled) {
  settings_->SetImageAnalyzerEnabled(enabled);
}
#endif

#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
void WebSettingsImplExt::SetVerticalHideScrollbars(bool enabled) {
  settings_->SetVerticalHideScrollbars(enabled);
}

void WebSettingsImplExt::SetHorizontalHideScrollbars(bool enabled) {
  settings_->SetHorizontalHideScrollbars(enabled);
}

void WebSettingsImplExt::SetScrollable(bool enabled) {
  settings_->SetScrollable(enabled);
}
#endif

#if BUILDFLAG(ARKWEB_ZOOM)
void WebSettingsImplExt::SetTextZoomFactor(float factor) {
  settings_->SetTextZoomFactor(factor);
}
#endif

#if BUILDFLAG(ARKWEB_COPY_OPTION)
void WebSettingsImplExt::SetCopyOption(mojom::blink::CopyOptionMode value) {
  settings_->SetCopyOption(value);
}
#endif  // BUILDFLAG(ARKWEB_COPY_OPTION)

#if BUILDFLAG(ARKWEB_FOCUS)
void WebSettingsImplExt::SetGestureFocusMode(int mode) {
  settings_->SetGestureFocusMode(mode);
}
#endif

#if BUILDFLAG(ARKWEB_CSS_FONT)
void WebSettingsImplExt::SetFontWeightScale(float value) {
  settings_->SetFontWeightScale(value);
}
#endif

#if BUILDFLAG(ARKWEB_SYNC_RENDER)
void WebSettingsImplExt::SetDrawMode(int mode) {
  settings_->SetDrawMode(mode);
}
#endif

#if BUILDFLAG(ARKWEB_EXT_FREE_COPY)
void WebSettingsImplExt::SetContextMenuCustomization(bool enabled) {
  settings_->SetContextMenuCustomization(enabled);
}
#endif

#if BUILDFLAG(ARKWEB_SAME_LAYER)
void WebSettingsImplExt::SetNativeEmbedModeEnabled(bool enabled) {
  settings_->SetNativeEmbedModeEnabled(enabled);
}

bool WebSettingsImplExt::NativeEmbedModeEnabled() const {
  return settings_->GetNativeEmbedModeEnabled();
}

void WebSettingsImplExt::SetIntrinsicSizeEnabled(bool enabled) {
  settings_->SetIntrinsicSizeEnabled(enabled);
}

void WebSettingsImplExt::SetCssDisplayChangeEnabled(bool enabled) {
  settings_->SetCSSDisplayChangeEnabled(enabled);
}

void WebSettingsImplExt::RegisterNativeEmbedRule(const WebString& tag,
                                              const WebString& type) {
  settings_->RegisterNativeEmbedRule(tag, type);
}
#endif

#if BUILDFLAG(ARKWEB_CUSTOM_VIDEO_PLAYER)
void WebSettingsImplExt::SetCustomVideoPlayerEnabled(bool enable) {
  settings_->SetCustomVideoPlayerEnabled(enable);
}
void WebSettingsImplExt::SetCustomVideoPlayerOverlay(bool overlay) {
  settings_->SetCustomVideoPlayerOverlay(overlay);
}
#endif  // ARKWEB_CUSTOM_VIDEO_PLAYER

#if BUILDFLAG(ARKWEB_MEDIA_CAPABILITIES_ENHANCE)
void WebSettingsImplExt::SetUsageScenario(int32_t usage_scenario) {
  settings_->SetUsageScenario(usage_scenario);
}
#endif  // ARKWEB_MEDIA_CAPABILITIES_ENHANCE

#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
void WebSettingsImplExt::SetVideoAssistantEnabled(bool enabled) {
  settings_->SetVideoAssistantEnabled(enabled);
}
void WebSettingsImplExt::SetCustomMediaPlayerEnabled(bool enabled) {
  settings_->SetCustomMediaPlayerEnabled(enabled);
  MediaControlsResourceLoader::SetCustomMediaPlayerEnabled(enabled);
}
bool WebSettingsImplExt::GetCustomMediaPlayerEnabled() {
  return settings_->GetCustomMediaPlayerEnabled();
}
#endif  // ARKWEB_VIDEO_ASSISTANT

#if BUILDFLAG(ARKWEB_BFCACHE)
void WebSettingsImplExt::SetMediaResumeFromBFCachePage(bool resume) {
  settings_->SetMediaResumeFromBFCachePage(resume);
}

bool WebSettingsImplExt::GetMediaResumeFromBFCachePage() {
  return settings_->GetMediaResumeFromBFCachePage();
}
#endif  // BUILDFLAG(ARKWEB_BFCACHE)

#if BUILDFLAG(ARKWEB_LOGGER_REPORT)
int32_t WebSettingsImplExt::GetUsageScenario() {
  return settings_->GetUsageScenario();
}
#endif

#if BUILDFLAG(ARKWEB_SCROLLBAR_AVOID_CORNER)
void WebSettingsImplExt::SetBorderRadiusFromWeb(
    double borderRadiusTopLeft,
    double borderRadiusTopRight,
    double borderRadiusBottomLeft,
    double borderRadiusBottomRight)
{
  settings_->SetBorderRadiusFromWeb(borderRadiusTopLeft, borderRadiusTopRight,
                                    borderRadiusBottomLeft,
                                    borderRadiusBottomRight);
}
double WebSettingsImplExt::GetBorderRadiusTopLeft()
{
  return settings_->GetBorderRadiusTopLeft();
}
double WebSettingsImplExt::GetBorderRadiusTopRight()
{
  return settings_->GetBorderRadiusTopRight();
}
double WebSettingsImplExt::GetBorderRadiusBottomLeft()
{
  return settings_->GetBorderRadiusBottomLeft();
}
double WebSettingsImplExt::GetBorderRadiusBottomRight()
{
  return settings_->GetBorderRadiusBottomRight();
}
#endif  // ARKWEB_SCROLLBAR_AVOID_CORNER

#if BUILDFLAG(ARKWEB_PASSWORD_AUTOFILL)
void WebSettingsImplExt::SetEnableAutoFill(bool enable)
{
  settings_->SetEnableAutoFill(enable);
}

bool  WebSettingsImplExt::GetEnableAutoFill()
{
  return settings_->GetEnableAutoFill();
}
#endif  // BUILDFLAG(ARKWEB_PASSWORD_AUTOFILL)

#if BUILDFLAG(ARKWEB_CLIPBOARD)
void WebSettingsImplExt::SetClipboardSitePermissionEnabled(bool enable)
{
  settings_->SetClipboardSitePermissionEnabled(enable);
}
#endif  // BUILDFLAG(ARKWEB_CLIPBOARD)

#if BUILDFLAG(ARKWEB_MENU)
void WebSettingsImplExt::SetTouchHandleExistState(bool touchHandleExist)
{
  settings_->SetTouchHandleExistState(touchHandleExist);
}

bool WebSettingsImplExt::IsTouchHandleExist()
{
  return settings_->IsTouchHandleExist();
}

void WebSettingsImplExt::SetViewportScaleState(bool viewportScale)
{
  settings_->SetViewportScaleState(viewportScale);
}

bool WebSettingsImplExt::IsViewportScale()
{
  return settings_->IsViewportScale();
}
#endif  // BUILDFLAG(ARKWEB_MENU)
// LCOV_EXCL_STOP

}  // namespace blink
