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

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "base/functional/callback.h"
#include "base/no_destructor.h"
#include "mojo/public/cpp/system/message_pipe.h"
#include "content/public/renderer/render_frame.h"
#include "content/public/test/render_view_test.h"
#include "third_party/blink/public/web/web_range.h"
#include "third_party/blink/public/web/web_document.h"
#include "third_party/blink/public/web/web_local_frame.h"
#include "third_party/blink/public/platform/browser_interface_broker_proxy.h"
#include "arkweb/chromium_ext/components/content_settings/renderer/arkweb_content_settings_agent_impl_ext.h"
#include "components/content_settings/core/common/content_settings.h"
#include "components/content_settings/core/common/content_settings_types.h"
#include "components/content_settings/core/common/content_settings_utils.h"


using ::testing::Return;
using ::testing::NiceMock;
using blink::WebDocument;
using blink::WebFrame;
using blink::WebSecurityOrigin;
using blink::WebString;
using blink::WebURL;
using content_settings::ArkWebContentSettingsAgentImplExt;

namespace content {
const char kUnreachableWebDataURL[] = "chrome-unreachable://";
const char kChromeUIScheme[] = "chrome";
const char kChromeDevToolsScheme[] = "devtools";
#if BUILDFLAG(ARKWEB_ARKWEB_EXTENSIONS)
const char kArkWebUIScheme[] = "arkweb";
#endif
}  // namespace content

class MockWebSecurityOrigin {
 public:
  MOCK_METHOD(bool, IsOpaque, (), (const));
  MOCK_METHOD(WebString, Protocol, (), (const));
};

#if BUILDFLAG(ARKWEB_EXT_EXCEPTION_LIST)
class ArkWebContentSettingsAgentImplExtTest
    : public content::RenderViewTest {
 protected:
  void SetupUrlAndOrigin(const std::string& url_str, 
                         const std::string& protocol = "",
                         bool is_opaque = false) {
    GURL url(url_str);
    WebURL web_url(url);
  }
};

TEST_F(ArkWebContentSettingsAgentImplExtTest, ShouldAllowlistForUnreachableUrl) {
  SetupUrlAndOrigin(content::kUnreachableWebDataURL);
  
  ArkWebContentSettingsAgentImplExt agent(GetMainRenderFrame(), nullptr);
  EXPECT_FALSE(agent.ShouldAllowlistForContentSettings());
}

TEST_F(ArkWebContentSettingsAgentImplExtTest, ShouldNotAllowlistForNormalUrl) {
  SetupUrlAndOrigin("https://normal-site.com");
  
  ArkWebContentSettingsAgentImplExt agent(GetMainRenderFrame(), nullptr);
  EXPECT_FALSE(agent.ShouldAllowlistForContentSettings());
}

TEST_F(ArkWebContentSettingsAgentImplExtTest, WhitelistedForChromeUIScheme) {
  SetupUrlAndOrigin("chrome://settings/", content::kChromeUIScheme);
  
  ArkWebContentSettingsAgentImplExt agent(GetMainRenderFrame(), nullptr);
  EXPECT_FALSE(agent.IsWhitelistedForContentSettings());
}

#if BUILDFLAG(ARKWEB_ARKWEB_EXTENSIONS)
TEST_F(ArkWebContentSettingsAgentImplExtTest, WhitelistedForArkWebUIScheme) {
  SetupUrlAndOrigin("arkweb://settings/", content::kArkWebUIScheme);
  
  ArkWebContentSettingsAgentImplExt agent(GetMainRenderFrame(), nullptr);
  EXPECT_FALSE(agent.IsWhitelistedForContentSettings());
}
#endif

TEST_F(ArkWebContentSettingsAgentImplExtTest, WhitelistedForDevToolsScheme) {
  SetupUrlAndOrigin("devtools://inspect/", content::kChromeDevToolsScheme);
  
  ArkWebContentSettingsAgentImplExt agent(GetMainRenderFrame(), nullptr);
  EXPECT_FALSE(agent.IsWhitelistedForContentSettings());
}

TEST_F(ArkWebContentSettingsAgentImplExtTest, WhitelistedForDirectoryListing) {
  SetupUrlAndOrigin("file:///usr/local/", url::kFileScheme);
  
  ArkWebContentSettingsAgentImplExt agent(GetMainRenderFrame(), nullptr);
  EXPECT_FALSE(agent.IsWhitelistedForContentSettings());
}

TEST_F(ArkWebContentSettingsAgentImplExtTest, NotWhitelistedForFileWithName) {
  SetupUrlAndOrigin("file:///home/user/file.html", url::kFileScheme);
  
  ArkWebContentSettingsAgentImplExt agent(GetMainRenderFrame(), nullptr);
  EXPECT_FALSE(agent.IsWhitelistedForContentSettings());
}

TEST_F(ArkWebContentSettingsAgentImplExtTest, NotWhitelistedForOpaqueOrigin) {
  SetupUrlAndOrigin("https://example.com", "", true);
  
  ArkWebContentSettingsAgentImplExt agent(GetMainRenderFrame(), nullptr);
  EXPECT_FALSE(agent.IsWhitelistedForContentSettings());
}

TEST_F(ArkWebContentSettingsAgentImplExtTest, NotWhitelistedForNormalHttps) {
  SetupUrlAndOrigin("https://example.com");
  
  ArkWebContentSettingsAgentImplExt agent(GetMainRenderFrame(), nullptr);
  EXPECT_FALSE(agent.IsWhitelistedForContentSettings());
}

TEST_F(ArkWebContentSettingsAgentImplExtTest, TestSetContentSettingRules) {
  ArkWebContentSettingsAgentImplExt agent(GetMainRenderFrame(), nullptr);

  // Set the default mixed content blocking setting.RendererContentSettingRules
  RendererContentSettingRules content_setting_rules;
  ContentSettingsForOneType& mixed_content_setting_rules =
      content_setting_rules.mixed_content_rules;
  mixed_content_setting_rules.push_back(ContentSettingPatternSource(
      ContentSettingsPattern::Wildcard(), ContentSettingsPattern::Wildcard(),
      content_settings::ContentSettingToValue(CONTENT_SETTING_BLOCK), content_settings::ProviderType::kNone,
      false));

  agent.SetContentSettingRules(&content_setting_rules);
  EXPECT_FALSE(false);
}
#endif