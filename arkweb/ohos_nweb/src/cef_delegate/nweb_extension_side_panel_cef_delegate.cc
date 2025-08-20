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
 
#include "nweb_extension_side_panel_cef_delegate.h"
 
#include "base/logging.h"
#include "content/public/browser/browser_context.h"
#include "cef/libcef/browser/request_context_impl.h"
#include "nweb_extension_utils.h"

#if BUILDFLAG(ARKWEB_NWEB_EX)
#include "ohos_nweb_ex/core/extension/nweb_extension_side_panel_dispatcher.h"
#endif
 
namespace OHOS::NWeb {
#if BUILDFLAG(ARKWEB_NWEB_EX)
namespace {
  static std::map<int, SidePanelOnOpenCallback> g_sidepanel_open_map_;
  static std::map<int, SidePanelOnSetOptionsCallback> g_sidepanel_setoptions_map_;
}
#endif

// static
NWebExtensionSidePanelCefDelegate* NWebExtensionSidePanelCefDelegate::GetInstance() {
  static NWebExtensionSidePanelCefDelegate instance;
  return &instance;
}
 
NWebExtensionSidePanelCefDelegate::NWebExtensionSidePanelCefDelegate() {}
 
bool NWebExtensionSidePanelCefDelegate::HasOnOpenByPbCallback() {
#if !BUILDFLAG(ARKWEB_NWEB_EX)
  return false;
#else
  return NWebExtensionSidePanelDispatcher::HasOnOpenByPbCallback();
#endif
}
 
bool NWebExtensionSidePanelCefDelegate::HasOnSetOptionsByPbCallback() {
#if !BUILDFLAG(ARKWEB_NWEB_EX)
  return false;
#else
  return NWebExtensionSidePanelDispatcher::HasOnSetOptionsByPbCallback();
#endif
}
 
// static
NO_SANITIZE("cfi-icall")
bool NWebExtensionSidePanelCefDelegate::OnOpenByPb(
    ExtensionSidePanelOpenOptions& options,
    SidePanelOnOpenCallback callback) {
#if !BUILDFLAG(ARKWEB_NWEB_EX)
  return false;
#else
  LOG(DEBUG) << "NWebExtensionSidePanelCefDelegate OnOpenByPb";
  static int open_request_id = 0;
  open_request_id++;
  g_sidepanel_open_map_[open_request_id ] = std::move(callback);
  options.requestId = open_request_id;
  
  if (!NWebExtensionSidePanelDispatcher::OnOpenByPb(options)) {
    g_sidepanel_open_map_.erase(open_request_id);
    return false;
  }
 
  return true;
#endif
}
 
// static
NO_SANITIZE("cfi-icall")
bool NWebExtensionSidePanelCefDelegate::OnSetOptionsByPb(
    ExtensionSidePanelSetOptions& options,
    SidePanelOnSetOptionsCallback callback) {
#if !BUILDFLAG(ARKWEB_NWEB_EX)
  return false;
#else
  LOG(DEBUG) << "NWebExtensionSidePanelCefDelegate OnSetOptionsByPb";
  static int request_id = 0;
  request_id++;
  g_sidepanel_setoptions_map_[request_id] = std::move(callback);
  options.requestId = request_id;
  
  if (!NWebExtensionSidePanelDispatcher::OnSetOptionsByPb(options)) {
    g_sidepanel_setoptions_map_.erase(request_id);
    return false;
  }
 
  return true;
#endif
}

#if BUILDFLAG(ARKWEB_NWEB_EX)
void NWebExtensionSidePanelCefDelegate::OnOpenCallback(
      int request_id,
      const std::optional<std::string>& error) {
  if (g_sidepanel_open_map_.count(request_id)) {
    std::move(g_sidepanel_open_map_[request_id]).Run(error);
    g_sidepanel_open_map_.erase(request_id);
  } else {
    LOG(ERROR) << "request_id is inValid";
  }
}
 
void NWebExtensionSidePanelCefDelegate::OnSetOptionsCallback(
    int request_id,
    const std::optional<std::string>& error) {
  if (g_sidepanel_setoptions_map_.count(request_id)) {
    std::move(g_sidepanel_setoptions_map_[request_id]).Run(error);
    g_sidepanel_setoptions_map_.erase(request_id);
  } else {
    LOG(ERROR) << "request_id is inValid";
  }
}
#endif

} // namespace OHOS::NWeb
