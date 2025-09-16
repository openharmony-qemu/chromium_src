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

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "base/files/file_util.h"
#include "base/files/scoped_temp_dir.h"
#define private public
#include "extensions/browser/extension_registry_info_manager.h"
#include "extensions/common/extension.h"
#undef private
#include "chrome/browser/extensions/extension_service_test_base.h"
#include "extensions/browser/unloaded_extension_reason.h"
#include "extensions/common/extension_builder.h"
#include "extensions/common/manifest_constants.h"
#include "extensions/common/manifest_url_handlers.h"
#include "extensions/test/test_extensions_client.h"
#include "extensions/browser/extensions_test.h"

#if BUILDFLAG(ARKWEB_NWEB_EX)
#include "arkweb/ohos_nweb_ex/build/features/features.h"
#include "extensions/browser/extension_util.h"
#include "extensions/common/manifest_handlers/incognito_info.h"
#include "ohos_nweb_ex/core/extension/nweb_extension_manager_dispatcher.h"
#endif

namespace extensions {
class Extension;
class ExtensionAction;
class ExtensionRegistry;

class ExtensionRegistryInfoManagerTest : public ExtensionsTest {
 public:
  ExtensionRegistryInfoManagerTest() {}
  ~ExtensionRegistryInfoManagerTest() {}

 protected:
  void SetUp() override {
    ASSERT_TRUE(temp_dir_.CreateUniqueTempDir());
    ExtensionsTest::SetUp();
    client_ = std::make_unique<extensions::TestExtensionsClient>();
    extensions::ExtensionsClient::Set(client_.get());
    manager_ =
        std::make_unique<ExtensionRegistryInfoManager>(browser_context());
  }

  void TearDown() override {
    manager_.reset();
    client_.reset();
    ExtensionsTest::TearDown();
    base::DeletePathRecursively(temp_dir_.GetPath());
  }

  scoped_refptr<Extension> CreateTestExtension(const std::string& name) {
    base::FilePath path = temp_dir_.GetPath();
    path = path.AppendASCII(name);

    auto manifest = base::Value::Dict()
                        .Set("name", name)
                        .Set("version", "1")
                        .Set("manifest_version", 2);
    std::string error;
    scoped_refptr<Extension> extension(
        Extension::Create(path, mojom::ManifestLocation::kInternal, manifest,
                          Extension::NO_FLAGS, &error));
    EXPECT_TRUE(extension.get()) << error;
    return extension;
  }

  scoped_refptr<const Extension> CreateTestExtension() {
    return ExtensionBuilder("Test Extension")
        .SetManifestVersion(3)
        .SetID("test_extension_id")
        .AddAPIPermission("contextMenus")
        .AddAPIPermission("sidePanel")
        .SetAction(ActionInfo::Type::kBrowser)
        .Build();
  }

  ExtensionRegistryInfoManager* GetInfoManager() { return manager_.get(); }

  void SetListener(
      void (*onWebExtensionLoaded)(const WebExtensionInfo& loadedinfo),
      void (*onWebExtensionUnLoaded)(std::string extensionId),
      void (*onWebExtensionOpenUrlFun)(std::string url)) {
    callback_called_count_ = 0;
    ExtensionRegistryInfoManager::UnRegisterWebExtensionManagerListener();
    auto mock_listener = std::make_shared<NWebExtensionManagerCallBack>();
    EXPECT_NE(mock_listener, nullptr);
    mock_listener->OnWebExtensionLoaded = onWebExtensionLoaded;
    mock_listener->OnWebExtensionUnLoaded = onWebExtensionUnLoaded;
    mock_listener->OnWebExtensionOpenUrlFun = onWebExtensionOpenUrlFun;
    ExtensionRegistryInfoManager::RegisterWebExtensionManagerListener(
        mock_listener);
  }

  static void OnWebExtensionLoadedCallback(const WebExtensionInfo& info) {
    callback_called_count_++;
  }
  static void OnWebExtensionUnLoadedCallback(std::string id) {
    callback_called_count_++;
  }
  static void OnWebExtensionOpenUrlCallback(std::string url) {
    callback_called_count_++;
  }

  int GetCalledCount() { return callback_called_count_; }

