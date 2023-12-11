// Copyright (c) 2023 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
#include "content/browser/media/ohos/ohos_media_player_renderer_web_contents_observer.h"

#include "content/browser/media/ohos/ohos_media_player_renderer.h"

namespace content {

OHOSMediaPlayerRendererWebContentsObserver::
    OHOSMediaPlayerRendererWebContentsObserver(WebContents* web_contents)
    : WebContentsObserver(web_contents),
      WebContentsUserData<OHOSMediaPlayerRendererWebContentsObserver>(
          *web_contents) {}

OHOSMediaPlayerRendererWebContentsObserver::
    ~OHOSMediaPlayerRendererWebContentsObserver() = default;

void OHOSMediaPlayerRendererWebContentsObserver::AddMediaPlayerRenderer(
    OHOSMediaPlayerRenderer* player) {
  DCHECK(player);
  DCHECK(players_.find(player) == players_.end());
  players_.insert(player);
}

void OHOSMediaPlayerRendererWebContentsObserver::RemoveMediaPlayerRenderer(
    OHOSMediaPlayerRenderer* player) {
  DCHECK(player);
  auto erase_result = players_.erase(player);
  DCHECK_EQ(1u, erase_result);
}

void OHOSMediaPlayerRendererWebContentsObserver::DidUpdateAudioMutingState(
    bool muted) {
  for (OHOSMediaPlayerRenderer* player : players_)
    player->OnUpdateAudioMutingState(muted);
}

void OHOSMediaPlayerRendererWebContentsObserver::WebContentsDestroyed() {
  for (OHOSMediaPlayerRenderer* player : players_)
    player->OnWebContentsDestroyed();
  players_.clear();
}

WEB_CONTENTS_USER_DATA_KEY_IMPL(OHOSMediaPlayerRendererWebContentsObserver);

}  // namespace content
