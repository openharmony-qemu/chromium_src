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

#include "arkweb/chromium_ext/third_party/blink/renderer/core/html/html_native_loader.h"
#include "arkweb/chromium_ext/third_party/blink/renderer/core/html/html_plugin_element_utils.h"
#include "cc/test/test_task_graph_runner.h"
#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "third_party/blink/renderer/core/dom/document.h"
#include "third_party/blink/renderer/core/frame/local_dom_window.h"
#include "third_party/blink/renderer/core/frame/settings.h"
#include "third_party/blink/renderer/core/html/html_embed_element.h"
#include "third_party/blink/renderer/core/testing/page_test_base.h"
#include "third_party/blink/renderer/core/html/html_plugin_element.h"

namespace blink {

class HTMLPlugInElementUtilsTest : public PageTestBase {
 protected:
  void SetUp() override {
    PageTestBase::SetUp();
    GetFrame().GetSettings()->SetNativeEmbedModeEnabled(true);

    GetDocument().body()->setInnerHTML("<embed id='test-plugin' type='test/native'>");
    plugin_ = To<HTMLEmbedElement>(GetDocument().getElementById(AtomicString("test-plugin")));
    utils_ = std::make_unique<HTMLPlugInElementUtils>(plugin_.Get());

    Settings* settings = GetFrame().GetSettings();
    settings->RegisterNativeEmbedRule("valid_key", "service_prefix");
    loader_ = MakeGarbageCollected<HTMLNativeLoader>(plugin_.Get());
    plugin_->native_loader_ = loader_;
  }

  void SetServiceType(const String& service_type) {
    plugin_->SetServiceType(service_type);
  }

  void SetNativeLoader(HTMLNativeLoader* native_loader) {
    plugin_->native_loader_ = native_loader;
  }

  void TearDown() override {
    PageTestBase::TearDown();
  }

  Persistent<HTMLEmbedElement> plugin_;
  std::unique_ptr<HTMLPlugInElementUtils> utils_;
  Persistent<HTMLNativeLoader> loader_;
};

TEST_F(HTMLPlugInElementUtilsTest, CheckNativeType_FailWhenDocumentInactive) {
  GetDocument().Shutdown();
  EXPECT_FALSE(utils_->CheckNativeType("valid_key"));
}

TEST_F(HTMLPlugInElementUtilsTest, CheckNativeType_FailWhenEmbedDisabled) {
  GetFrame().GetSettings()->SetNativeEmbedModeEnabled(false);
  EXPECT_FALSE(utils_->CheckNativeType("valid_key"));
}

TEST_F(HTMLPlugInElementUtilsTest, CheckNativeType_FailWhenKeyNotFound) {
  EXPECT_FALSE(utils_->CheckNativeType("invalid_key"));
}

TEST_F(HTMLPlugInElementUtilsTest, CheckNativeType_FailWhenServiceMismatch) {
  SetServiceType("wrong_service");
  EXPECT_FALSE(utils_->CheckNativeType("valid_key"));
}

TEST_F(HTMLPlugInElementUtilsTest, CheckNativeType_Success) {
  SetServiceType("service_prefix.match");
  EXPECT_TRUE(utils_->CheckNativeType("valid_key"));
}

TEST_F(HTMLPlugInElementUtilsTest, CheckIntrinsicSize_FailWhenEmbedDisabled) {
  GetFrame().GetSettings()->SetNativeEmbedModeEnabled(false);
  EXPECT_FALSE(utils_->CheckIntrinsicSizeEnable());
}

TEST_F(HTMLPlugInElementUtilsTest, CheckIntrinsicSize_ReturnSettingsValue) {
  GetFrame().GetSettings()->SetIntrinsicSizeEnabled(true);
  EXPECT_TRUE(utils_->CheckIntrinsicSizeEnable());
}

TEST_F(HTMLPlugInElementUtilsTest, IsCssDisplayChange_FailWhenEmbedDisabled) {
  GetFrame().GetSettings()->SetNativeEmbedModeEnabled(false);
  EXPECT_FALSE(utils_->IsCssDisplayChangeEnabled());
}

TEST_F(HTMLPlugInElementUtilsTest, IsCssDisplayChange_ReturnSettingsValue) {
  GetFrame().GetSettings()->SetCSSDisplayChangeEnabled(true);
  EXPECT_FALSE(utils_->IsCssDisplayChangeEnabled());
}

TEST_F(HTMLPlugInElementUtilsTest, SetNativeEmbedOverlay_NoChange) {
  utils_->SetNativeEmbedOverlay(false);
  EXPECT_FALSE(utils_->IsOverlay());
}

TEST_F(HTMLPlugInElementUtilsTest, SetNativeEmbedOverlay_ChangeWithoutLoader) {
  SetNativeLoader(nullptr);
  utils_->SetNativeEmbedOverlay(true);
  EXPECT_TRUE(utils_->IsOverlay());
}

TEST_F(HTMLPlugInElementUtilsTest, SetNativeEmbedOverlayInfinity_NoChange) {
  utils_->SetNativeEmbedOverlayInfinity(false);
  EXPECT_FALSE(utils_->IsOverlayInfinity());
}

TEST_F(HTMLPlugInElementUtilsTest, SetNativeEmbedOverlayInfinity_ChangeWithoutLoader) {
  SetNativeLoader(nullptr);
  utils_->SetNativeEmbedOverlayInfinity(true);
  EXPECT_TRUE(utils_->IsOverlayInfinity());
}

}  // namespace blink