 private:
  std::unique_ptr<ExtensionRegistryInfoManager> manager_;
  std::unique_ptr<extensions::TestExtensionsClient> client_;
  base::ScopedTempDir temp_dir_;
  static int callback_called_count_;
};
int ExtensionRegistryInfoManagerTest::callback_called_count_ = 0;

#if BUILDFLAG(ARKWEB_NWEB_EX)
void SetIncognitoMode(Extension* extension,
                      api::incognito::IncognitoMode mode) {
  extension->SetManifestData(api::incognito::ManifestKeys::kIncognito,
                             std::make_unique<IncognitoInfo>(mode));
}
TEST_F(ExtensionRegistryInfoManagerTest, GetExtensionIncognitoMode) {
  scoped_refptr<Extension> extension(CreateTestExtension("test_extension_id"));
  ASSERT_TRUE(extension.get() != nullptr);

  // IsIncognitoAllowed true
  SetIncognitoMode(extension.get(), api::incognito::IncognitoMode::kNotAllowed);
  auto mode = GetInfoManager()->GetExtensionIncognitoMode(extension.get());
  EXPECT_EQ(mode, EXT_INCOGNITO_NOT_ALLOWED);
  // IsSpanningMode true
  SetIncognitoMode(extension.get(), api::incognito::IncognitoMode::kSpanning);
  mode = GetInfoManager()->GetExtensionIncognitoMode(extension.get());
  EXPECT_EQ(mode, EXT_INCOGNITO_SPANNING);
  // IsSplitMode true
  SetIncognitoMode(extension.get(), api::incognito::IncognitoMode::kSplit);
  mode = GetInfoManager()->GetExtensionIncognitoMode(extension.get());
  EXPECT_EQ(mode, EXT_INCOGNITO_SPLIT);
  // SetSplitMode false
  SetIncognitoMode(extension.get(), api::incognito::IncognitoMode::kNone);
  mode = GetInfoManager()->GetExtensionIncognitoMode(extension.get());
  EXPECT_EQ(mode, EXT_INCOGNITO_NONE);
}
#endif

TEST_F(ExtensionRegistryInfoManagerTest, GetExtensionActionInfo_NoData) {
  scoped_refptr<const Extension> extension = CreateTestExtension();
  ASSERT_TRUE(extension);
  ExtensionRegistry* registry = ExtensionRegistry::Get(browser_context());
  EXPECT_NE(registry, nullptr);
  bool result = registry->AddEnabled(extension);
  ASSERT_TRUE(result);

  const int32_t tab_id = 100;
  WebExtensionActionInfo action_info =
      GetInfoManager()->GetExtensionActionInfo(*extension.get(), tab_id);
  EXPECT_TRUE(action_info.extensionId == extension->id());

  EXPECT_FALSE(action_info.isEnabled);
  EXPECT_FALSE(action_info.badgeBackgroundColor.has_value());
  EXPECT_FALSE(action_info.badgeTextColor.has_value());
  EXPECT_FALSE(action_info.badgeText.has_value());
  EXPECT_FALSE(action_info.popup.has_value());
  EXPECT_FALSE(action_info.title.has_value());

  WebExtensionActionInfoV2 action_info_v2 =
      GetInfoManager()->GetExtensionActionInfoV2(*extension.get(), tab_id);
  EXPECT_TRUE(action_info_v2.extensionId == extension->id());
  EXPECT_FALSE(action_info_v2.isEnabled);
  EXPECT_FALSE(action_info_v2.badgeBackgroundColor.has_value());
  EXPECT_FALSE(action_info_v2.badgeTextColor.has_value());
  EXPECT_FALSE(action_info_v2.badgeText.has_value());
  EXPECT_FALSE(action_info_v2.popup.has_value());
  EXPECT_FALSE(action_info_v2.title.has_value());
}

TEST_F(ExtensionRegistryInfoManagerTest, GetExtensionActionInfo_HasData) {
  scoped_refptr<const Extension> extension = CreateTestExtension();
  ASSERT_TRUE(extension);
  ExtensionRegistry* registry = ExtensionRegistry::Get(browser_context());
  EXPECT_NE(registry, nullptr);
  bool result = registry->AddEnabled(extension);
  ASSERT_TRUE(result);

  // create extension_action and set some values
  ExtensionAction* extension_action =
      ExtensionActionManager::Get(browser_context())
          ->GetExtensionAction(*extension.get());
  EXPECT_NE(extension_action, nullptr);
  const int32_t tab_id = 10;
  const std::string text = "test-001";
  const std::string title = "title-001";
  const GURL url("https://www.google.com");
  extension_action->SetBadgeBackgroundColor(tab_id, 0xFFFF0000u);
  extension_action->SetBadgeTextColor(tab_id, 0xFFFF0000u);
  extension_action->SetBadgeText(tab_id, text);
  extension_action->SetPopupUrl(tab_id, url);
  extension_action->SetTitle(tab_id, title);
  extension_action->SetIsVisible(tab_id, false);

  auto action_info =
      GetInfoManager()->GetExtensionActionInfo(*extension.get(), tab_id);
  EXPECT_TRUE(action_info.extensionId == extension->id());

  EXPECT_TRUE(action_info.isEnabled);
  ASSERT_TRUE(action_info.badgeBackgroundColor.has_value());
  EXPECT_EQ(action_info.badgeBackgroundColor.value()[0], 255);
  ASSERT_TRUE(action_info.badgeTextColor.has_value());
  EXPECT_EQ(action_info.badgeTextColor.value()[0], 255);
  EXPECT_EQ(action_info.popup, url.spec());
  EXPECT_EQ(action_info.title, title);
  EXPECT_EQ(action_info.badgeText, text);

  WebExtensionActionInfoV2 action_info_v2 =
      GetInfoManager()->GetExtensionActionInfoV2(*extension.get(), tab_id);
  EXPECT_TRUE(action_info_v2.extensionId == extension->id());
  EXPECT_TRUE(action_info_v2.isEnabled);
  EXPECT_TRUE(action_info_v2.badgeBackgroundColor.has_value());
  EXPECT_TRUE(action_info_v2.badgeTextColor.has_value());
  EXPECT_EQ(action_info_v2.popup, url.spec());
  EXPECT_EQ(action_info_v2.title, title);
  EXPECT_EQ(action_info_v2.badgeText, text);
}

TEST_F(ExtensionRegistryInfoManagerTest, GetExtensionManifestInfo) {
  const std::string extension_id = "test-extension-id";
  const std::string launch_url = "https://chrome.google.com/launch";
  scoped_refptr<Extension> extension(CreateTestExtension(extension_id));
  WebExtensionManifestInfo manifest;
  // invalid launch
  GetInfoManager()->GetExtensionManifestInfo(*extension.get(), manifest);
  EXPECT_FALSE(manifest.homepage_url.has_value());

  std::unique_ptr<ManifestURL> manifest_url = std::make_unique<ManifestURL>();
  ASSERT_TRUE(manifest_url);
  manifest_url->url_ = GURL(launch_url);
  extension->SetManifestData(manifest_keys::kHomepageURL,
                             std::move(manifest_url));
  GetInfoManager()->GetExtensionManifestInfo(*extension.get(), manifest);
  EXPECT_EQ(manifest.homepage_url, launch_url);
}


TEST_F(ExtensionRegistryInfoManagerTest,
       ExtensionRegistryInfoManager_OnExtensionLoadedCallBack) {
  WebExtensionInfo extension_info;
  extension_info.extensionId = "test_extension_id";

  testing::internal::CaptureStderr();
  // clear env
  ExtensionRegistryInfoManager::UnRegisterWebExtensionManagerListener();
  // no register
  ExtensionRegistryInfoManager::OnExtensionLoadedCallBack(extension_info);
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_NE(log_output.find("No web extension manager listener"),
            std::string::npos);

  SetListener(nullptr, nullptr, nullptr);
  testing::internal::CaptureStderr();
  ExtensionRegistryInfoManager::OnExtensionLoadedCallBack(extension_info);
  log_output = testing::internal::GetCapturedStderr();
  EXPECT_NE(log_output.find("No OnWebExtensionLoaded listener"),
            std::string::npos);

  SetListener(OnWebExtensionLoadedCallback, OnWebExtensionUnLoadedCallback,
              OnWebExtensionOpenUrlCallback);
  ExtensionRegistryInfoManager::OnExtensionLoadedCallBack(extension_info);
  EXPECT_EQ(GetCalledCount(), 1);
}

TEST_F(ExtensionRegistryInfoManagerTest,
       ExtensionRegistryInfoManager_OnExtensionUnLoadedCallBack) {
  std::string eid = "test_extension_id";
  testing::internal::CaptureStderr();
  // clear env
  ExtensionRegistryInfoManager::UnRegisterWebExtensionManagerListener();
  // no register
  ExtensionRegistryInfoManager::OnExtensionUnLoadedCallBack(eid);
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_NE(log_output.find("No web extension manager listener"),
            std::string::npos);

  SetListener(nullptr, nullptr, nullptr);
  testing::internal::CaptureStderr();
  ExtensionRegistryInfoManager::OnExtensionUnLoadedCallBack(eid);
  log_output = testing::internal::GetCapturedStderr();
  EXPECT_NE(log_output.find("No OnWebExtensionUnLoaded listener"),
            std::string::npos);

  SetListener(OnWebExtensionLoadedCallback, OnWebExtensionUnLoadedCallback,
              OnWebExtensionOpenUrlCallback);
  ExtensionRegistryInfoManager::OnExtensionUnLoadedCallBack(eid);
  EXPECT_EQ(GetCalledCount(), 1);
}

TEST_F(ExtensionRegistryInfoManagerTest,
       ExtensionRegistryInfoManager_OnExtensionOpenUrlCallBack) {
  std::string url = "http://test.com";
  testing::internal::CaptureStderr();
  // clear env
  ExtensionRegistryInfoManager::UnRegisterWebExtensionManagerListener();
  // no register
  ExtensionRegistryInfoManager::OnExtensionOpenUrlCallBack(url);
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_NE(log_output.find("No web extension manager listener"),
            std::string::npos);

  SetListener(nullptr, nullptr, nullptr);
  testing::internal::CaptureStderr();
  ExtensionRegistryInfoManager::OnExtensionOpenUrlCallBack(url);
  log_output = testing::internal::GetCapturedStderr();
  EXPECT_NE(log_output.find("No OnWebExtensionOpenUrlFun listener"),
            std::string::npos);

  SetListener(OnWebExtensionLoadedCallback, OnWebExtensionUnLoadedCallback,
              OnWebExtensionOpenUrlCallback);
  ExtensionRegistryInfoManager::OnExtensionOpenUrlCallBack(url);
  EXPECT_EQ(GetCalledCount(), 1);
}

}  // namespace extensions
