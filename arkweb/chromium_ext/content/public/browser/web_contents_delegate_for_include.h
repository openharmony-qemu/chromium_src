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

#ifndef ARKWEB_CHROMIUM_EXT_CONTENT_PUBLIC_BROWSER_WEB_CONTENTS_DELEGATE_FOR_INCLUDE_H_
#define ARKWEB_CHROMIUM_EXT_CONTENT_PUBLIC_BROWSER_WEB_CONTENTS_DELEGATE_FOR_INCLUDE_H_

#if BUILDFLAG(ARKWEB_ADBLOCK)
virtual void OnAdsBlocked(
    const std::string& main_frame_url,
    const std::map<std::string, int32_t>& subresource_blocked,
    bool is_site_first_report) {}

virtual bool TrigAdBlockEnabledForSiteFromUi(const std::string& main_frame_url,
                                             int main_frame_tree_node_id) {
  return false;
}
#endif

#if BUILDFLAG(ARKWEB_NETWORK_LOAD)
virtual void OnShareFile(const std::string& filePath,
                         const std::string& utdTypeId) {}
#endif

#if BUILDFLAG(ARKWEB_MULTI_WINDOW)
virtual void OnActivateContent() {}
#endif

#if BUILDFLAG(ARKWEB_ARKWEB_EXTENSIONS)
virtual void WebExtensionUpdateTab(
    int32_t tab_id,
    const NWebExtensionTabUpdateProperties* update_properties);
virtual void WebExtensionUpdateTabUrl(int32_t tab_id, const GURL& url) {}
virtual int32_t ExtensionGetTabId() const {
  return -1;
}
#endif

#if BUILDFLAG(ARKWEB_DRAG_DROP)
virtual void ClearContextMenu() {}
#endif  // BUILDFLAG(ARKWEB_DRAG_DROP)

#if BUILDFLAG(ARKWEB_SAME_LAYER)
virtual void OnNativeEmbedStatusUpdate(const NativeEmbedInfo& native_embed_info,
                                       NativeEmbedInfo::TagState state) {}
virtual void OnNativeEmbedFirstFramePaint(
    int32_t native_embed_id,
    const std::string& embed_id_attribute) {}
virtual void OnLayerRectVisibilityChange(const std::string& embed_id,
                                         bool visibility) {}
#endif

#if BUILDFLAG(ARKWEB_BFCACHE)
#define ARK_WEB_BFCACHE_DEFAULT_CACHE_TTL 600
#define ARK_WEB_BFCACHE_DEFAULT_CACHE_SIZE -1

// Return back_forward_cache time to live.
virtual int BackForwardCacheTimeToLive() {
  return ARK_WEB_BFCACHE_DEFAULT_CACHE_TTL;
}

// Return back_forward_cache max cache size.
virtual int BackForwardCacheSize() {
  return ARK_WEB_BFCACHE_DEFAULT_CACHE_SIZE;
}
#endif

#if BUILDFLAG(ARKWEB_CUSTOM_VIDEO_PLAYER)
virtual std::unique_ptr<CustomMediaPlayer> CreateCustomMediaPlayer(
    std::unique_ptr<CustomMediaPlayerListener> listener,
    const MediaInfo& media_info);
#endif  // ARKWEB_CUSTOM_VIDEO_PLAYER

#if BUILDFLAG(ARKWEB_DISATCH_BEFORE_UNLOAD)
virtual void OnBeforeUnloadFired(bool proceed) {}
#endif  // ARKWEB_DISATCH_BEFORE_UNLOAD

#if BUILDFLAG(ARKWEB_DATALIST)
virtual void OnShowAutofillPopup(
    const gfx::RectF& element_bounds,
    bool is_rtl,
    const std::vector<autofill::Suggestion>& suggestions,
    bool is_password_popup_type) {}
virtual void OnHideAutofillPopup() {}
#endif

#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
virtual std::unique_ptr<VideoAssistant> CreateVideoAssistant();
virtual void PopluateVideoAssistantConfig(
    const std::string& url,
    media::mojom::VideoAssistantConfigPtr& config);
virtual void OnVideoPlaying(
    media::mojom::VideoAttributesForVASTPtr video_attributes);
virtual void OnUpdateVideoAttributes(
    media::mojom::VideoAttributesForVASTPtr video_attributes);

virtual void OnShowToast(double duration, const std::string& toast);
virtual void OnShowVideoAssistant(const std::string& videoAssistantItems);
virtual void OnReportStatisticLog(const std::string& content);

virtual std::unique_ptr<MediaPlayerListener> OnFullScreenOverlayEnter(
    media::mojom::MediaInfoForVASTPtr media_info,
    const MediaPlayerId& media_player_id);
#endif  // ARKWEB_VIDEO_ASSISTANT

#endif  // ARKWEB_CHROMIUM_EXT_CONTENT_PUBLIC_BROWSER_WEB_CONTENTS_DELEGATE_FOR_INCLUDE_H_
