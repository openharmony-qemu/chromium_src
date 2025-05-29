// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "components/content_settings/renderer/content_settings_agent_impl.h"
#include "arkweb/chromium_ext/components/content_settings/renderer/arkweb_content_settings_agent_impl_ext.h"

#include <utility>

#include "arkweb/ohos_nweb_ex/build/features/features.h"
#include "base/feature_list.h"
#include "base/functional/bind.h"
#include "base/metrics/histogram_macros.h"
#include "base/strings/string_number_conversions.h"
#include "components/content_settings/core/common/content_settings.h"
#include "components/content_settings/core/common/content_settings.mojom.h"
#include "components/content_settings/core/common/content_settings_pattern.h"
#include "components/content_settings/core/common/content_settings_utils.h"
#include "content/public/child/child_thread.h"
#include "content/public/common/content_features.h"
#include "content/public/common/origin_util.h"
#include "content/public/common/url_constants.h"
#include "content/public/renderer/render_frame.h"
#include "third_party/blink/public/common/associated_interfaces/associated_interface_provider.h"
#include "third_party/blink/public/common/associated_interfaces/associated_interface_registry.h"
#include "third_party/blink/public/platform/browser_interface_broker_proxy.h"
#include "third_party/blink/public/platform/url_conversion.h"
#include "third_party/blink/public/platform/web_security_origin.h"
#include "third_party/blink/public/platform/web_url.h"
#include "third_party/blink/public/web/web_document.h"
#include "third_party/blink/public/web/web_local_frame.h"
#include "third_party/blink/public/web/web_local_frame_client.h"
#include "third_party/blink/public/web/web_view.h"
#include "url/gurl.h"
#include "url/origin.h"
#include "url/url_constants.h"

#if BUILDFLAG(ARKWEB_EXT_EXCEPTION_LIST)
#include "base/command_line.h"
#include "components/content_settings/core/common/content_settings_pattern.h"
#include "content/public/common/content_switches.h"
#include "content/public/common/url_constants.h"
#include "third_party/blink/public/platform/web_content_settings_client.h"
#include "third_party/blink/public/platform/web_string.h"
#include "third_party/blink/public/web/web_document.h"
#include "url/origin.h"

namespace {
GURL GetOriginOrURL(const blink::WebFrame* frame) {
  url::Origin top_origin = url::Origin(frame->Top()->GetSecurityOrigin());
  // The |top_origin| is unique ("null") e.g., for file:// URLs. Use the
  // document URL as the primary URL in those cases.
  // TODO(alexmos): This is broken for --site-per-process, since top() can be a
  // WebRemoteFrame which does not have a document(), and the WebRemoteFrame's
  // URL is not replicated.  See https://crbug.com/628759.
  if (top_origin.opaque() && frame->Top()->IsWebLocalFrame()) {
    return frame->Top()->ToWebLocalFrame()->GetDocument().Url();
  }
  return top_origin.GetURL();
}

// Allow passing both WebURL and GURL here, so that we can early return
// without allocating a new backing string if only the default rule matches.
template <typename URL>
ContentSetting GetContentSettingFromRules(
    const ContentSettingsForOneType& rules,
    const blink::WebFrame* frame,
    const URL& secondary_url) {
  // If there is only one rule, it's the default rule and we don't need to match
  // the patterns.
  if (rules.size() == 1) {
    DCHECK(rules[0].primary_pattern == ContentSettingsPattern::Wildcard());
    DCHECK(rules[0].secondary_pattern == ContentSettingsPattern::Wildcard());
    return rules[0].GetContentSetting();
  }
  const GURL& primary_url = GetOriginOrURL(frame);
  const GURL& secondary_gurl = secondary_url;
  for (const auto& rule : rules) {
    if (rule.primary_pattern.Matches(primary_url) &&
        rule.secondary_pattern.Matches(secondary_gurl)) {
      return rule.GetContentSetting();
    }
  }
  NOTREACHED();
  return CONTENT_SETTING_DEFAULT;
}
}  // namespace
#endif

using blink::WebDocument;
using blink::WebFrame;
using blink::WebSecurityOrigin;
using blink::WebString;
using blink::WebURL;

namespace content_settings {

ArkWebContentSettingsAgentImplExt::ArkWebContentSettingsAgentImplExt(
    content::RenderFrame* render_frame,
    std::unique_ptr<Delegate> delegate)
    : ContentSettingsAgentImpl(render_frame, std::move(delegate)) {}

ArkWebContentSettingsAgentImplExt::~ArkWebContentSettingsAgentImplExt() = default;

#if BUILDFLAG(ARKWEB_EXT_EXCEPTION_LIST)
bool ArkWebContentSettingsAgentImplExt::ShouldAllowlistForContentSettings() const {
  return render_frame()->GetWebFrame()->GetDocument().Url().GetString() ==
         content::kUnreachableWebDataURL;
}

// static
bool ArkWebContentSettingsAgentImplExt::IsWhitelistedForContentSettings() const {
  if (!render_frame() || !render_frame()->GetWebFrame()) {
    return false;
  }
  const blink::WebDocument& document =
      render_frame()->GetWebFrame()->GetDocument();
  return IsWhitelistedForContentSettings(document.GetSecurityOrigin(),
                                         document.Url());
}

bool ArkWebContentSettingsAgentImplExt::IsWhitelistedForContentSettings(
    const blink::WebSecurityOrigin& origin,
    const blink::WebURL& document_url) {
  if (document_url.GetString() == content::kUnreachableWebDataURL) {
    return true;
  }

  if (origin.IsOpaque()) {
    return false;  // Uninitialized document?
  }

  blink::WebString protocol = origin.Protocol();

  if (protocol == content::kChromeUIScheme) {
    return true;  // Browser UI elements should still work.
  }

  if (protocol == content::kChromeDevToolsScheme) {
    return true;  // DevTools UI elements should still work.
  }

  // If the scheme is file:, an empty file name indicates a directory listing,
  // which requires JavaScript to function properly.
  if (protocol == url::kFileScheme &&
      document_url.ProtocolIs(url::kFileScheme)) {
    return GURL(document_url).ExtractFileName().empty();
  }
  return false;
}

void ArkWebContentSettingsAgentImplExt::SetContentSettingRules(
    const RendererContentSettingRules* content_setting_rules) {
  content_setting_rules_ =
      std::make_unique<RendererContentSettingRules>(*content_setting_rules);
}
#endif

}  // namespace content_settings
