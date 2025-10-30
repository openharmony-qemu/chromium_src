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

#include "testing/gmock/include/gmock/gmock.h"
#include "chrome/browser/extensions/api/declarative_content/content_action.h"
#include "arkweb/chromium_ext/chrome/browser/extensions/api/declarative_content/content_action_for_include.cc"

namespace extensions {
namespace {
const char* TEST_DEFAULT_URL = "http://example.com";
const char* TEST_DEFAULT_TITLE = "Initial Title";
const char* TEST_EXTENSION_NAME = "Test Extension";
const char* TEST_EXTENSION_ID = "test_extension_id";
}  // namespace

TEST(ContentActionTest, ContentActionGetActionIcon_NullAction) {
  gfx::Image result = ContentActionGetActionIcon(nullptr, 1);
  EXPECT_TRUE(result.IsEmpty());
}

TEST(ContentActionTest, ContentActionGetActionIcon_ExplicitlySetIcon) {
  ActionInfo action_info(ActionInfo::Type::kPage);
  action_info.default_popup_url = GURL(TEST_DEFAULT_URL);
  action_info.default_title = TEST_DEFAULT_TITLE;
  std::unique_ptr<ExtensionAction> action = CreateAction(action_info);

  int tab_id = 1;

  SkBitmap bitmap;
  bitmap.allocN32Pixels(16, 16);
  bitmap.eraseColor(SK_ColorRED);
  gfx::Image test_image = gfx::Image::CreateFrom1xBitmap(bitmap);
  action->SetIcon(tab_id, test_image);
  gfx::Image result = ContentActionGetActionIcon(action.get(), tab_id);
  EXPECT_FALSE(result.IsEmpty());
  EXPECT_TRUE(test_image.AsImageSkia().BackedBySameObjectAs(result.AsImageSkia()));
}

TEST(ContentActionTest, ContentActionGetActionIcon_DeclarativeIcon) {
  ActionInfo action_info(ActionInfo::Type::kPage);
  action_info.default_popup_url = GURL(TEST_DEFAULT_URL);
  action_info.default_title = TEST_DEFAULT_TITLE;
  std::unique_ptr<ExtensionAction> action = CreateAction(action_info);

  int tab_id = 1;
  SkBitmap bitmap;
  bitmap.allocN32Pixels(16, 16);
  bitmap.eraseColor(SK_ColorBLUE);
  gfx::Image declarative_image = gfx::Image::CreateFrom1xBitmap(bitmap);
  action->DeclarativeSetIcon(tab_id, 100, declarative_image);
  gfx::Image result = ContentActionGetActionIcon(action.get(), tab_id);
  EXPECT_FALSE(result.IsEmpty());
  EXPECT_TRUE(declarative_image.AsImageSkia().BackedBySameObjectAs(result.AsImageSkia()));
}

TEST(ContentActionTest, ContentActionGetActionIcon_DefaultIcon) {
  ActionInfo action_info(ActionInfo::Type::kPage);
  action_info.default_popup_url = GURL(TEST_DEFAULT_URL);
  action_info.default_title = TEST_DEFAULT_TITLE;
  std::unique_ptr<ExtensionAction> action = CreateAction(action_info);

  int tab_id = 1;
  gfx::Image result = ContentActionGetActionIcon(action.get(), tab_id);
  EXPECT_FALSE(result.IsEmpty());
}

class MockNWebExtensionActionCefDelegate
    : public OHOS::NWeb::NWebExtensionActionCefDelegate {
 public:
  MockNWebExtensionActionCefDelegate() : OHOS::NWeb::NWebExtensionActionCefDelegate() {}
  MOCK_METHOD(void, OnEnable,
      (const std::string&, std::optional<int>&, std::optional<std::string>, std::optional<bool>),
      ());
  MOCK_METHOD(void, OnDisable,
      (const std::string&, std::optional<int>&, std::optional<std::string>, std::optional<bool>),
      ());
};

TEST(ContentActionTest, ContentActionShowExtension_NullAction001) {
  MockNWebExtensionActionCefDelegate mock_delegate;
  EXPECT_CALL(mock_delegate, OnEnable).Times(0);

  scoped_refptr<const Extension> extension =
      ExtensionBuilder(TEST_EXTENSION_NAME).SetID(TEST_EXTENSION_ID).Build();
  ContentAction::ApplyInfo apply_info;
  apply_info.extension = extension.get();
  ContentActionShowExtension(nullptr, apply_info, false);
}

TEST(ContentActionTest, ContentActionShowExtension_NullAction002) {
  MockNWebExtensionActionCefDelegate mock_delegate;
  EXPECT_CALL(mock_delegate, OnEnable).Times(0);
  EXPECT_CALL(mock_delegate, OnDisable).Times(0);

  scoped_refptr<const Extension> extension =
      ExtensionBuilder(TEST_EXTENSION_NAME).SetID(TEST_EXTENSION_ID).Build();
  ContentAction::ApplyInfo apply_info;
  apply_info.extension = extension.get();
  ContentActionShowExtension(nullptr, apply_info, true);
}

TEST(ContentActionTest, ContentActionShowExtension_NullExtension) {
  ActionInfo action_info(ActionInfo::Type::kPage);
  action_info.default_popup_url = GURL(TEST_DEFAULT_URL);
  action_info.default_title = TEST_DEFAULT_TITLE;
  std::unique_ptr<ExtensionAction> action = CreateAction(action_info);

  ContentAction::ApplyInfo apply_info;
  apply_info.extension = nullptr;
  MockNWebExtensionActionCefDelegate mock_delegate;
  EXPECT_CALL(mock_delegate, OnEnable).Times(0);
  EXPECT_CALL(mock_delegate, OnDisable).Times(0);

  ContentActionShowExtension(action.get(), apply_info, false);
  ContentActionShowExtension(action.get(), apply_info, true);
}

TEST(ContentActionTest, ContentActionShowExtension_ForcedFlagTrue) {
  ActionInfo action_info(ActionInfo::Type::kPage);
  action_info.default_popup_url = GURL(TEST_DEFAULT_URL);
  action_info.default_title = TEST_DEFAULT_TITLE;
  std::unique_ptr<ExtensionAction> action = CreateAction(action_info);

  scoped_refptr<const Extension> extension =
      ExtensionBuilder(TEST_EXTENSION_NAME).SetID(TEST_EXTENSION_ID).Build();

  ContentAction::ApplyInfo apply_info;
  apply_info.extension = extension.get();
  apply_info.tab = nullptr;
  apply_info.browser_context = nullptr;

  MockNWebExtensionActionCefDelegate mock_delegate;
  EXPECT_CALL(mock_delegate, OnDisable).Times(0);
  ContentActionShowExtension(action.get(), apply_info, true);
}

TEST(ContentActionTest, ContentActionShowExtension_ActionVisible) {
  ActionInfo action_info(ActionInfo::Type::kPage);
  action_info.default_popup_url = GURL(TEST_DEFAULT_URL);
  action_info.default_title = TEST_DEFAULT_TITLE;
  std::unique_ptr<ExtensionAction> action = CreateAction(action_info);
  action->SetIsVisible(ExtensionAction::kDefaultTabId, true);

  scoped_refptr<const Extension> extension =
      ExtensionBuilder(TEST_EXTENSION_NAME).SetID(TEST_EXTENSION_ID).Build();

  ContentAction::ApplyInfo apply_info;
  apply_info.extension = extension.get();
  apply_info.tab = nullptr;
  apply_info.browser_context = nullptr;

  MockNWebExtensionActionCefDelegate mock_delegate;
  EXPECT_CALL(mock_delegate, OnDisable).Times(0);
  ContentActionShowExtension(action.get(), apply_info, false);
}

TEST(ContentActionTest, ContentActionShowExtension_ActionNotVisible) {
  ActionInfo action_info(ActionInfo::Type::kPage);
  action_info.default_popup_url = GURL(TEST_DEFAULT_URL);
  action_info.default_title = TEST_DEFAULT_TITLE;
  std::unique_ptr<ExtensionAction> action = CreateAction(action_info);
  action->SetIsVisible(ExtensionAction::kDefaultTabId, false);

  scoped_refptr<const Extension> extension =
      ExtensionBuilder(TEST_EXTENSION_NAME).SetID(TEST_EXTENSION_ID).Build();

  ContentAction::ApplyInfo apply_info;
  apply_info.extension = extension.get();
  apply_info.tab = nullptr;
  apply_info.browser_context = nullptr;

  MockNWebExtensionActionCefDelegate mock_delegate;
  EXPECT_CALL(mock_delegate, OnEnable).Times(0);
  ContentActionShowExtension(action.get(), apply_info, false);
}
}  // namespace extensions
