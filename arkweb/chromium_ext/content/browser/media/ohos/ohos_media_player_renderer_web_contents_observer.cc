/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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
  for (OHOSMediaPlayerRenderer* player : players_) {
    player->OnUpdateAudioMutingState(muted);
  }
}

void OHOSMediaPlayerRendererWebContentsObserver::WebContentsDestroyed() {
  for (OHOSMediaPlayerRenderer* player : players_) {
    player->OnWebContentsDestroyed();
  }
  players_.clear();
}

WEB_CONTENTS_USER_DATA_KEY_IMPL(OHOSMediaPlayerRendererWebContentsObserver);

}  // namespace content
