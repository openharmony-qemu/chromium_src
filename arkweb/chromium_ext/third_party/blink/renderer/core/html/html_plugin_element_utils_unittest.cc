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
#include "arkweb/chromium_ext/cc/layer/layer_utils.h"
#include "cc/test/test_task_graph_runner.h"
#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "third_party/blink/renderer/core/dom/document.h"
#include "third_party/blink/renderer/core/frame/local_dom_window.h"
#include "third_party/blink/renderer/core/frame/settings.h"
#include "third_party/blink/renderer/core/html/html_embed_element.h"
#include "third_party/blink/renderer/core/testing/page_test_base.h"
#include "third_party/blink/renderer/core/html/html_plugin_element.h"
#include "third_party/bounds_checking_function/include/securec.h"

namespace blink {
class MockCcLayer : public cc::Layer {
 public:
  explicit MockCcLayer() : cc::Layer() {
    layer_utils_ = std::make_unique<cc::LayerUtils>(this);
  }

  MOCK_METHOD1(SetNativeEmbedOverlayInfinity, void(bool));
  MOCK_METHOD1(SetNativeEmbedOverlay, void(bool));

 private:
  std::unique_ptr<cc::LayerUtils> layer_utils_;
};

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

  Element::AttributeModificationParams CreateParams(const AtomicString& value) {
    return Element::AttributeModificationParams(
        html_names::kDirAttr, g_null_atom, value,
        Element::AttributeModificationReason::kDirectly);
  }

