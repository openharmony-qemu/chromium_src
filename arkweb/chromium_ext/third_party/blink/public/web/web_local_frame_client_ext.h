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

#ifndef ARKWEB_CHROMIUM_EXT_THIRD_PARTY_BLINK_PUBLIC_WEB_WEB_LOCAL_FRAME_CLIENT_EXT_H_
#define ARKWEB_CHROMIUM_EXT_THIRD_PARTY_BLINK_PUBLIC_WEB_WEB_LOCAL_FRAME_CLIENT_EXT_H_

#include "third_party/blink/public/web/web_local_frame_client.h"
#if BUILDFLAG(ARKWEB_SAME_LAYER)
#include "arkweb/chromium_ext/third_party/blink/renderer/platform/web_native_bridge.h"
#endif

namespace blink {
#if BUILDFLAG(ARKWEB_SAME_LAYER)
class WebLocalFrameClient;
class WebNativeClient;
#endif

class BLINK_EXPORT WebLocalFrameClientExt : public WebLocalFrameClient {
 public:
  ~WebLocalFrameClientExt() override = default;
  WebLocalFrameClientExt* AsWebLocalFrameClientExt() override { return this; }

#if BUILDFLAG(ARKWEB_SAME_LAYER)
  virtual WebNativeBridge* CreateWebNativeBridge(WebNativeClient* client) {
    return nullptr;
  }
#endif


#if BUILDFLAG(ARKWEB_CLIPBOARD)
  virtual void MouseSelectMenuShow(bool show) {}
  virtual void ChangeVisibilityOfQuickMenu() {}
#endif

#if BUILDFLAG(ARKWEB_AI)
  virtual void CloseImageOverlaySelection() {}
#endif

#if BUILDFLAG(ARKWEB_DRAG_DROP)
  // Tell the UI to dismiss the contextmenu popup window
  virtual void ClearContextMenu() {}
#endif  // BUILDFLAG(ARKWEB_DRAG_DROP)

#if BUILDFLAG(ARKWEB_ADBLOCK)
  virtual void DidSubresourceFiltered() {}
  virtual bool GetGlobalAdblockEnabled() { return false; }
#endif

#if BUILDFLAG(ARKWEB_JSPROXY)
  virtual void RunScriptsAtHeadReady() {}
#endif

#if BUILDFLAG(ARKWEB_BLANK_OPTIMIZE)
  virtual void NotifyLcpForBlankless() {}
  virtual void SendBlanklessKeyToRenderFrame(uint32_t nweb_id,
                                             uint64_t blankless_key,
                                             uint64_t frame_sink_id,
                                             int64_t pref_hash) {}
#endif
};

}

#endif // ARKWEB_CHROMIUM_EXT_THIRD_PARTY_BLINK_PUBLIC_WEB_WEB_LOCAL_FRAME_CLIENT_EXT_H_