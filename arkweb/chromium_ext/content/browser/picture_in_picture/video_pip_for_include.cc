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
#include "third_party/blink/renderer/core/dom/events/event.h"
namespace content {


#if BUILDFLAG(ARKWEB_PIP)
void VideoPictureInPictureWindowControllerImpl::CloseExt(bool should_pause_video) {
  // The call to `Hide()` may cause `window_` to be cleared.
  CloseInternal(should_pause_video);
}

PictureInPictureResult VideoPictureInPictureWindowControllerImpl::StartSessionExt(
    PictureInPictureServiceImpl* service,
    const MediaPlayerId& player_id,
    mojo::PendingAssociatedRemote<media::mojom::MediaPlayer> player_remote,
    const viz::SurfaceId& surface_id,
    const gfx::Size& natural_size,
    bool show_play_pause_button,
    mojo::PendingRemote<blink::mojom::PictureInPictureSessionObserver> observer,
    const gfx::Rect& source_bounds,
    mojo::PendingRemote<blink::mojom::PictureInPictureSession>* session_remote,
    gfx::Size* window_size) {
  auto result = GetWebContentsImpl()->EnterPictureInPicture();
  LOG(INFO) << "PIC :" << __func__ << " result:" << (int)result << " size:" << natural_size.ToString()
           << " " << player_id.delegate_id << " " << player_id.frame_routing_id.child_id << " "
           << player_id.frame_routing_id.frame_routing_id;
  GetWebContentsImpl()->AsWebContentsImplExt()->OnPip(PIP_STATE_ENTER,
                              player_id.delegate_id,
                              player_id.frame_routing_id.child_id,
                              player_id.frame_routing_id.frame_routing_id,
                              natural_size.width(),
                              natural_size.height());

  // Picture-in-Picture may not be supported by all embedders, so we should only
  // create the session if the EnterPictureInPicture request was successful.
  if (result != PictureInPictureResult::kSuccess)
    return result;

  if (active_session_)
    active_session_->Disconnect();

  source_bounds_ = source_bounds;

  active_session_ = std::make_unique<PictureInPictureSession>(
      service, player_id, std::move(player_remote),
      session_remote->InitWithNewPipeAndPassReceiver(), std::move(observer));

  // There can be a window already if this session is replacing an old one,
  // without the old one being closed first.
  if (!window_) {
    window_ =
        GetContentClient()->browser()->CreateWindowForVideoPictureInPicture(
            this);
  }
  LOG(INFO) << "PIC window_:" << window_;
  DCHECK(window_) << "Picture in Picture requires a valid window.";

  // If the window is closed by the system, then the picture in picture session
  // will end. The renderer must call `StartSession()` again.
  EmbedSurface(surface_id, natural_size);
  SetShowPlayPauseButton(show_play_pause_button);

  return result;
}

void VideoPictureInPictureWindowControllerImpl::OnLeavingPictureInPictureExt(
    bool should_pause_video) {
  DCHECK(active_session_);
  LOG(INFO) << "Pip Exit " << pip_media_player_id_.delegate_id
            << " " << pip_media_player_id_.frame_routing_id.child_id
            << " " << pip_media_player_id_.frame_routing_id.frame_routing_id;
  GetWebContentsImpl()->AsWebContentsImplExt()->OnPip(PIP_STATE_EXIT,
      pip_media_player_id_.delegate_id,
      pip_media_player_id_.frame_routing_id.child_id,
      pip_media_player_id_.frame_routing_id.frame_routing_id, 0, 0);

  active_session_->GetMediaPlayerRemote()->PipEnable(false);

  if (IsPlayerActive() && should_pause_video) {
    // Pause the current video so there is only one video playing at a time.

    active_session_->GetMediaPlayerRemote()->PipEnable(false);

    active_session_->GetMediaPlayerRemote()->RequestPause(
        /*triggered_by_user=*/false);
  }

  active_session_->Shutdown();
  active_session_ = nullptr;
}

void VideoPictureInPictureWindowControllerImpl::OnPictureInPictureStateChanged(
    const MediaPlayerId& id, uint32_t state) {
  DCHECK(active_session_);
  LOG(INFO) << "PIC ------------" << __func__ << " state:" << state <<  " " << id.delegate_id << " " <<  id.frame_routing_id.child_id << " " << id.frame_routing_id.frame_routing_id;
  GetWebContentsImpl()->AsWebContentsImplExt()->OnPip(state, id.delegate_id,
                              id.frame_routing_id.child_id,
                              id.frame_routing_id.frame_routing_id, 0, 0);
}

void VideoPictureInPictureWindowControllerImpl::WebContentsDestroyedExt() {
  DCHECK(active_session_);
  LOG(INFO) << "Pip Exit " << pip_media_player_id_.delegate_id
            << " " << pip_media_player_id_.frame_routing_id.child_id
            << " " << pip_media_player_id_.frame_routing_id.frame_routing_id;
  GetWebContentsImpl()->AsWebContentsImplExt()->OnPip(PIP_STATE_EXIT,
      pip_media_player_id_.delegate_id,
      pip_media_player_id_.frame_routing_id.child_id,
      pip_media_player_id_.frame_routing_id.frame_routing_id, 0, 0);
}
#endif
}