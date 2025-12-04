// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Based on media_web_contents_observer.h originally written by
// Copyright (c) 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. 
 
#ifndef CONTENT_BROWSER_MEDIA_OHOS_NATIVE_WEB_CONTENTS_OBSERVER_H_
#define CONTENT_BROWSER_MEDIA_OHOS_NATIVE_WEB_CONTENTS_OBSERVER_H_

#include <stdint.h>

#include <map>
#include <memory>

#include "arkweb/chromium_ext/media/mojo/mojom/native_bridge.mojom.h"
#include "base/containers/flat_map.h"
#include "base/memory/raw_ptr.h"
#include "base/memory/weak_ptr.h"
#include "build/build_config.h"
#include "content/common/content_export.h"
#include "content/public/browser/global_routing_id.h"
#include "content/public/browser/media_player_id.h"
#include "content/public/browser/native_embed_info.h"
#include "content/public/browser/render_frame_host.h"
#include "content/public/browser/web_contents_observer.h"
#include "media/base/use_after_free_checker.h"
#include "mojo/public/cpp/bindings/associated_receiver.h"
#include "mojo/public/cpp/bindings/associated_receiver_set.h"
#include "mojo/public/cpp/bindings/pending_associated_receiver.h"
#include "mojo/public/cpp/bindings/pending_associated_remote.h"
#include "mojo/public/cpp/bindings/remote.h"
#include "services/device/public/mojom/wake_lock.mojom.h"
#include "third_party/abseil-cpp/absl/types/optional.h"

namespace gfx {
class Rect;
}  // namespace gfx

namespace content {

class WebContentsImpl;

// This class manages all RenderFrame based media related managers at the
// browser side. It receives IPC messages from media RenderFrameObservers and
// forwards them to the corresponding managers. The managers are responsible
// for sending IPCs back to the RenderFrameObservers at the render side.
class CONTENT_EXPORT NativeWebContentsObserver : public WebContentsObserver {
 public:
  explicit NativeWebContentsObserver(WebContentsImpl* web_contents);

  NativeWebContentsObserver(const NativeWebContentsObserver&) = delete;
  NativeWebContentsObserver& operator=(const NativeWebContentsObserver&) =
      delete;

  ~NativeWebContentsObserver() override;

  // WebContentsObserver implementation.
  void WebContentsDestroyed() override;
  void RenderFrameDeleted(RenderFrameHost* render_frame_host) override;
  void MediaPictureInPictureChanged(bool is_picture_in_picture) override {}
  void DidUpdateAudioMutingState(bool muted) override {}

  // Creates a new NativeBridgeHostImpl associated to |id| if
  // needed, and then passes |bridge_receiver| to it to establish a
  // communication channel.
  void BindNativeBridgeHost(
      GlobalRenderFrameHostId frame_routing_id,
      mojo::PendingAssociatedReceiver<media::mojom::NativeBridgeHost>
          bridge_receiver);

  void OnRenderFrameHostEnterBackForwardCache(
      const GlobalRenderFrameHostId& id);
  void OnRenderFrameHostLeaveBackForwardCache(
      const GlobalRenderFrameHostId& id);

 private:
  class BridgeInfo;
  using BridgeInfoMap =
      base::flat_map<MediaPlayerId, std::unique_ptr<BridgeInfo>>;

  // Helper class providing a per-RenderFrame object implementing the only
  // method of the media::mojom::MediaPlayerHost mojo interface, to provide the
  // renderer process with a way to notify the browser when a new MediaPlayer
  // has been created, so that a communication channel can be established.

  class NativeBridgeHostImpl : public media::mojom::NativeBridgeHost {
   public:
    NativeBridgeHostImpl(
        GlobalRenderFrameHostId frame_routing_id,
        NativeWebContentsObserver* native_web_contents_observer);
    ~NativeBridgeHostImpl() override;

    // Used to bind receivers via the BrowserInterfaceBroker.
    void BindNativeBridgeHostReceiver(
        mojo::PendingAssociatedReceiver<media::mojom::NativeBridgeHost>
            receiver);

