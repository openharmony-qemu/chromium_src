// Copyright (c) 2023 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
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
