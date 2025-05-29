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

#ifndef CONTENT_BROWSER_RENDERER_HOST_RENDER_FRAME_HOST_DELEGATE_EXT_H_
#define CONTENT_BROWSER_RENDERER_HOST_RENDER_FRAME_HOST_DELEGATE_EXT_H_

#if BUILDFLAG(ARKWEB_SAME_LAYER)
#include "arkweb/chromium_ext/content/public/browser/native_embed_info.h"
#include "arkweb/chromium_ext/media/mojo/mojom/native_bridge.mojom.h"
#endif

namespace content {

#if BUILDFLAG(ARKWEB_SAME_LAYER)
struct GlobalRenderFrameHostId;
#endif
class RenderFrameHostDelegateExt {
 public:
#if BUILDFLAG(ARKWEB_EXT_FREE_COPY)
  virtual void NotifyContextMenuWillShow() {}
#endif
#if BUILDFLAG(ARKWEB_MENU)
  virtual void MouseSelectMenuShow(bool show) {}
  virtual void ChangeVisibilityOfQuickMenu() {}
#endif

#if BUILDFLAG(ARKWEB_AI)
  virtual void CloseImageOverlaySelection() {}
#endif  // BUILDFLAG(ARKWEB_AI)
#if BUILDFLAG(ARKWEB_SAME_LAYER)
  virtual void CreateNativeBridgeHostForRenderFrameHost(
      RenderFrameHostImpl* frame_host,
      mojo::PendingAssociatedReceiver<media::mojom::NativeBridgeHost>
          receiver) {}

  virtual void OnNativeEmbedStatusUpdate(
      const NativeEmbedInfo& native_embed_info,
      NativeEmbedInfo::TagState state) {}

  virtual void OnRenderFrameHostEnterBackForwardCache(
      const GlobalRenderFrameHostId& id) {}
  virtual void OnRenderFrameHostLeaveBackForwardCache(
      const GlobalRenderFrameHostId& id) {}
#endif
#if BUILDFLAG(ARKWEB_DRAG_DROP)
  // Notified to UI, contextmenu popup window should be dimissed
  virtual void ClearContextMenu() {}
#endif  // BUILDFLAG(ARKWEB_DRAG_DROP)
};
}  // namespace content
#endif  // CONTENT_BROWSER_RENDERER_HOST_RENDER_FRAME_HOST_DELEGATE_EXT_H_