    // media::mojom::NativeBridgeHost implementation.
    void OnNativeBridgeAdded(mojo::PendingAssociatedReceiver<
                                 media::mojom::NativeBridgeObserver> observer,
                             int32_t bridge_id) override;

   private:
    GlobalRenderFrameHostId frame_routing_id_;
    raw_ptr<NativeWebContentsObserver> native_web_contents_observer_;
    mojo::AssociatedReceiverSet<media::mojom::NativeBridgeHost> receivers_;
  };

  // Helper class providing a per-MediaPlayerId object implementing the
  // media::mojom::NativeBridgeObserver mojo interface.
  class NativeBridgeObserverHostImpl
      : public media::mojom::NativeBridgeObserver {
   public:
    NativeBridgeObserverHostImpl(
        const MediaPlayerId& native_bridge_id,
        NativeWebContentsObserver* native_web_contents_observer);
    ~NativeBridgeObserverHostImpl() override;

    // Used to bind the receiver via the BrowserInterfaceBroker.
    void BindNativeBridgeObserverReceiver(
        mojo::PendingAssociatedReceiver<media::mojom::NativeBridgeObserver>
            native_bridge_observer);

    // media::mojom::NativeBridgeObserver implementation
    void OnCreateNativeSurface(
        media::mojom::NativeEmbedInfoPtr embed_info) override;
    void OnNativeEmbedFirstFramePaint(
        int32_t native_embed_id,
        const std::string& embed_id_attribute) override;
    void OnEmbedRectChange(const gfx::Rect& new_rect) override;
    void OnDestroyNativeSurface() override;
    void OnLayerRectVisibilityChange(bool visibility, int embed_id) override;

   private:
    // Re-use MediaPlayerId to manage frame routing_id and delegate_id.
    const MediaPlayerId native_bridge_id_;
    const raw_ptr<NativeWebContentsObserver> native_web_contents_observer_;
    mojo::AssociatedReceiver<media::mojom::NativeBridgeObserver>
        native_bridge_observer_receiver_{this};

    base::WeakPtrFactory<NativeBridgeObserverHostImpl> weak_factory_{this};
  };

  using NativeBridgeHostImplMap =
      base::flat_map<GlobalRenderFrameHostId,
                     std::unique_ptr<NativeBridgeHostImpl>>;
  using NativeBridgeObserverHostImplMap =
      base::flat_map<MediaPlayerId,
                     std::unique_ptr<NativeBridgeObserverHostImpl>>;

  // Communicates with the MediaSessionControllerManager to find or create (if
  // needed) a MediaSessionController identified by |id|, in order to
  // bind its mojo remote for media::mojom::MediaPlayer.
  void OnNativeBridgeAdded(
      mojo::PendingAssociatedReceiver<media::mojom::NativeBridgeObserver>
          native_bridge_observer,
      MediaPlayerId id);

  // Returns the BridgeInfo associated with |id|, or nullptr if no such
  // BridgeInfo exists.
  BridgeInfo* GetBridgeInfo(const MediaPlayerId& id) const;

  void OnBridgeInfoChanged(const MediaPlayerId& id,
                           NativeEmbedInfo& native_embed_info);

  // Used to notify when the renderer -> browser mojo connection via the
  // interface media::mojom::MediaPlayerObserver gets disconnected.
  void OnNativeBridgeObserverDisconnected(const MediaPlayerId& id);

  // Convenience method that casts web_contents() to a WebContentsImpl*.
  WebContentsImpl* web_contents_impl() const;

  void RemoveBridgeInfo(const MediaPlayerId& id);

  // Tracking variables and associated wake locks for native bridge.
  BridgeInfoMap bridge_info_map_;

  media::UseAfterFreeChecker use_after_free_checker_;

  NativeBridgeHostImplMap native_bridge_hosts_;
  NativeBridgeObserverHostImplMap native_bridge_observer_hosts_;
};

}  // namespace content

#endif  // CONTENT_BROWSER_MEDIA_OHOS_NATIVE_WEB_CONTENTS_OBSERVER_H_
