// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#ifndef COMPONENTS_CONTENT_SETTINGS_RENDERER_ARKWEB_CONTENT_SETTINGS_AGENT_IMPL_EXT_H_
#define COMPONENTS_CONTENT_SETTINGS_RENDERER_ARKWEB_CONTENT_SETTINGS_AGENT_IMPL_EXT_H_

#include <string>
#include <utility>

#include "base/containers/flat_map.h"
#include "base/containers/flat_set.h"
#include "base/functional/callback.h"
#include "base/gtest_prod_util.h"
#include "components/content_settings/common/content_settings_agent.mojom.h"
#include "components/content_settings/common/content_settings_manager.mojom.h"
#include "components/content_settings/core/common/content_settings.h"
#include "components/content_settings/core/common/content_settings_types.h"
#include "content/public/renderer/render_frame_observer.h"
#include "content/public/renderer/render_frame_observer_tracker.h"
#include "mojo/public/cpp/bindings/associated_receiver.h"
#include "mojo/public/cpp/bindings/associated_receiver_set.h"
#include "mojo/public/cpp/bindings/associated_remote.h"
#include "third_party/blink/public/platform/web_content_settings_client.h"
#include "url/gurl.h"
#include "url/origin.h"

#if BUILDFLAG(IS_ARKWEB_EXT)
#include "arkweb/ohos_nweb_ex/build/features/features.h"
#endif

#if BUILDFLAG(ARKWEB_EXT_EXCEPTION_LIST)
#include "base/containers/flat_map.h"
#include "components/content_settings/core/common/content_settings.h"
#include "third_party/blink/public/platform/web_security_origin.h"
#include "third_party/blink/public/platform/web_url.h"
#include "third_party/blink/public/web/web_frame.h"
#include "ui/base/page_transition_types.h"
#endif

namespace blink {
class WebFrame;
class WebURL;
}  // namespace blink

namespace content_settings {

class ArkWebContentSettingsAgentImplExt : public ContentSettingsAgentImpl {
 public:
  ArkWebContentSettingsAgentImplExt(content::RenderFrame* render_frame,
                                    std::unique_ptr<Delegate> delegate);

  ArkWebContentSettingsAgentImplExt(const ArkWebContentSettingsAgentImplExt&) = delete;
  ArkWebContentSettingsAgentImplExt& operator=(const ArkWebContentSettingsAgentImplExt&) = delete;

  ~ArkWebContentSettingsAgentImplExt() override;

  ArkWebContentSettingsAgentImplExt *AsArkWebContentSettingsAgentImplExt() override { return this; }

#if BUILDFLAG(ARKWEB_EXT_EXCEPTION_LIST)
  bool ShouldAllowlistForContentSettings() const;

  // Helpers.
  // True if |render_frame()| contains content that is white-listed for content
  // settings.
  bool IsWhitelistedForContentSettings() const;

  // Exposed for unit tests.
  static bool IsWhitelistedForContentSettings(
      const blink::WebSecurityOrigin& origin,
      const blink::WebURL& document_url);

  void SetContentSettingRules(
      const RendererContentSettingRules* content_setting_rules);
#endif
};

}  // namespace content_settings

#endif  // COMPONENTS_CONTENT_SETTINGS_RENDERER_ARKWEB_CONTENT_SETTINGS_AGENT_IMPL_EXT_H_
