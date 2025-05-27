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

#ifndef CONTENT_PUBLIC_BROWSER_WEB_CONTENTS_FOR_INCLUDE_H_
#define CONTENT_PUBLIC_BROWSER_WEB_CONTENTS_FOR_INCLUDE_H_

#if BUILDFLAG(ARKWEB_USERAGENT)
virtual void SetCustomUA(std::string custom_user_agent) = 0;
virtual std::string GetCustomUA() = 0;
#endif

#if BUILDFLAG(IS_ARKWEB)
// creating two ends of a message channel.
// @param ports the web message ports get from nweb.
virtual void CreateWebMessagePorts(
    std::vector<blink::WebMessagePort>& ports) = 0;

// Posts MessageEvent to the main frame.
// @param message message send to mmain frame.
// @param ports the web message ports send to main frame.
// @param targetUri the uri which can received the ports.
virtual void PostWebMessage(std::string& message,
                            std::vector<blink::WebMessagePort>& ports,
                            std::string& targetUri) = 0;
#if BUILDFLAG(ARKWEB_CSS_INPUT_TIME)
virtual void OpenDateTimeChooser() = 0;
virtual void CloseDateTimeChooser() = 0;
#endif  // #if BUILDFLAG(ARKWEB_CSS_INPUT_TIME)
#endif

#if BUILDFLAG(ARKWEB_CLIPBOARD)
virtual void CollapseAllFramesSelection() = 0;
#endif

#if BUILDFLAG(ARKWEB_EXT_FORCE_ZOOM)
virtual void SetForceEnableZoom(bool forceEnableZoom) = 0;
virtual bool GetForceEnableZoom() = 0;
#endif  // ARKWEB_EXT_FORCE_ZOOM

#if BUILDFLAG(ARKWEB_CLIPBOARD)
virtual void SetTouchInsertHandleMenuShow(bool show) = 0;
virtual bool GetTouchInsertHandleMenuShow() = 0;
#endif

#if BUILDFLAG(ARKWEB_ADBLOCK)
virtual void TrigAdBlockEnabledForSiteFromUi(
    const std::string& main_frame_url) = 0;

virtual void EnableAdsBlock(bool enable) = 0;

virtual bool IsAdsBlockEnabled() = 0;
virtual bool IsAdsBlockEnabledForCurPage() = 0;

virtual void OnAdsBlocked(
    const std::string& main_frame_url,
    const std::map<std::string, int32_t>& subresource_blocked,
    bool is_site_first_report) = 0;

virtual void UpdateAdBlockEnabledToRender(bool site_adblock_enabled) = 0;

virtual bool GetAdblockEnabledForSite() = 0;

virtual void SetAdBlockEnabledForSite(bool is_adblock_enabled,
                                      int main_frame_tree_node_id) = 0;
#endif

#if BUILDFLAG(ARKWEB_DATALIST)
virtual void ShowAutofillPopup(
    const gfx::RectF& element_bounds,
    bool is_rtl,
    const std::vector<autofill::Suggestion>& suggestions,
    bool is_password_popup_type) = 0;
virtual void HideAutofillPopup() = 0;
#endif

#if BUILDFLAG(ARKWEB_WEBRTC)
virtual void StartCamera(int nWebID) = 0;
virtual void StopCamera(int nWebID) = 0;
virtual void CloseCamera(int nWebID) = 0;
virtual int GetNWebId() = 0;
virtual void SetNWebId(int nWebID) = 0;
#endif  // BUILDFLAG(ARKWEB_WEBRTC)

#if BUILDFLAG(ARKWEB_EXT_FREE_COPY)
virtual void ShowFreeCopyMenu() = 0;
virtual bool ShouldShowFreeCopyMenu() = 0;
#endif

#if BUILDFLAG(ARKWEB_NETWORK_LOAD)
virtual void OnShareFile(const std::string& filePath,
                         const std::string& utdTypeId) = 0;
