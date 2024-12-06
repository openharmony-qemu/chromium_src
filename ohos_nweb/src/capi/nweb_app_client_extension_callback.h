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

#include <map>
#include <stddef.h>
#include <string>

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
  void (*OnHidePasswordAutofillPopup)(int nweb_id);
  void (*OnSaveOrUpdatePassword)(bool isUpdate, std::string url, int nweb_id);
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
#ifdef OHOS_ARKWEB_ADBLOCK
  void (*OnAdsBlocked)(std::string url,
                       std::map<std::string, int32_t> adsBlocked,
                       int nweb_id);

  bool (*TrigAdBlockEnabledForSiteFromUi)(std::string url,
                                          int main_frame_tree_node_id,
                                          int nweb_id);
#endif

  void (*OnRequestOpenDevTools)(int32_t nweb_id);
};

#endif  // OHOS_NWEB_SRC_NWEB_APP_CLIENT_EXTENSION_CALLBACK_H_
