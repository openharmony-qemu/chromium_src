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

#ifndef CONTENT_BROWSER_MEIA_OHOS_MEDIA_PLAYER_RENDERER_WEB_CONTENTS_OBSERVER_H_
#define CONTENT_BROWSER_MEIA_OHOS_MEDIA_PLAYER_RENDERER_WEB_CONTENTS_OBSERVER_H_

#include "base/containers/flat_set.h"
#include "content/public/browser/web_contents_observer.h"
#include "content/public/browser/web_contents_user_data.h"

namespace content {

class OHOSMediaPlayerRenderer;

class OHOSMediaPlayerRendererWebContentsObserver
    : public WebContentsObserver,
      public WebContentsUserData<OHOSMediaPlayerRendererWebContentsObserver> {
 public:
  OHOSMediaPlayerRendererWebContentsObserver(
      const OHOSMediaPlayerRendererWebContentsObserver&) = delete;
  OHOSMediaPlayerRendererWebContentsObserver& operator=(
      const OHOSMediaPlayerRendererWebContentsObserver&) = delete;

  ~OHOSMediaPlayerRendererWebContentsObserver() override;

  void AddMediaPlayerRenderer(OHOSMediaPlayerRenderer* player);
  void RemoveMediaPlayerRenderer(OHOSMediaPlayerRenderer* player);

  // WebContentsObserver implementation.
  void DidUpdateAudioMutingState(bool muted) override;
  void WebContentsDestroyed() override;

 private:
  explicit OHOSMediaPlayerRendererWebContentsObserver(
      WebContents* web_contents);
  friend class WebContentsUserData<OHOSMediaPlayerRendererWebContentsObserver>;

  base::flat_set<OHOSMediaPlayerRenderer*> players_;

  WEB_CONTENTS_USER_DATA_KEY_DECL();
};
}  // namespace content

#endif  // CONTENT_BROWSER_MEIA_OHOS_MEDIA_PLAYER_RENDERER_WEB_CONTENTS_OBSERVER_H_
