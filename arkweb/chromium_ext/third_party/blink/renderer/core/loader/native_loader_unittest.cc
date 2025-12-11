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

#include "arkweb/chromium_ext/media/mojo/mojom/native_bridge.mojom.h"
#include "arkweb/chromium_ext/third_party/blink/renderer/core/html/html_plugin_element_utils.h"
#include "arkweb/chromium_ext/third_party/blink/renderer/core/frame/local_frame_client_impl_utils.h"
#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "third_party/blink/renderer/core/frame/frame_test_helpers.h"
#include "third_party/blink/renderer/core/frame/settings.h"
#include "third_party/blink/renderer/core/html/html_embed_element.h"
#include "third_party/blink/renderer/core/html/html_native_loader.h"
#include "third_party/blink/renderer/core/html/html_plugin_element.h"
#include "third_party/blink/renderer/core/loader/native_loader.h"
#include "third_party/blink/renderer/core/testing/page_test_base.h"

using ::testing::_;


namespace blink {

class MockFrameClient : public LocalFrameClientImplUtils {
 public:
  MOCK_METHOD2(CreateWebNativeBridge,
               std::unique_ptr<WebNativeBridge>(WebNativeClient&, NativeLoader*));
};

class MockCcLayer : public cc::Layer {
 public:
  explicit MockCcLayer() : cc::Layer() {
    layer_utils_ = std::make_unique<cc::LayerUtils>(this);
  }

  MOCK_METHOD1(SetNativeEmbedOverlayInfinity, void(bool));
  MOCK_METHOD1(SetNativeEmbedOverlay, void(bool));
  MOCK_METHOD0(SetNeedsCommit, void());
  MOCK_METHOD0(SetNeedsPushProperties, void());
  MOCK_METHOD0(SetNeedsDisplay, void());

  cc::LayerUtils* layer_utils() const { return layer_utils_.get(); }

 private:
  std::unique_ptr<cc::LayerUtils> layer_utils_;
};

class MockWebNativeBridge : public WebNativeBridge {
 public:
  MOCK_METHOD0(StartPipeline, void());
  MOCK_METHOD1(OnLayerRectChange, void(const gfx::Rect& rect));
  MOCK_METHOD1(OnLayerRectVisibilityChange, void(bool visibility));
  MOCK_METHOD1(CleanupVisibilityForRemovedLayer, void(bool visibility));
  MOCK_METHOD0(GetDelegateId, int());
  MOCK_CONST_METHOD0(NaturalSize, gfx::Size());
};

class NativeLoaderTest : public PageTestBase {
 protected:
  void SetUp() override {
    EnableCompositing();
    PageTestBase::SetUp();
    web_view_helper_.Initialize();
    GetFrame().GetSettings()->SetNativeEmbedModeEnabled(true);
    GetDocument().body()->setInnerHTML("<embed id='test-plugin' type='test/native'>");
    plugin_element = DynamicTo<HTMLPlugInElement>(GetDocument().getElementById(AtomicString("test-plugin")));
    loader_ = MakeGarbageCollected<HTMLNativeLoader>(plugin_element);
  }

  void TearDown() override {
    web_view_helper_.Reset();
  }

  Document& GetDocument() {
    return *static_cast<Document*>(
        web_view_helper_.LocalMainFrame()->GetDocument());
  }

  gfx::Rect TransformRect(gfx::Rect rect) {
    return loader_->TransformRect(rect);
  }

  void ClearNativeResource() {
    loader_->ClearNativeResource();
  }

  void SetFirstUpdateVisibility(bool value) {
    loader_->first_update_visibility_ = value;
  }

  void AddNativeBridgeObserverAndPassReceiver() {
    loader_->AddNativeBridgeObserverAndPassReceiver();
  }

  void ResetMojoState() {
    loader_->ResetMojoState();
  }

  void ContextDestroyed() {
    loader_->ContextDestroyed();
  }

  void GetNativeBridgeHostRemote() {
    loader_->GetNativeBridgeHostRemote();
  }

  void ReportFirstPaintTime(
    const viz::FrameTimingDetails& frame_timing_details) {
    loader_->ReportFirstPaintTime(frame_timing_details);
  }

  void SetParamUpdateTaskPending(bool flag) {
    loader_->param_update_task_pending_ = flag;
  }

  void SetNativeEmbedId(int id) {
    loader_->native_embed_id_ = id;
  }