#endif

#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
virtual void EnableVideoAssistant(bool enable) = 0;
virtual void ExecuteVideoAssistantFunction(const std::string& cmdId) = 0;
virtual void CustomWebMediaPlayer(bool enable) = 0;
#endif  // ARKWEB_VIDEO_ASSISTANT

#if BUILDFLAG(ARKWEB_MEDIA_POLICY)
// Set whether to the HTML play can be used to control media
virtual void SetHtmlPlayEnabled(bool enabled) = 0;
virtual bool IsHtmlPlayEnabled() = 0;
#endif  // BUILDFLAG(ARKWEB_MEDIA_POLICY)

#if BUILDFLAG(ARKWEB_SCREEN_LOCK)
virtual void SetWakeLockHandler(int32_t windowId,
                                const SetKeepScreenOn& handler) = 0;
#endif  // BUILDFLAG(ARKWEB_SCREEN_LOCK)

#if BUILDFLAG(ARKWEB_ARKWEB_EXTENSIONS)
virtual void WebExtensionUpdateTab(
    int32_t tab_id,
    const NWebExtensionTabUpdateProperties* update_properties) = 0;
virtual void WebExtensionUpdateTabUrl(int32_t tab_id, const GURL& url) = 0;
virtual int32_t ExtensionGetTabId() const = 0;
#endif

#if BUILDFLAG(ARKWEB_PERFORMANCE_PERSISTENT_TASK)
virtual void OneShotMediaPlayerStopped() = 0;
#endif
#if BUILDFLAG(ARKWEB_DRAG_DROP)
virtual void ClearContextMenu() = 0;
#endif  // BUILDFLAG(ARKWEB_DRAG_DROP)

#if BUILDFLAG(ARKWEB_SAFEBROWSING)
virtual void EnableSafeBrowsingDetection(bool enable, bool strictMode) = 0;
virtual bool IsSafeBrowsingDetectionConfig() = 0;
virtual bool IsSafeBrowsingDetectionStrict() = 0;
virtual bool IsSafeBrowsingDetectionDisabled() = 0;
#endif  // BUILDFLAG(ARKWEB_SAFEBROWSING)

#if BUILDFLAG(ARKWEB_ACTIVITY_STATE)
virtual void OnFormEditingStateChanged(uint64_t form_id, bool did_submit) = 0;
#endif

#if BUILDFLAG(ARKWEB_EXT_TOPCONTROLS)
virtual void UpdateBrowserControlsHeight(int, bool) = 0;
#endif

#if BUILDFLAG(ARKWEB_DISATCH_BEFORE_UNLOAD)
virtual void OnBeforeUnloadFired(bool proceed) = 0;
#endif  // ARKWEB_DISATCH_BEFORE_UNLOAD
#if BUILDFLAG(ARKWEB_MEDIA_AVSESSION)
void SetMediaTitle(const std::string& data) {
  media_title_ = data;
}

std::string GetMediaTitle() {
  return media_title_;
}

void SetVideoPoster(const std::string& data) {
  video_poster_ = data;
}

std::string GetVideoPoster() {
  return video_poster_;
}
#endif  // ARKWEB_MEDIA_AVSESSION

#if BUILDFLAG(ARKWEB_EX_SCREEN_CAPTURE)
virtual void StopScreenCapture(int32_t nweb_id,
                               const std::string& session_id) = 0;

virtual void SetScreenCapturePickerShow() = 0;

virtual void DisableSessionReuse() = 0;
#endif  // defined(ARKWEB_EX_SCREEN_CAPTURE)

private:
#if BUILDFLAG(ARKWEB_MEDIA_AVSESSION)
std::string media_title_;
std::string video_poster_;
#endif  // ARKWEB_MEDIA_AVSESSION

#endif  // CONTENT_PUBLIC_BROWSER_WEB_CONTENTS_FOR_INCLUDE_H_