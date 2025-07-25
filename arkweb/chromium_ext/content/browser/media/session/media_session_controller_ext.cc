// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "content/browser/media/session/media_session_controller.h"
#include "content/browser/media/media_web_contents_observer.h"
#include "content/browser/media/session/media_session_impl.h"
#include "content/browser/web_contents/web_contents_impl.h"

namespace content {

  MediaSessionControllerExt::MediaSessionControllerExt(const MediaPlayerId& id,
                                                       WebContentsImpl* web_contents)
    : MediaSessionController(id, std::move(web_contents)) {
  }

#if BUILDFLAG(ARKWEB_MEDIA_POLICY)
void MediaSessionControllerExt::OnSetHtmlPlayEnabled(int player_id, bool enabled) {
  DCHECK_EQ(player_id_, player_id);
  if (!web_contents_)
    return;
  auto web_contents_observer = web_contents_->media_web_contents_observer();
  if (!web_contents_observer)
    return;
  if (web_contents_observer->IsPlayerIdInMediaPlayerRemotesMap(id_))
    web_contents_observer->GetMediaPlayerRemote(id_)->SetHtmlPlayEnabled(enabled);
}

void MediaSessionControllerExt::SetHtmlPlayEnabled(bool enabled) {
  OnSetHtmlPlayEnabled(player_id_, enabled);
}

void MediaSessionControllerExt::SetSessionStateIfNeed(bool isNeedMediaSession)
{
  if (!media_session_)
    return;
  if (media_content_type_ == media::MediaContentType::kOneShot) {
    LOG(INFO) << "MediaSessionController contentType is oneShot, don't control mediaSession";
    return;
  }
  if (isNeedMediaSession) {
    LOG(INFO) << "MediaSessionController media has mediaSession";
    media_session_->SetSessionState(MediaSessionImpl::NWebMediaSessionState::NEED);
  } else {
    LOG(INFO) << "MediaSessionController media is preloading, has no mediaSession";
    media_session_->SetSessionState(MediaSessionImpl::NWebMediaSessionState::NONEED);
  }
}
#endif  // BUILDFLAG(ARKWEB_MEDIA_POLICY)

#if BUILDFLAG(ARKWEB_PIP)
void MediaSessionControllerExt::OnPictureInPictureStateChanged(
    const MediaPlayerId& id,
    uint32_t state) {
  LOG(INFO) << __func__ << " Pip state:" << state <<  " "
            << id.delegate_id << " " <<  id.frame_routing_id.child_id
			<< " " << id.frame_routing_id.frame_routing_id;
  media_session_->OnPictureInPictureStateChanged(id, state);
}
#endif

#if BUILDFLAG(ARKWEB_MEDIA_AVSESSION)
void MediaSessionControllerExt::OnEndAVSession(const MediaPlayerId& id, bool is_hidden) {
  if (is_hidden && media_session_) {
    LOG(INFO) << "MediaSessionControllerExt: OnEndAVSession";
    media_session_->EndSessionWhenHide();
  }
}
#endif // ARKWEB_MEDIA_AVSESSION

#if BUILDFLAG(ARKWEB_MEDIA_MEMORY_PRESSURE)
void MediaSessionControllerExt::OnNotifyMemoryLevel(int player_id, int32_t level) {
  DCHECK_EQ(player_id_, player_id);
  if (!web_contents_) {
    LOG(ERROR) << "DMABUF::OnNotifyMemoryLevel, web_contents is null";
    return;
  }
  auto web_contents_observer = web_contents_->media_web_contents_observer();
  if (!web_contents_observer) {
    LOG(ERROR) << "DMABUF::OnNotifyMemoryLevel, web_contents_observer is null";
    return;
  }
  if (web_contents_observer->IsPlayerIdInMediaPlayerRemotesMap(id_)) {
    LOG(INFO) << "DMABUF::MediaSessionControllerExt:OnNotifyMemoryLevel, level = " << level;
    web_contents_observer->GetMediaPlayerRemote(id_)->NotifyMemoryLevel(level);
  }
}
#endif
}