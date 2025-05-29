/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef OHOS_NWEB_SRC_NWEB_APP_CLIENT_EXTENSION_CALLBACK_H_
#define OHOS_NWEB_SRC_NWEB_APP_CLIENT_EXTENSION_CALLBACK_H_

#include <stddef.h>

#include <map>
#include <string>

#include "arkweb/build/features/features.h"
#include "build/build_config.h"
#include "ohos_nweb/src/capi/nweb_native_embed_first_frame_paint_event.h"
#include "ohos_nweb/src/capi/nweb_permission_request.h"

#if BUILDFLAG(IS_ARKWEB_EXT)
#include "arkweb/ohos_nweb_ex/build/features/features.h"
#endif

#if BUILDFLAG(ARKWEB_NAVIGATION)
#include "arkweb/ohos_nweb/src/capi/nweb_icon_size.h"
#endif

class NWebMediaPlayerController;
class NWebMediaPlayerListener;

struct NWebAppClientExtensionCallback {
  size_t struct_size = sizeof(NWebAppClientExtensionCallback);
  int nweb_id{0};
  void (*OnReceivedFaviconUrl)(const char* image_url,
                               size_t width,
                               size_t height,
                               int color_type,
                               int alpha_type,
                               int nweb_id);
  void (*OnLoadStarted)(bool toDifferentDocument, int nweb_id);
  void (*OnActivityStateChanged)(int state, int type, int nweb_id);
  void (*OnOpenURLFromTab)(std::string target_url,
                           int type,
                           bool user_gesture,
                           int nweb_id);
  void (*OnHidePasswordAutofillPopup)(int nweb_id);
  void (*OnSaveOrUpdatePassword)(bool isUpdate, std::string url, int nweb_id);
  void (*OnUpdateTargetURL)(std::string url, int nweb_id);
  void (*OnShowPasswordAutofillPopup)(int left,
                                      int top,
                                      int width,
                                      int height,
                                      bool is_rtl,
                                      std::vector<std::string> labels,
                                      std::vector<std::string> sublabels,
                                      int nweb_id);
  int (*OnGetTopControlsHeight)(int nweb_id);
  void (*OnTopControlsChanged)(float top_controls_offset,
                               float top_content_offset,
                               int nweb_id);
  void (*ContentsBrowserZoomChange)(double zoom_factor,
                                    bool can_show_bubble,
                                    int nweb_id);
  void (*OnPermissionRequest)(int nweb_id,
                              std::shared_ptr<NWebPermissionRequest> request);
  void (*OnAdsBlocked)(std::string url,
                       std::map<std::string, int32_t> adsBlocked,
                       int nweb_id);
  bool (*TrigAdBlockEnabledForSiteFromUi)(std::string url,
                                          int main_frame_tree_node_id,
                                          int nweb_id);
  void (*OnIntelligentTrackingPreventionResult)(const std::string& host,
                                                const std::string& trackerHost,
                                                int nweb_id);
  void (*OnShareFile)(const std::string& filePath,
                      const std::string& utdTypeId,
                      int nweb_id);
  void (*OnNativeEmbedFirstFramePaint)(
      int32_t nweb_id,
      std::shared_ptr<NWebNativeEmbedFirstFramePaintEvent> request);

  void (*OnRequestOpenDevTools)(int32_t nweb_id);

#if BUILDFLAG(ARKWEB_MULTI_WINDOW)
  void (*OnActivateContent)(int nweb_id);
#endif

#if BUILDFLAG(ARKWEB_PULL_TO_REFRESH)
  bool (*OnPullToRefreshAction)(int action, int nweb_id);
  void (*OnPullToRefreshPull)(float offset_x, float offset_y, int nweb_id);
#endif
#if BUILDFLAG(ARKWEB_DISATCH_BEFORE_UNLOAD)
  void (*OnBeforeUnloadFired)(bool proceed, int nweb_id);
#endif  // ARKWEB_DISATCH_BEFORE_UNLOAD
#if BUILDFLAG(ARKWEB_NAVIGATION)
  void (*OnTouchIconUrlWithSizesReceived)(const char* image_url,
                                          bool precomposed,
                                          const IconSize* sizes,
                                          size_t sizes_count,
                                          int nweb_id);
#endif

#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
  NWebMediaPlayerListener* (*OnFullScreenOverlayEnter)(int32_t nweb_id,
      NWebMediaPlayerController* media_player_controller,
      const char* media_info);
#endif // ARKWEB_VIDEO_ASSISTANT

  void (*OnShowToast)(int32_t nweb_id, double duration, const char* toast);

  void (*OnShowVideoAssistant)(int32_t nweb_id,
                               const char* video_assistant_items);
#if BUILDFLAG(ARKWEB_PIP)
  void (*OnPipEvent)(int event, int nweb_id);
#endif
};

#endif  // OHOS_NWEB_SRC_NWEB_APP_CLIENT_EXTENSION_CALLBACK_H_
