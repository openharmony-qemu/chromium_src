// Copyright (c) 2022 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CEF_OHOS_CEF_EXT_LIBCEF_RENDERER_ARKWEB_CONTENT_SETTINGS_CLIENT_H_
#define CEF_OHOS_CEF_EXT_LIBCEF_RENDERER_ARKWEB_CONTENT_SETTINGS_CLIENT_H_

#include "content/public/renderer/render_frame_observer.h"
#include "third_party/blink/public/platform/web_content_settings_client.h"

// NWeb implementation of blink::WebContentSettingsClient.
class ArkWebContentSettingsClient : public content::RenderFrameObserver,
                                    public blink::WebContentSettingsClient {
 public:
  ArkWebContentSettingsClient(const ArkWebContentSettingsClient&) = delete;
  ArkWebContentSettingsClient& operator=(const ArkWebContentSettingsClient&) =
      delete;

  explicit ArkWebContentSettingsClient(content::RenderFrame* render_view);

 private:
  ~ArkWebContentSettingsClient() override;

  // content::RenderFrameObserver implementation.
  void OnDestruct() override;

  bool ShouldAutoupgradeMixedContent() override;
#ifdef BUILDFLAG(ARKWEB_ARKWEB_EXTENSIONS)
  bool AllowWriteToClipboard() override;
#endif
};

#endif  // CEF_OHOS_CEF_EXT_LIBCEF_RENDERER_ARKWEB_CONTENT_SETTINGS_CLIENT_H_