  AtomicString CreateAlignedAtomicString(const char* str) {
    size_t len = std::strlen(str);
    const size_t alignment = 8;
    const size_t buffer_size = len + alignment;
    char* raw_mem = static_cast<char*>(std::malloc(buffer_size));
    if (!raw_mem) {
      return AtomicString();
    }
    uintptr_t raw_addr = reinterpret_cast<uintptr_t>(raw_mem);
    uintptr_t aligned_addr = (raw_addr + alignment - 1) & ~(alignment - 1);
    char* aligned_ptr = reinterpret_cast<char*>(aligned_addr);
    size_t safe_copy_size = buffer_size - (aligned_addr - raw_addr);
    memcpy_s(aligned_ptr, safe_copy_size, str, len + 1);
    AtomicString result(aligned_ptr);
    std::free(raw_mem);
    return result;
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

  void ClearBufferedParamChanges() {
    utils_->buffered_param_changes_.clear();
  }

  void AppendBufferedParamChanges(const Vector<ParamChangeInfo>& changes) {
    utils_->buffered_param_changes_.AppendVector(changes);
  }

  Persistent<HTMLEmbedElement> plugin_;
  std::unique_ptr<HTMLPlugInElementUtils> utils_;
  Persistent<HTMLNativeLoader> loader_;
  MockCcLayer mock_cc_layer_;
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

TEST_F(HTMLPlugInElementUtilsTest, IsCssDisplayChange001) {
  utils_->CheckNativeType("invalid_key");
  EXPECT_FALSE(utils_->IsCssDisplayChangeEnabled());
}

TEST_F(HTMLPlugInElementUtilsTest, IsCssDisplayChange002) {
  GetFrame().GetSettings()->SetNativeEmbedModeEnabled(true);
  SetServiceType("service_prefix.match");
  bool ret = utils_->IsCssDisplayChangeEnabled();
  EXPECT_FALSE(ret);
}

TEST_F(HTMLPlugInElementUtilsTest, ProcessParamChanges) {
  Vector<ParamChangeInfo> param_changes;
  utils_->ProcessParamChanges(param_changes);
}

TEST_F(HTMLPlugInElementUtilsTest, ProcessBufferedParamChanges001) {
  ClearBufferedParamChanges();
  utils_->ProcessBufferedParamChanges();
}

TEST_F(HTMLPlugInElementUtilsTest, ProcessBufferedParamChanges002) {
  Vector<ParamChangeInfo> param_changes;
  param_changes.push_back(ParamChangeInfo(ParamChangeInfo::Status::kAdd,
                                          AtomicString("test"),
                                          AtomicString("test"),
                                          AtomicString("test")));
  AppendBufferedParamChanges(param_changes);
  utils_->ProcessBufferedParamChanges();
}

TEST_F(HTMLPlugInElementUtilsTest, SetNativeEmbedOverlay_ChangeWithLoader) {
  HTMLPlugInElement* plugin = To<HTMLPlugInElement>(GetDocument().getElementById(AtomicString("test-plugin")));
  auto html_loader = MakeGarbageCollected<HTMLNativeLoader>(plugin);
  html_loader->SetCcLayer(&mock_cc_layer_);
  SetNativeLoader(html_loader);
  EXPECT_CALL(mock_cc_layer_, SetNativeEmbedOverlay(true)).Times(1);
  utils_->SetNativeEmbedOverlay(true);
  EXPECT_TRUE(utils_->IsOverlay());
}

TEST_F(HTMLPlugInElementUtilsTest, SetNativeEmbedOverlayInfinity_ChangeWithLoader) {
  HTMLPlugInElement* plugin = To<HTMLPlugInElement>(GetDocument().getElementById(AtomicString("test-plugin")));
  auto html_loader = MakeGarbageCollected<HTMLNativeLoader>(plugin);
  html_loader->SetCcLayer(&mock_cc_layer_);
  SetNativeLoader(html_loader);
  EXPECT_CALL(mock_cc_layer_, SetNativeEmbedOverlayInfinity(true)).Times(1);
  utils_->SetNativeEmbedOverlayInfinity(true);
  EXPECT_TRUE(utils_->IsOverlayInfinity());
}

TEST_F(HTMLPlugInElementUtilsTest, ProcessParamChanges_NativeLoaderNotSet) {
  SetNativeLoader(nullptr);
  Vector<ParamChangeInfo> changes;
  changes.emplace_back(ParamChangeInfo::Status::kAdd, AtomicString("id"), AtomicString("name"), AtomicString("value"));
  utils_->ProcessParamChanges(changes);
  utils_->ProcessBufferedParamChanges();
  utils_->ProcessStretchContentToFillBounds();
  EXPECT_TRUE(loader_);
}

TEST_F(HTMLPlugInElementUtilsTest, SetStretchContentToFillBounds_ChangeWithoutLoader) {
  SetNativeLoader(nullptr);
  utils_->SetStretchContentToFillBounds(true);
}

TEST_F(HTMLPlugInElementUtilsTest, SetStretchContentToFillBounds001) {
  utils_->SetStretchContentToFillBounds(false);
}

TEST_F(HTMLPlugInElementUtilsTest, ProcessStretchContentToFillBounds) {
  Vector<ParamChangeInfo> param_changes;
  param_changes.push_back(
      ParamChangeInfo(ParamChangeInfo::Status::kAdd, AtomicString("test"), AtomicString("test"), AtomicString("test")));
  AppendBufferedParamChanges(param_changes);
  utils_->ProcessStretchContentToFillBounds();
}

TEST_F(HTMLPlugInElementUtilsTest, HandlesEmptyValue) {
  auto params = CreateParams(AtomicString(""));
  utils_->AnalysisStretchContentToFillBounds(params);
}

TEST_F(HTMLPlugInElementUtilsTest, ContainsObjectFitNone) {
  AtomicString aligned_str = CreateAlignedAtomicString("object-fit:none");
  auto params = CreateParams(aligned_str);
  utils_->AnalysisStretchContentToFillBounds(params);
}

TEST_F(HTMLPlugInElementUtilsTest, ContainsObjectFitStretch) {
  AtomicString aligned_str = CreateAlignedAtomicString("object-fit:stretch");
  auto params = CreateParams(aligned_str);
  utils_->AnalysisStretchContentToFillBounds(params);
}

TEST_F(HTMLPlugInElementUtilsTest, HandlesInvalidValue) {
  AtomicString aligned_str = CreateAlignedAtomicString("invalid_value");
  auto params = CreateParams(aligned_str);
  utils_->AnalysisStretchContentToFillBounds(params);
}

}  // namespace blink