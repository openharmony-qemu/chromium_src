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

#include "third_party/blink/renderer/core/paint/native_painter.h"
#include "third_party/blink/renderer/core/paint/paint_info.h"
#include "third_party/blink/renderer/core/testing/core_unit_test_helper.h"
#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "third_party/blink/renderer/core/html/html_embed_element.h"
#include "third_party/blink/renderer/core/html/html_native_loader.h"
#include "third_party/blink/renderer/core/html/html_plugin_element.h"
#include "third_party/blink/renderer/core/layout/layout_native.h"
#include "third_party/blink/renderer/core/frame/frame_test_helpers.h"
#include "third_party/blink/renderer/platform/graphics/paint/paint_controller.h"

namespace blink {
namespace {

class MockLayoutNative : public LayoutImage {
 public:
  explicit MockLayoutNative(Element* element) : LayoutImage(element) {}
  MOCK_METHOD(HTMLPlugInElement*, PluginElement, (), (const));
  const char* GetName() const override { return "MockLayoutNative"; }
};

class TestNativeLoader : public NativeLoader {
 public:
  explicit TestNativeLoader(HTMLPlugInElement* plugin)
      : NativeLoader(plugin) {}

  cc::Layer* CcLayer() const { return cc_layer_; }

  WebNativeBridge* GetWebNativeBridge() { return nullptr; }
  String DebugName() const override { return "TestNativeLoader"; }

  cc::Layer* cc_layer_ = nullptr;
};

class TestHTMLPlugInElement : public HTMLPlugInElement {
 public:
  explicit TestHTMLPlugInElement(Document& doc) 
      : HTMLPlugInElement(html_names::kEmbedTag, doc, CreateElementFlags()) {}

  TestNativeLoader* NativeLoader() const { return loader_; }
  void SetNativeLoader(TestNativeLoader* loader) { loader_ = loader; }
  bool HasFallbackContent() const override { return false; }
  bool UseFallbackContent() const override { return false; }
  LayoutEmbeddedContent* LayoutEmbeddedContentForJSBindings() const override {}
  LayoutEmbeddedContent* ExistingLayoutEmbeddedContent() const override {}
  void UpdatePluginInternal() override {}

  FrameOwnerElementType OwnerType() const override {
    return FrameOwnerElementType::kNone;
  }

  Member<TestNativeLoader> loader_;
};

class NativePainterTest : public RenderingTest {
 protected:
  void SetUp() override {
    EnableCompositing();
    RenderingTest::SetUp();
    web_view_helper_.Initialize();
    auto* element = MakeGarbageCollected<HTMLEmbedElement>(GetDocument());
    GetDocument().body()->appendChild(element);
    layout_native_ = MakeGarbageCollected<LayoutNative>(element);
    plugin_element_ = MakeGarbageCollected<TestHTMLPlugInElement>(GetDocument());
    native_loader_ = MakeGarbageCollected<TestNativeLoader>(plugin_element_);
    plugin_element_->SetNativeLoader(native_loader_);
  }

  Document& GetDocument() {
    return *static_cast<Document*>(
        web_view_helper_.LocalMainFrame()->GetDocument());
  }

  Persistent<LayoutNative> layout_native_;
  Persistent<TestHTMLPlugInElement> plugin_element_;
  Persistent<TestNativeLoader> native_loader_;
  frame_test_helpers::WebViewHelper web_view_helper_;
};

TEST_F(NativePainterTest, SkipNonForegroundPhase) {
  PaintController controller;
  GraphicsContext context(controller);
  PaintInfo paint_info(context, CullRect(), PaintPhase::kBlockBackground, false);

  NativePainter painter(*layout_native_);
  painter.PaintReplaced(paint_info, PhysicalOffset());
}

TEST_F(NativePainterTest, MissingNativeLoader) {
  plugin_element_->SetNativeLoader(nullptr);

  PaintController controller;
  GraphicsContext context(controller);
  PaintInfo paint_info(context, CullRect(), PaintPhase::kForeground, false);

  NativePainter painter(*layout_native_);
  painter.PaintReplaced(paint_info, PhysicalOffset());
}

TEST_F(NativePainterTest, PaintWithForeignLayer) {
  auto layer = cc::Layer::Create();
  native_loader_->cc_layer_ = layer.get();

  PaintController controller;
  GraphicsContext context(controller);
  PaintInfo paint_info(context, CullRect(), PaintPhase::kForeground, false);

  NativePainter painter(*layout_native_);
  painter.PaintReplaced(paint_info, PhysicalOffset());
}

TEST_F(NativePainterTest, SoftwarePaintPath) {
  PaintController controller;
  GraphicsContext context(controller);
  PaintInfo paint_info(context,
                       CullRect(),
                       PaintPhase::kForeground,
                       true,
                       PaintFlag::kOmitCompositingInfo);

  NativePainter painter(*layout_native_);
  painter.PaintReplaced(paint_info, PhysicalOffset());
}

TEST_F(NativePainterTest, AllowSelectionDragImagePhase) {
  PaintController controller;
  GraphicsContext context(controller);
  PaintInfo paint_info(context, CullRect(), PaintPhase::kSelectionDragImage, false);

  NativePainter painter(*layout_native_);
  painter.PaintReplaced(paint_info, PhysicalOffset());
}
}  // namespace
}  // namespace blink