  HTMLPlugInElement* plugin_element;
  Persistent<HTMLNativeLoader> loader_;
  MockCcLayer mock_cc_layer_;
  frame_test_helpers::WebViewHelper web_view_helper_;
};

TEST_F(NativeLoaderTest, LoadResource_WithValidFrame) {
  loader_->ScheduleLoadResource();
}

TEST_F(NativeLoaderTest, TransformRect_WithLayoutObject) {
  GetDocument().UpdateStyleAndLayout(DocumentUpdateReason::kTest);
  gfx::Rect input_rect(10, 10, 100, 100);
  gfx::Rect result = TransformRect(input_rect);
  EXPECT_FALSE(result.IsEmpty());
}

TEST_F(NativeLoaderTest, TransformRect_WithoutLayoutObject) {
  loader_->current_plugin_element()->remove();
  gfx::Rect input_rect(10, 10, 100, 100);
  gfx::Rect result = TransformRect(input_rect);
  EXPECT_EQ(result, input_rect);
}

TEST_F(NativeLoaderTest, OnCreateNativeSurface_WithCcLayer) {
  loader_->SetCcLayer(&mock_cc_layer_);
  loader_->OnCreateNativeSurface(123, base::DoNothing());
}

TEST_F(NativeLoaderTest, OnCreateNativeSurface_WithCcLayer001) {
  loader_->SetCcLayer(&mock_cc_layer_);
  SetFirstUpdateVisibility(true);
  loader_->OnCreateNativeSurface(123, base::DoNothing());
}

TEST_F(NativeLoaderTest, OnCreateNativeSurface_NoCcLayer) {
  loader_->OnCreateNativeSurface(123, base::DoNothing());
}

TEST_F(NativeLoaderTest, OnLayerRectChange_NoObservers) {
  loader_->SetCcLayer(&mock_cc_layer_);
  loader_->OnLayerRectChange(gfx::Rect(10, 10, 100, 100));
}

TEST_F(NativeLoaderTest, OnLayerRectChange_WithObservers) {
  loader_->SetCcLayer(&mock_cc_layer_);
  loader_->OnCreateNativeSurface(123, base::DoNothing());
  loader_->OnLayerRectChange(gfx::Rect(10, 10, 100, 100));
}

TEST_F(NativeLoaderTest, OnDestroyNativeSurface_WithObservers) {
  loader_->SetCcLayer(&mock_cc_layer_);
  loader_->OnCreateNativeSurface(123, base::DoNothing());
  loader_->OnDestroyNativeSurface();
}

TEST_F(NativeLoaderTest, OnDestroyNativeSurface_NoObservers) {
  loader_->SetCcLayer(&mock_cc_layer_);
  loader_->OnDestroyNativeSurface();
}

TEST_F(NativeLoaderTest, Repaint_WithCcLayer) {
  loader_->SetCcLayer(&mock_cc_layer_);
  loader_->Repaint();
}

TEST_F(NativeLoaderTest, Repaint_NoCcLayer) {
  loader_->Repaint();
}

TEST_F(NativeLoaderTest, SetCcLayer_SameLayer) {
  loader_->SetCcLayer(&mock_cc_layer_);
  EXPECT_CALL(mock_cc_layer_, SetNeedsPushProperties()).Times(0);
  loader_->SetCcLayer(&mock_cc_layer_);
}

TEST_F(NativeLoaderTest, SetCcLayer_OverlayInfinityType) {
  auto* embed = To<HTMLEmbedElement>(GetDocument().getElementById(AtomicString("test-plugin")));
  embed->setAttribute(html_names::kTypeAttr, AtomicString("native/overlay-infinity"));
  loader_->SetCcLayer(&mock_cc_layer_);
}

TEST_F(NativeLoaderTest, SetCcLayer_StandardOverlayType) {
  auto* embed = To<HTMLEmbedElement>(GetDocument().getElementById(AtomicString("test-plugin")));
  embed->setAttribute(html_names::kTypeAttr, AtomicString("native/overlay"));
  loader_->SetCcLayer(&mock_cc_layer_);
}

TEST_F(NativeLoaderTest, SetCcLayer_NonOverlayType) {
  auto* embed = To<HTMLEmbedElement>(GetDocument().getElementById(AtomicString("test-plugin")));
  embed->setAttribute(html_names::kTypeAttr, AtomicString("native/normal"));
  EXPECT_CALL(mock_cc_layer_, SetNativeEmbedOverlayInfinity(_)).Times(0);
  EXPECT_CALL(mock_cc_layer_, SetNativeEmbedOverlay(_)).Times(0);
  loader_->SetCcLayer(&mock_cc_layer_);
}

TEST_F(NativeLoaderTest, ClearNativeResource) {
  loader_->SetCcLayer(&mock_cc_layer_);
  ClearNativeResource();
  EXPECT_EQ(loader_->GetWebNativeBridge(), nullptr);
}

TEST_F(NativeLoaderTest, CleanupVisibilityForRemovedLayer_Enabled) {
  loader_->CleanupVisibilityForRemovedLayer(true);
}

TEST_F(NativeLoaderTest, NotifyVisibilityChange_NoEmbedId) {
  loader_->NotifyVisibilityChange(true);
}

TEST_F(NativeLoaderTest, SetNativeEmbedOverlayInfinity_WithCcLayer) {
  loader_->SetCcLayer(&mock_cc_layer_);
  EXPECT_CALL(mock_cc_layer_, SetNativeEmbedOverlayInfinity(true));
  loader_->SetNativeEmbedOverlayInfinity(true);
}

TEST_F(NativeLoaderTest, SetNativeEmbedOverlay_WithCcLayer) {
  loader_->SetCcLayer(&mock_cc_layer_);
  EXPECT_CALL(mock_cc_layer_, SetNativeEmbedOverlay(true));
  loader_->SetNativeEmbedOverlay(true);
}

TEST_F(NativeLoaderTest, ResetMojoState) {
  AddNativeBridgeObserverAndPassReceiver();
  ResetMojoState();
}

TEST_F(NativeLoaderTest, ContextDestroyed) {
  ContextDestroyed();
}

TEST_F(NativeLoaderTest, GetNativeBridgeHostRemote) {
  GetNativeBridgeHostRemote();
}

TEST_F(NativeLoaderTest, ReportFirstPaintTime) {
  viz::FrameTimingDetails frame_timing_details;
  ReportFirstPaintTime(frame_timing_details);
}

TEST_F(NativeLoaderTest, ProcessParamChanges001) {
  Vector<ParamChangeInfo> param_changes;
  param_changes.push_back(ParamChangeInfo(ParamChangeInfo::Status::kAdd,
                                          AtomicString("test"),
                                          AtomicString("test"),
                                          AtomicString("test")));
  param_changes.push_back(ParamChangeInfo(ParamChangeInfo::Status::kUpdate,
                                          AtomicString("test"),
                                          AtomicString("test"),
                                          AtomicString("test")));
  param_changes.push_back(ParamChangeInfo(ParamChangeInfo::Status::kDelete,
                                          AtomicString("test"),
                                          AtomicString("test"),
                                          AtomicString("test")));
  loader_->ProcessParamChanges(param_changes);
}

TEST_F(NativeLoaderTest, ProcessParamChanges002) {
  Vector<ParamChangeInfo> param_changes;
  param_changes.push_back(ParamChangeInfo(ParamChangeInfo::Status::kAdd,
                                          AtomicString("test"),
                                          AtomicString("test"),
                                          AtomicString("test")));
  param_changes.push_back(ParamChangeInfo(ParamChangeInfo::Status::kUpdate,
                                          AtomicString("test"),
                                          AtomicString("test"),
                                          AtomicString("test")));
  param_changes.push_back(ParamChangeInfo(ParamChangeInfo::Status::kDelete,
                                          AtomicString("test"),
                                          AtomicString("test"),
                                          AtomicString("test")));
  SetParamUpdateTaskPending(true);
  loader_->ProcessParamChanges(param_changes);
}

TEST_F(NativeLoaderTest, ProcessPendingParamChanges001) {
  SetNativeEmbedId(-1);
  loader_->ProcessPendingParamChanges();
}

TEST_F(NativeLoaderTest, ProcessPendingParamChanges002) {
  SetNativeEmbedId(1);
  loader_->ProcessPendingParamChanges();
}

TEST_F(NativeLoaderTest, SetStretchContentToFillBounds) {
  loader_->SetStretchContentToFillBounds(false);
  bool result = loader_->GetStretchContentToFillBounds();
  EXPECT_FALSE(result);
}

}  // namespace blink