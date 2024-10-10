/*
 * Copyright (c) 2022-2024 Huawei Device Co., Ltd.
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

#include "content/browser/media/ohos/native_web_contents_observer.h"

#include <memory>
#include <tuple>

#include "base/containers/cxx20_erase.h"
#include "base/debug/crash_logging.h"
#include "base/debug/dump_without_crashing.h"
#include "base/functional/bind.h"
#include "base/memory/raw_ptr.h"
#include "content/browser/renderer_host/render_frame_host_impl.h"
#include "content/browser/web_contents/web_contents_impl.h"
#include "content/public/browser/native_embed_info.h"
#include "content/public/browser/render_frame_host.h"
#include "content/public/browser/web_contents.h"
#include "mojo/public/cpp/bindings/message.h"
#include "mojo/public/cpp/bindings/pending_receiver.h"
#include "services/device/public/mojom/wake_lock_context.mojom.h"
#include "ui/gfx/geometry/size.h"

namespace content {

// Maintains state for a single bridge.  Issues WebContents and power-related
// notifications appropriate for state changes.
class NativeWebContentsObserver::BridgeInfo {
 public:
  BridgeInfo(const MediaPlayerId& id,
             NativeEmbedInfo& native_embed_info,
             NativeWebContentsObserver* observer)
      : id_(id), native_embed_info_(native_embed_info), observer_(observer) {}

  ~BridgeInfo() {}

  NativeEmbedInfo native_embed_info() { return native_embed_info_; }

  BridgeInfo(const BridgeInfo&) = delete;
  BridgeInfo& operator=(const BridgeInfo&) = delete;

 private:
  const MediaPlayerId id_;
  NativeEmbedInfo native_embed_info_;
  const raw_ptr<NativeWebContentsObserver> observer_;
};

NativeWebContentsObserver::NativeWebContentsObserver(
    WebContentsImpl* web_contents)
    : WebContentsObserver(web_contents) {}

NativeWebContentsObserver::~NativeWebContentsObserver() = default;

void NativeWebContentsObserver::WebContentsDestroyed() {
  use_after_free_checker_.check();

  bridge_info_map_.clear();

  // Remove all the mojo receivers and remotes associated to the native bridges
  // handled by this WebContents to prevent from handling/sending any more
  // messages after this point, plus properly cleaning things up.
  native_bridge_hosts_.clear();
  native_bridge_observer_hosts_.clear();
}

void NativeWebContentsObserver::RenderFrameDeleted(
    RenderFrameHost* render_frame_host) {
  use_after_free_checker_.check();

  GlobalRenderFrameHostId frame_routing_id = render_frame_host->GetGlobalId();

  LOG(DEBUG) << "NativeEmbed BFCache NativeWebContentsObserver::RenderFrameDeleted, frame_routing_id = "
    << frame_routing_id;

  base::EraseIf(native_bridge_hosts_,
                [frame_routing_id](const NativeBridgeHostImplMap::value_type&
                                       native_bridge_hosts_value_type) {
                  return frame_routing_id ==
                         native_bridge_hosts_value_type.first;
                });

  base::EraseIf(
      native_bridge_observer_hosts_,
      [frame_routing_id](const NativeBridgeObserverHostImplMap::value_type&
                             native_bridge_observer_hosts_value_type) {
        return frame_routing_id ==
               native_bridge_observer_hosts_value_type.first.frame_routing_id;
      });

  base::EraseIf(
      bridge_info_map_,
      [frame_routing_id](const BridgeInfoMap::value_type& id_and_bridge_info) {
        return frame_routing_id == id_and_bridge_info.first.frame_routing_id;
      });

  // Cancel any pending callbacks for bridges from this frame.
  use_after_free_checker_.check();
}

// NativeWebContentsObserver::MediaPlayerHostImpl
NativeWebContentsObserver::NativeBridgeHostImpl::NativeBridgeHostImpl(
    GlobalRenderFrameHostId frame_routing_id,
    NativeWebContentsObserver* native_web_contents_observer)
    : frame_routing_id_(frame_routing_id),
      native_web_contents_observer_(native_web_contents_observer) {}

NativeWebContentsObserver::NativeBridgeHostImpl::~NativeBridgeHostImpl() =
    default;

void NativeWebContentsObserver::NativeBridgeHostImpl::
    BindNativeBridgeHostReceiver(
        mojo::PendingAssociatedReceiver<media::mojom::NativeBridgeHost>
            receiver) {
  receivers_.Add(this, std::move(receiver));
}

void NativeWebContentsObserver::NativeBridgeHostImpl::OnNativeBridgeAdded(
    mojo::PendingAssociatedReceiver<media::mojom::NativeBridgeObserver>
        observer,
    int32_t bridge_id) {
  LOG(INFO) << "NativeEmbed BFCache, NativeBridgeHostImpl::OnNativeBridgeAdded, frame_routing_id_ = "
    << frame_routing_id_;
  native_web_contents_observer_->OnNativeBridgeAdded(
      std::move(observer), MediaPlayerId(frame_routing_id_, bridge_id));
}

NativeWebContentsObserver::NativeBridgeObserverHostImpl::
    NativeBridgeObserverHostImpl(
        const MediaPlayerId& native_bridge_id,
        NativeWebContentsObserver* native_web_contents_observer)
    : native_bridge_id_(native_bridge_id),
      native_web_contents_observer_(native_web_contents_observer) {}

NativeWebContentsObserver::NativeBridgeObserverHostImpl::
    ~NativeBridgeObserverHostImpl() {
  OnDestroyNativeSurface();
}

void NativeWebContentsObserver::NativeBridgeObserverHostImpl::
    BindNativeBridgeObserverReceiver(
        mojo::PendingAssociatedReceiver<media::mojom::NativeBridgeObserver>
            native_bridge_observer) {
  native_bridge_observer_receiver_.Bind(std::move(native_bridge_observer));

  // |native_web_contents_observer_| outlives NativeBridgeHostImpl, so it's safe
  // to use base::Unretained().
  native_bridge_observer_receiver_.set_disconnect_handler(base::BindOnce(
      &NativeWebContentsObserver::OnNativeBridgeObserverDisconnected,
      base::Unretained(native_web_contents_observer_), native_bridge_id_));
}

void NativeWebContentsObserver::NativeBridgeObserverHostImpl::
    OnCreateNativeSurface(media::mojom::NativeEmbedInfoPtr embed_info) {
  if (!native_web_contents_observer_) {
    return;
  }

  std::map<std::string, std::string> params_data;
  for (auto& item : embed_info->params) {
    params_data.emplace(item.first, item.second);
  }

  NativeEmbedInfo native_embed_info(
      embed_info->embed_id,
      native_web_contents_observer_->web_contents_impl()->GetURL(),
      embed_info->element_id, embed_info->type, embed_info->source,
      embed_info->tag, embed_info->rect, params_data);
  native_web_contents_observer_->OnBridgeInfoChanged(native_bridge_id_,
                                                     native_embed_info);
  native_web_contents_observer_->web_contents_impl()->OnNativeEmbedStatusUpdate(
      native_embed_info, NativeEmbedInfo::TagState::TAG_STATE_CREATE);
}

void NativeWebContentsObserver::NativeBridgeObserverHostImpl::
    OnDestroyNativeSurface() {
  if (!native_web_contents_observer_) {
    return;
  }

  if (auto* bridge_info =
          native_web_contents_observer_->GetBridgeInfo(native_bridge_id_)) {
    native_web_contents_observer_->web_contents_impl()
        ->OnNativeEmbedStatusUpdate(
            bridge_info->native_embed_info(),
            NativeEmbedInfo::TagState::TAG_STATE_DESTROY);

    native_web_contents_observer_->RemoveBridgeInfo(native_bridge_id_);
  }
}

void NativeWebContentsObserver::NativeBridgeObserverHostImpl::OnLayerRectVisibilityChange(
    bool visibility, int embed_id) {
  if (!native_web_contents_observer_) {
    return;
  }
  native_web_contents_observer_->web_contents_impl()
        ->OnLayerRectVisibilityChange(std::to_string(embed_id), visibility);
}

void NativeWebContentsObserver::NativeBridgeObserverHostImpl::OnEmbedRectChange(
    const gfx::Rect& new_rect) {
  if (!native_web_contents_observer_) {
    return;
  }

  if (auto* bridge_info =
          native_web_contents_observer_->GetBridgeInfo(native_bridge_id_)) {
    auto info = bridge_info->native_embed_info();
    info.rect = new_rect;
    native_web_contents_observer_->OnBridgeInfoChanged(native_bridge_id_, info);
    native_web_contents_observer_->web_contents_impl()
        ->OnNativeEmbedStatusUpdate(
            info, NativeEmbedInfo::TagState::TAG_STATE_CHANGE);
  }
}

// NativeWebContentsObserver
NativeWebContentsObserver::BridgeInfo* NativeWebContentsObserver::GetBridgeInfo(
    const MediaPlayerId& id) const {
  const auto it = bridge_info_map_.find(id);
  return it != bridge_info_map_.end() ? it->second.get() : nullptr;
}

void NativeWebContentsObserver::OnBridgeInfoChanged(
    const MediaPlayerId& id,
    NativeEmbedInfo& native_embed_info) {
  auto* bridge_info = GetBridgeInfo(id);
  if (!bridge_info) {
    bridge_info_map_.emplace(
        id, std::make_unique<BridgeInfo>(id, native_embed_info, this));
  }
}

void NativeWebContentsObserver::RemoveBridgeInfo(const MediaPlayerId& id) {
  bridge_info_map_.erase(id);
}

void NativeWebContentsObserver::OnNativeBridgeObserverDisconnected(
    const MediaPlayerId& id) {
  DCHECK(native_bridge_observer_hosts_.contains(id));
  native_bridge_observer_hosts_.erase(id);
}

WebContentsImpl* NativeWebContentsObserver::web_contents_impl() const {
  return static_cast<WebContentsImpl*>(web_contents());
}

void NativeWebContentsObserver::BindNativeBridgeHost(
    GlobalRenderFrameHostId frame_routing_id,
    mojo::PendingAssociatedReceiver<media::mojom::NativeBridgeHost>
        bridge_receiver) {
  if (!native_bridge_hosts_.contains(frame_routing_id)) {
    native_bridge_hosts_[frame_routing_id] =
        std::make_unique<NativeBridgeHostImpl>(frame_routing_id, this);
  }

  native_bridge_hosts_[frame_routing_id]->BindNativeBridgeHostReceiver(
      std::move(bridge_receiver));
}

void NativeWebContentsObserver::OnRenderFrameHostEnterBackForwardCache(const GlobalRenderFrameHostId& id) {
  for (auto& bridge_info : bridge_info_map_) {
    if (bridge_info.first.frame_routing_id == id && web_contents_impl()) {
      LOG(INFO) << "NativeEmbed BFCache, NativeWebContentsObserver received render frame host enter bfcache," \
        " GlobalId = " << id << ", native_embed_info = " << bridge_info.second->native_embed_info();

      web_contents_impl()->OnNativeEmbedStatusUpdate(bridge_info.second->native_embed_info(),
        NativeEmbedInfo::TagState::TAG_STATE_ENTER_BFCACHE);
    }
  }
}

void NativeWebContentsObserver::OnRenderFrameHostLeaveBackForwardCache(const GlobalRenderFrameHostId& id) {
  for (auto& bridge_info : bridge_info_map_) {
    if (bridge_info.first.frame_routing_id == id && web_contents_impl()) {
      LOG(INFO) << "NativeEmbed BFCache, NativeWebContentsObserver received render frame host leave bfcache," \
        " GlobalId = " << id << ", native_embed_info = " << bridge_info.second->native_embed_info();

      web_contents_impl()->OnNativeEmbedStatusUpdate(bridge_info.second->native_embed_info(),
        NativeEmbedInfo::TagState::TAG_STATE_LEAVE_BFCACHE);
    }
  }
}

void NativeWebContentsObserver::OnNativeBridgeAdded(
    mojo::PendingAssociatedReceiver<media::mojom::NativeBridgeObserver>
        native_bridge_observer,
    MediaPlayerId id) {
  // Create a new NativeBridgeObserverHostImpl for |id|, implementing the
  // media::mojom::NativeBridgeObserver mojo interface, to handle messages sent
  // from the NativeBridge element in the renderer process.
  if (!native_bridge_observer_hosts_.contains(id)) {
    native_bridge_observer_hosts_[id] =
        std::make_unique<NativeBridgeObserverHostImpl>(id, this);
  }
  native_bridge_observer_hosts_[id]->BindNativeBridgeObserverReceiver(
      std::move(native_bridge_observer));
}

}  // namespace content